#include <imgui.h>
#include <imgui_impl_sdl.h>
#include "base.h"
#include "cpu.h"
#include "registers_viewer.h"

//--------------------------------------------
// --
//--------------------------------------------
RegistersViewer::RegistersViewer(const CPU &_cpu) : mCpu(_cpu)
{

}

//--------------------------------------------
// --
//--------------------------------------------
void RegistersViewer::Render()
{
    ImGui::SetNextWindowPos(ImVec2(405, 0));

    if (ImGui::Begin("Registers", nullptr, ImVec2(250, 300), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
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