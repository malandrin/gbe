#include <imgui.h>
#include <imgui_impl_sdl.h>
#include "base.h"
#include "mmu.h"
#include "cpu.h"
#include "debugger.h"
#include "instructions_viewer.h"

//--------------------------------------------
// --
//--------------------------------------------
InstructionsViewer::InstructionsViewer(const MMU &_mmu, CPU &_cpu, Debugger &_debugger) : mMmu(_mmu), mCpu(_cpu), mROMWalker(_mmu.GetActiveRom(), _mmu.GetActiveRomSize(), _mmu.GetActiveRomEntryAddr()), mDebugger(_debugger)
{
    CalculateInstructionLines();
}

//--------------------------------------------
// --
//--------------------------------------------
InstructionsViewer::~InstructionsViewer()
{
    if (mPCLineInfo != nullptr)
        delete[] mPCLineInfo;
}

//--------------------------------------------
// --
//--------------------------------------------
bool InstructionsViewer::OnStep()
{
    u16 pc = mCpu.GetRegPC();
    mPrevActiveLineIdx = mActiveLineIdx;
    mActiveLineIdx = mPCLineInfo[pc] >> 1;

    if ((mPCLineInfo[pc] & 1) != 0)
        return true;

    return false;
}

//--------------------------------------------
// --
//--------------------------------------------
void InstructionsViewer::Render()
{
	float wndWidth = 400.0f;
	float wndHeight = 468.0f;

    ImGui::SetNextWindowPos(ImVec2(0, 0));

    if (ImGui::Begin("Instructions", nullptr, ImVec2(wndWidth, wndHeight), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
    {
        float lineHeight = ImGui::GetTextLineHeight();

        ImGui::BeginChild("##instructions_scrolling", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImGuiListClipper clipper(mInstructionLines.size(), lineHeight);

        // ...
		if ((mPrevActiveLineIdx != mActiveLineIdx) && ((mActiveLineIdx < clipper.DisplayStart) || (mActiveLineIdx >= clipper.DisplayEnd)))
		{
            if (((mActiveLineIdx - mPrevActiveLineIdx) == 1) && (abs(mActiveLineIdx - clipper.DisplayEnd) <= 2))
                ImGui::SetScrollY(ImGui::GetScrollY() + (lineHeight * 3));
            else
                ImGui::SetScrollY(mActiveLineIdx * lineHeight);

			mPrevActiveLineIdx = mActiveLineIdx;
		}

        // ...
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            InstructionLine *line = &mInstructionLines[i];
			ImVec2 screenPos = ImGui::GetCursorScreenPos();

            // colors
			if (i == mSelectedLineIdx)
				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0, screenPos.y), ImVec2(wndWidth, screenPos.y + lineHeight), ImColor(127, 127, 127, 255));

            if ((mPCLineInfo[line->addr] & 1) != 0)
				ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(15, screenPos.y + (lineHeight / 2.0f)), 5.0f, ImColor(255, 0, 0, 255));

            if (i == mActiveLineIdx)
            {
				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(18, screenPos.y + 5.0f),
														  ImVec2(25.0f, screenPos.y + lineHeight - 5.0f), ImColor(255, 255, 0, 255));

				ImGui::GetWindowDrawList()->AddTriangleFilled(ImVec2(25.0f, screenPos.y + 3.0f), ImVec2(25.0f, screenPos.y + lineHeight - 3.0f),
															  ImVec2(30.0f, screenPos.y + (lineHeight / 2.0f)), ImColor(255, 255, 0, 255));
            }

			// margin
			ImGui::Text("    ");
			ImGui::SameLine();

            screenPos = ImGui::GetCursorScreenPos();

            if (ImGui::IsMouseDoubleClicked(0))
            {
				ImGuiStyle &style = ImGui::GetStyle();
                ImVec2 mousePos = ImGui::GetMousePos();

				if ((mousePos.x < wndWidth - style.ScrollbarSize - style.WindowPadding.x) &&
					(mousePos.y >= screenPos.y) && (mousePos.y <= (screenPos.y + lineHeight - 1)))
				{
                    ToggleBreakpoint(line->addr);
				}
            }
			else if (ImGui::IsMouseClicked(0))
			{
				ImGuiStyle &style = ImGui::GetStyle();
				ImVec2 mousePos = ImGui::GetMousePos();

				if ((mousePos.x < wndWidth - style.ScrollbarSize - style.WindowPadding.x) &&
					(mousePos.y >= screenPos.y) && (mousePos.y <= (screenPos.y + lineHeight - 1)))
				{
					mSelectedLineIdx = i;
				}
			}

			ImGui::GetWindowDrawList()->AddLine(ImVec2(screenPos.x, screenPos.y), ImVec2(screenPos.x, screenPos.y + lineHeight), ImColor(ImGui::GetStyle().Colors[ImGuiCol_Border]));

			// address
            ImGui::Text(" %0*X: ", mAddrDigitCount, line->addr);
			ImGui::SameLine();

			// opcode
			for (int b = 0; b < line->numBytes; ++b)
			{
				ImGui::Text("%02X ", mMmu.ReadU8(line->addr + b));
				ImGui::SameLine();
			}

			ImGui::SameLine(200);

			// asm code
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
			ImGui::Text(line->asmCode.c_str());
			ImGui::PopStyleColor();
        }

        clipper.End();
        ImGui::PopStyleVar(2);
        ImGui::EndChild();

        // debug buttons
        ImGui::Separator();

        if (mCpu.IsOnDebugMode())
        {
    		if (ImGui::Button("Step"))
    			mDebugger.Step();

            ImGui::SameLine();
            if (ImGui::Button("Continue"))
                mDebugger.Continue();
        }
        else
        {
            if (ImGui::Button("Break"))
                mDebugger.Break();
        }
    }

    ImGui::End();
}

//--------------------------------------------
// --
//--------------------------------------------
void InstructionsViewer::ToggleBreakpoint(u16 _addr)
{
    u8 val = mPCLineInfo[_addr] & 1;

    if (val == 0)
        mPCLineInfo[_addr] |= 1;
    else
        mPCLineInfo[_addr] &= ~1;
}

//--------------------------------------------
// --
//--------------------------------------------
void InstructionsViewer::CalculateInstructionLines()
{
    int instrMemSize = mMmu.GetActiveRomSize();

	mInstructionLines.clear();

    if (mPCLineInfo != nullptr)
        delete[] mPCLineInfo;

    mPCLineInfo = new int[instrMemSize] {0};

    // ...
    int i = 0;

    while(i < instrMemSize)
    {
        u16 addr = i;
        u8  opcode = mMmu.ReadU8(i++);

        if (!mROMWalker.IsCode(addr))
            mInstructionLines.push_back(InstructionLine(addr, 1, string(".DB " + Int2Hex(opcode))));
        else 
        {
            switch(opcode)
            {
                case 0x00: // NOP
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("NOP")));
                    break;

                case 0x03: // INC BC
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("INC BC")));
                    break;

                case 0x04: // INC B
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("INC B")));
                    break;

                case 0x05: // DEC B
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("DEC B")));
                    break;

                case 0x06: // LD B, n
                    mInstructionLines.push_back(InstructionLine(addr, 2, string("LD B, " + Int2Hex(mMmu.ReadU8(i++)))));
                    break;

                case 0x08: // LD (nn), SP
                    mInstructionLines.push_back(InstructionLine(addr, 3, string("LD (" + Int2Hex(mMmu.ReadU16(i)) + "), SP")));
                    i += 2;
                    break;

                case 0x0B: // DEC BC
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("DEC BC")));
                    break;

                case 0x0C: // INC C
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("INC C")));
                    break;

                case 0x0D: // DEC C
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("DEC C")));
                    break;

                case 0x0E: // LD C, n
                    mInstructionLines.push_back(InstructionLine(addr, 2, string("LD C, ") + Int2Hex(mMmu.ReadU8(i++))));
                    break;

                case 0x11: // LD DE, nn
                    mInstructionLines.push_back(InstructionLine(addr, 3, string("LD DE, ") + Int2Hex(mMmu.ReadU16(i))));
                    i += 2;
                    break;

                case 0x13: // INC DE
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("INC DE")));
                    break;

                case 0x15: // DEC D
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("DEC D")));
                    break;

                case 0x16: // LD D, n
                    mInstructionLines.push_back(InstructionLine(addr, 2, string("LD D, ") + Int2Hex(mMmu.ReadU8(i++))));
                    break;

                case 0x17: // RLA
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("RLA")));
                    break;

                case 0x18: // JR r8
                    mInstructionLines.push_back(InstructionLine(addr, 2, string("JR :") + Int2Hex(i + 1 + (i8)mMmu.ReadU8(i))));
                    i += 1;
                    break;

                case 0x1A: // LD A, (DE)
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD A, (DE)")));
                    break;

                case 0x1C: // INC E
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("INC E")));
                    break;

                case 0x1D: // DEC E
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("DEC E")));
                    break;

                case 0x1E: // LD E, n
                    mInstructionLines.push_back(InstructionLine(addr, 2, string("LD E, ") + Int2Hex(mMmu.ReadU8(i++))));
                    break;

                case 0x20: // JR NZ, r8
                    mInstructionLines.push_back(InstructionLine(addr, 2, string("JR NZ, :") + Int2Hex(i + 1 + (i8)mMmu.ReadU8(i))));
                    i += 1;
                    break;

                case 0x21: // LD HL, nn
                    mInstructionLines.push_back(InstructionLine(addr, 3, string("LD HL, ") + Int2Hex(mMmu.ReadU16(i))));
                    i += 2;
                    break;

                case 0x22: // LD (HL+), A
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD (HL+), A")));
                    break;

                case 0x23: // INC HL
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("INC HL")));
                    break;

                case 0x24: // INC H
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("INC H")));
                    break;

                case 0x28: // JR Z, r8
                    mInstructionLines.push_back(InstructionLine(addr, 2, string("JR Z, :") + Int2Hex(i + 1 + (i8)mMmu.ReadU8(i))));
                    i += 1;
                    break;

                case 0x2E: // LD L, n
                    mInstructionLines.push_back(InstructionLine(addr, 2, string("LD L, ") + Int2Hex(mMmu.ReadU8(i++))));
                    break;

                case 0x31: // LD SP, nn
                    mInstructionLines.push_back(InstructionLine(addr, 3, string("LD SP, ") + Int2Hex(mMmu.ReadU16(i))));
				    i += 2;
                    break;

                case 0x32: // LD (HL-), A
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD (HL-), A")));
                    break;

                case 0x33: // INC SP
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("INC SP")));
                    break;

                case 0x3C: // INC A
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("INC A")));
                    break;

                case 0x3D: // DEC A
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("DEC A")));
                    break;

                case 0x3E: // LD A, n
                    mInstructionLines.push_back(InstructionLine(addr, 2, string("LD A, ") + Int2Hex(mMmu.ReadU8(i++))));
                    break;

                case 0x42: // LD B, D
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD B, D")));
                    break;

                case 0x4F: // LD C, A
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD C, A")));
                    break;

                case 0x57: // LD D, A
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD D, A")));
                    break;

                case 0x63: // LD H, E
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD H, E")));
                    break;

                case 0x66: // LD H, (HL)
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD H, (HL)")));
                    break;

                case 0x67: // LD H, A
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD H, A")));
                    break;

                case 0x6E: // LD L, (HL)
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD L, (HL)")));
                    break;

                case 0x73: // LD (HL), E
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD (HL), E")));
                    break;

                case 0x77: // LD (HL), A
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD (HL), A")));
                    break;

                case 0x78: // LD A, B
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD A, B")));
                    break;

                case 0x7B: // LD A, E
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD A, E")));
                    break;

                case 0x7C: // LD A, H
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD A, H")));
                    break;

                case 0x7D: // LD A, L
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD A, L")));
                    break;

                case 0x83: // LD A, E
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD A, E")));
                    break;

                case 0x86: // ADD A, (HL)
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("ADD A, (HL)")));
                    break;

                case 0x88: // ADC A, B
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("ADC A, B")));
                    break;

                case 0x89: // ADC A, C
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("ADC A, C")));
                    break;

                case 0x90: // SUB B
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("SUB B")));
                    break;

                case 0x99: // SBC A, C
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("SBC A, C")));
                    break;

                case 0x9F: // SBC A, A
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("SBC A, A")));
                    break;

                case 0xA5: // AND L
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("AND L")));
                    break;

                case 0xAF: // XOR A
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("XOR A")));
                    break;

                case 0xB9: // CP C
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("CP C")));
                    break;

                case 0xBB: // CP E
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("CP E")));
                    break;

                case 0xBE: // CP (HL)
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("CP (HL)")));
                    break;

                case 0xC1: // POP BC
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("POP BC")));
                    break;

                case 0xC5: // PUSH BC
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("PUSH BC")));
                    break;

                case 0xC9: // RET
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("RET")));
                    break;

                case 0xCB: // CB
                    ProcessCb(mMmu.ReadU8(i++), addr);
                    break;

                case 0xCC: // CALL Z, nn
                    mInstructionLines.push_back(InstructionLine(addr, 3, string("CALL Z, " + Int2Hex(mMmu.ReadU16(i)))));
                    i += 2;
                    break;

                case 0xCD: // CALL nn
                    mInstructionLines.push_back(InstructionLine(addr, 3, string("CALL " + Int2Hex(mMmu.ReadU16(i)))));
                    i += 2;
                    break;

                case 0xCE: // ADC A, n
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("ADC A, " + Int2Hex(mMmu.ReadU8(i++)))));
                    break;

                case 0xD9: // RETI
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("RETI")));
                    break;

                case 0xE0: // LD (0xFF00 + n), A
                    mInstructionLines.push_back(InstructionLine(addr, 2, string("LD (FF00 + " + Int2Hex(mMmu.ReadU8(i++)) + "), A")));
                    break;

                case 0xEA: // LD (nn), A
                    mInstructionLines.push_back(InstructionLine(addr, 3, string("LD (" + Int2Hex(mMmu.ReadU16(i)) + "), A")));
                    i += 2;
                    break;

                case 0xE2: // LD (0xFF00 + C), A
                    mInstructionLines.push_back(InstructionLine(addr, 1, string("LD (FF00 + C), A")));
                    break;

                case 0xF0: // LD A, (0xFF00 + n)
                    mInstructionLines.push_back(InstructionLine(addr, 2, string("LD A, (FF00 + " + Int2Hex(mMmu.ReadU8(i++)) + ")")));
                    break;

                case 0xFE: // CP n
                    mInstructionLines.push_back(InstructionLine(addr, 2, string("CP " + Int2Hex(mMmu.ReadU8(i++)))));
                    break;

			    default:
				    mInstructionLines.push_back(InstructionLine(addr, 1, string("OPCODE UNKNOW")));
				    break;
            }

            mPCLineInfo[addr] = (mInstructionLines.size() - 1) << 1;
        }
    }

    mAddrDigitCount = 0;

    for (int n = instrMemSize - 1; n > 0; n >>= 4)
        ++mAddrDigitCount;
}

//--------------------------------------------
// --
//--------------------------------------------
void InstructionsViewer::ProcessCb(u8 _opcode, u16 _addr)
{
    switch(_opcode)
    {
        case 0x11: // RL C
            mInstructionLines.push_back(InstructionLine(_addr, 2, string("RL C")));
            break;

        case 0x7C: // BIT 7, H
            mInstructionLines.push_back(InstructionLine(_addr, 2, string("BIT 7, H")));
            break;
    }
}