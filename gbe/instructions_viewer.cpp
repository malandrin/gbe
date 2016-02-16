#include <imgui.h>
#include <imgui_impl_sdl.h>
#include "base.h"
#include "mmu.h"
#include "instructions_viewer.h"

//--------------------------------------------
// --
//--------------------------------------------
InstructionsViewer::InstructionsViewer(const MMU &_mmu) : mMmu(_mmu)
{
    CalculateInstructionLines();
}

//--------------------------------------------
// --
//--------------------------------------------
void InstructionsViewer::Render()
{
	float wndWidth = 468.0f;

    ImGui::SetNextWindowPos(ImVec2(0, 0));

    if (ImGui::Begin("Instructions", nullptr, ImVec2(400.0f, wndWidth), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
    {
        float lineHeight = ImGui::GetTextLineHeight();

        ImGui::BeginChild("##instructions_scrolling", ImVec2(0, 0));

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
				ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(25, screenPos.y + (lineHeight / 2.0f)), 5.0f, ImColor(255, 0, 0, 255));

            if (i == mActiveLineIdx)
            {
				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(20, screenPos.y + 5.0f),
														  ImVec2(30.0f, screenPos.y + lineHeight - 5.0f), ImColor(255, 255, 0, 255));

				ImGui::GetWindowDrawList()->AddTriangleFilled(ImVec2(30.0f, screenPos.y + 3.0f), ImVec2(30.0f, screenPos.y + lineHeight - 3.0f),
															  ImVec2(35.0f, screenPos.y + (lineHeight / 2.0f)), ImColor(255, 255, 0, 255));
            }

			// margin
			ImGui::Text("    ");
			ImGui::SameLine();

            if (ImGui::IsMouseDoubleClicked(0))
            {
                ImVec2 mousePos = ImGui::GetMousePos();
                if ((mousePos.y >= screenPos.y) && (mousePos.y <= (screenPos.y + lineHeight - 1)))
                    ToggleBreakpoint(i);
            }
			else if (ImGui::IsMouseClicked(0))
			{
				ImVec2 mousePos = ImGui::GetMousePos();
				if ((mousePos.y >= screenPos.y) && (mousePos.y <= (screenPos.y + lineHeight - 1)))
					mSelectedLineIdx = i;
			}

			ImGui::GetWindowDrawList()->AddLine(ImVec2(screenPos.x, screenPos.y), ImVec2(screenPos.x, screenPos.y + lineHeight), ImColor(ImGui::GetStyle().Colors[ImGuiCol_Border]));

			// address
            ImGui::Text(" %0*X: ", mAddrDigitCount, line->pos);
			ImGui::SameLine();

			// opcode
			for (int b = 0; b < line->numBytes; ++b)
			{
				ImGui::Text("%X ", mMmu.ReadU8(line->pos + b));
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
    int instrMemSize = mMmu.IsInBootableRom() ? MMU::BootableRomSize : mMmu.GetRomSize();
    int i = 0;

    while(i < instrMemSize)
    {
        int pos = i;
        u8 opcode = mMmu.ReadU8(i++);

        switch(opcode)
        {
            case 0x21: // LD HL, nn
                mInstructionLines.push_back(InstructionLine(pos, 3, string("LD HL, ") + Int2Hex(mMmu.ReadU16(i))));
                i += 2;
                break;

            case 0x31: // LD SP, nn
                mInstructionLines.push_back(InstructionLine(pos, 3, string("LD SP, ") + Int2Hex(mMmu.ReadU16(i))));
				i += 2;
                break;

            case 0x32: // LD (HL-), A
                mInstructionLines.push_back(InstructionLine(pos, 1, string("LD (HL-), A")));
                break;

            case 0xAF: // XOR A
                mInstructionLines.push_back(InstructionLine(pos, 1, string("XOR A")));
                break;

			default:
				mInstructionLines.push_back(InstructionLine(pos, 1, string("OPCODE UNKNOW")));
				break;
        }
    }

    mAddrDigitCount = 0;

    for (int n = instrMemSize - 1; n > 0; n >>= 4)
        ++mAddrDigitCount;
}