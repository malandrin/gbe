#include <imgui.h>
#include <imgui_impl_sdl.h>
#include "base.h"
#include "mmu.h"
#include "cpu.h"
#include "instructions_viewer.h"

//--------------------------------------------
// --
//--------------------------------------------
InstructionsViewer::InstructionsViewer(const MMU &_mmu, CPU &_cpu) : mMmu(_mmu), mCpu(_cpu)
{
    CalculateInstructionLines();
	mCpu.AddListener(this);
}

//--------------------------------------------
// --
//--------------------------------------------
void InstructionsViewer::OnStep()
{
    u16 pc = mCpu.GetRegPC();
    mActiveLineIdx = mMapAddr2Line[pc];

    if (mBreakpoints.find(mActiveLineIdx) != mBreakpoints.end())
        mCpu.Break();
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

        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            InstructionLine *line = &mInstructionLines[i];
			ImVec2 screenPos = ImGui::GetCursorScreenPos();

            // colors
			if (i == mSelectedLineIdx)
				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0, screenPos.y), ImVec2(wndWidth, screenPos.y + lineHeight), ImColor(127, 127, 127, 255));

			if (mBreakpoints.find(i) != mBreakpoints.end())
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
					ToggleBreakpoint(i);
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
    			mCpu.Step();

            ImGui::SameLine();
            if (ImGui::Button("Continue"))
                mCpu.Continue();
        }
        else
        {
            if (ImGui::Button("Break"))
                mCpu.Break();
        }
    }

    ImGui::End();
}

//--------------------------------------------
// --
//--------------------------------------------
void InstructionsViewer::ToggleBreakpoint(int _line)
{
	if (mBreakpoints.find(_line) != mBreakpoints.end())
		mBreakpoints.erase(_line);
	else
		mBreakpoints.insert(_line);
}

//--------------------------------------------
// --
//--------------------------------------------
void InstructionsViewer::CalculateInstructionLines()
{
	mInstructionLines.clear();
    mMapAddr2Line.clear();

    int instrMemSize = mMmu.IsInBootableRom() ? MMU::BootableRomSize : mMmu.GetRomSize();
    int i = 0;

    while(i < instrMemSize)
    {
        u16 addr = i;
        u8  opcode = mMmu.ReadU8(i++);

        switch(opcode)
        {
            case 0x20: // JR NZ, r8
                mInstructionLines.push_back(InstructionLine(addr, 2, string("JR NZ, :") + Int2Hex(i + 1 + (i8)mMmu.ReadU8(i))));
                i += 1;
                break;

            case 0x21: // LD HL, nn
                mInstructionLines.push_back(InstructionLine(addr, 3, string("LD HL, ") + Int2Hex(mMmu.ReadU16(i))));
                i += 2;
                break;

            case 0x31: // LD SP, nn
                mInstructionLines.push_back(InstructionLine(addr, 3, string("LD SP, ") + Int2Hex(mMmu.ReadU16(i))));
				i += 2;
                break;

            case 0x32: // LD (HL-), A
                mInstructionLines.push_back(InstructionLine(addr, 1, string("LD (HL-), A")));
                break;

            case 0xAF: // XOR A
                mInstructionLines.push_back(InstructionLine(addr, 1, string("XOR A")));
                break;

            case 0xCB: // CB
                ProcessCb(mMmu.ReadU8(i++), addr);
                break;

			default:
				mInstructionLines.push_back(InstructionLine(addr, 1, string("OPCODE UNKNOW")));
				break;
        }

        mMapAddr2Line[addr] = mInstructionLines.size() - 1;
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
        case 0x7C: // BIT 7, H
            mInstructionLines.push_back(InstructionLine(_addr, 1, string("BIT 7, H")));
            break;
    }
}