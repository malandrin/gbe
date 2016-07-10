#include <imgui.h>
#include <imgui_impl_sdl.h>
#include "base.h"
#include "gb.h"
#include "debugger.h"
#include "opcodes_info.h"
#include "instructions_viewer.h"

//--------------------------------------------
// --
//--------------------------------------------
InstructionsViewer::InstructionsViewer(GB &_gb, Debugger &_debugger) : mGb(_gb), mROMWalker(_gb.GetMmu().GetActiveRom(), _gb.GetMmu().GetActiveRomSize(), _gb.GetMmu().GetActiveRomEntryAddr()), mDebugger(_debugger)
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
    u16 pc = mGb.GetCpu().GetRegPC();
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
				ImGui::Text("%02X ", mGb.GetMmu().ReadU8(line->addr + b));
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

        if (mGb.GetCpu().IsOnDebugMode())
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
    int instrMemSize = mGb.GetMmu().GetActiveRomSize();

	mInstructionLines.clear();

    if (mPCLineInfo != nullptr)
        delete[] mPCLineInfo;

    mPCLineInfo = new int[instrMemSize] {0};

    // ...
    int i = mGb.GetCpu().GetRegPC();

    while (i < instrMemSize)
    {
        u16 addr = i;
        u8  opcode = mGb.GetMmu().ReadU8(i++);

        //if (!mROMWalker.IsCode(addr))
          //  mInstructionLines.push_back(InstructionLine(addr, 1, string(".DB " + Int2Hex(opcode))));
        //else
        {
            if (opcode == 0xCB)
            {
                u8 opcodeCb = mGb.GetMmu().ReadU8(i);
                mInstructionLines.push_back(InstructionLine(addr, 2, OpcodesInfo::cb[opcodeCb].asmCode(mGb, i + 1)));
                ++i;
            }
            else
                mInstructionLines.push_back(InstructionLine(addr, OpcodesInfo::primary[opcode].bytesLength, OpcodesInfo::primary[opcode].asmCode(mGb, i)));

            mPCLineInfo[addr] = (mInstructionLines.size() - 1) << 1;
        }
    }

    // ...
    mAddrDigitCount = 0;

    for (int n = instrMemSize - 1; n > 0; n >>= 4)
        ++mAddrDigitCount;
}
