#include <imgui.h>
#include <imgui_impl_sdl.h>
#include "base.h"
#include "mmu.h"
#include "memory_viewer.h"

//--------------------------------------------
// --
//--------------------------------------------
void MemoryViewer::Render()
{
	ImGui::SetNextWindowPos(ImVec2(0, 468));
	if (ImGui::Begin("Memory Viewer", nullptr, ImVec2(1024, 300), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::BeginChild("##scrolling", ImVec2(0, 0));

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        int memSize = mMmu.GetRomSize();
        const u8* data = mMmu.GetRom();

		int addrDigitsCount = 0;
		for (int n = memSize - 1; n > 0; n >>= 4)
			addrDigitsCount++;

		float glyphWidth = ImGui::CalcTextSize("F").x;
		float cellWidth = glyphWidth * 3;
		float lineHeight = ImGui::GetTextLineHeight();
		int lineTotalCount = (int)((memSize + mNumCols - 1) / mNumCols);
		ImGuiListClipper clipper(lineTotalCount, lineHeight);

		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		{
			int addr = i * mNumCols;
			ImGui::Text("%0*X: ", addrDigitsCount, addr);
			ImGui::SameLine();

			float lineStartX = ImGui::GetCursorPosX();

			for (int n = 0; n < mNumCols && addr < memSize; n++, addr++)
			{
				ImGui::SameLine(lineStartX + cellWidth * n);
				ImGui::Text("%02X ", data[addr]);
			}

			ImGui::SameLine(lineStartX + cellWidth * mNumCols + glyphWidth * 2);

			ImVec2 screen_pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddLine(ImVec2(screen_pos.x - glyphWidth, screen_pos.y - 9999), ImVec2(screen_pos.x - glyphWidth, screen_pos.y + 9999), ImColor(ImGui::GetStyle().Colors[ImGuiCol_Border]));

			addr = i * mNumCols;
			for (int n = 0; n < mNumCols && addr < memSize; n++, addr++)
			{
				if (n > 0) ImGui::SameLine();
				int c = data[addr];
				ImGui::Text("%c", (c >= 32 && c < 128) ? c : '.');
			}
		}

		clipper.End();
		ImGui::PopStyleVar(2);

		ImGui::EndChild();
	}

	ImGui::End();
}