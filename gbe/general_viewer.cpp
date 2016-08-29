#include <imgui.h>
#include <imgui_impl_sdl.h>
#include "base.h"
#include "defines.h"
#include "cpu.h"
#include "mmu.h"
#include "general_viewer.h"

//--------------------------------------------
// --
//--------------------------------------------
GeneralViewer::GeneralViewer(const CPU &_cpu, const MMU &_mmu) : mCpu(_cpu), mMmu(_mmu)
{

}

//--------------------------------------------
// --
//--------------------------------------------
void GeneralViewer::Render()
{
    RenderRegistersWnd();
    RenderStackWnd();
}

//--------------------------------------------
// --
//--------------------------------------------
void GeneralViewer::RenderStackWnd()
{
    ImGui::SetNextWindowPos(ImVec2(405, 150));

    if (ImGui::Begin("Stack", nullptr, ImVec2(150, 318), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
    {
		u16 sp = mCpu.GetRegSP();

		if (sp != 0)
		{
			float lineHeight = ImGui::GetTextLineHeight();
			int lineTotalCount = (mCpu.GetSPStartAddr() - sp) >> 1;

			ImGui::BeginChild("##stack_scrolling", ImVec2(0, 0));

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

			ImGuiListClipper clipper(lineTotalCount, lineHeight);

			int addr = sp;

			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			{
				ImGui::Text("%0*X", 4, mMmu.ReadU16(addr));
				addr += 2;
			}

			clipper.End();
			ImGui::PopStyleVar(2);
			ImGui::EndChild();
		}
    }

    ImGui::End();
}

//--------------------------------------------
// --
//--------------------------------------------
void GeneralViewer::RenderRegistersWnd()
{
    ImGui::SetNextWindowPos(ImVec2(405, 0));

    if (ImGui::Begin("Registers", nullptr, ImVec2(150, 150), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::Text("A:  ");
        ImGui::SameLine();
        ImGui::Text("%02X", mCpu.GetRegA());

        ImGui::Text("BC: ");
        ImGui::SameLine();
        ImGui::Text("%02X|%02X", mCpu.GetRegB(), mCpu.GetRegC());

        ImGui::Text("DE: ");
        ImGui::SameLine();
        ImGui::Text("%02X|%02X", mCpu.GetRegD(), mCpu.GetRegE());

        ImGui::Text("HL: ");
        ImGui::SameLine();
        ImGui::Text("%02X|%02X", mCpu.GetRegH(), mCpu.GetRegL());

        ImGui::Text("SP: ");
        ImGui::SameLine();
        ImGui::Text("%04X", mCpu.GetRegSP());

        ImGui::Text("PC: ");
        ImGui::SameLine();
        ImGui::Text("%04X", mCpu.GetRegPC());

        ImGui::Text("Flags: ");
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, mCpu.GetFlagZ() ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
        ImGui::Text("Z");
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, mCpu.GetFlagN() ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
        ImGui::Text("N");
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, mCpu.GetFlagH() ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
        ImGui::Text("H");
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, mCpu.GetFlagC() ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
        ImGui::Text("C");
        ImGui::PopStyleColor();
    }

    ImGui::End();
}