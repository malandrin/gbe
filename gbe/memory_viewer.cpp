#include <imgui.h>
#include <imgui_impl_sdl.h>
#include "base.h"
#include "mmu.h"
#include "memory_viewer.h"

// Based on: https://gist.github.com/ocornut/0673e37e54aff644298b

//--------------------------------------------
// --
//--------------------------------------------
MemoryViewer::MemoryViewer(const MMU &_mmu) : mMmu(_mmu)
{
	CalculateMemInfo(true);
}

//--------------------------------------------
// --
//--------------------------------------------
void MemoryViewer::Render()
{
	ImGui::SetNextWindowPos(ImVec2(0, 468));
	if (ImGui::Begin("Memory Viewer", nullptr, ImVec2(1024, 300), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
	{
		float glyphWidth = ImGui::CalcTextSize("F").x;
		float cellWidth = glyphWidth * 3;
		float lineHeight = ImGui::GetTextLineHeight();

		ImGui::BeginChild("##scrolling", ImVec2(0, 0));

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		ImGuiListClipper clipper(mLineTotalCount, lineHeight);

		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		{
			int memIdx = 0;
			MemInfo *memInfo = GetMemInfoByLine(i, memIdx);

			if (memIdx % 2 == 0)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			else
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

			int addr = memInfo->baseAddr + ((i - memInfo->lineStart) * mNumCols);
			ImGui::Text((memInfo->memType + ":%0*X: ").c_str(), mAddrDigitCount, addr);
			ImGui::SameLine();

			float lineStartX = ImGui::GetCursorPosX();

			for (int n = 0; n < mNumCols && addr < mMemSize; n++, addr++)
			{
				ImGui::SameLine(lineStartX + cellWidth * n);
				ImGui::Text("%02X ", memInfo->mem[addr - memInfo->baseAddr]);
			}

			ImGui::SameLine(lineStartX + cellWidth * mNumCols + glyphWidth * 2);

			ImVec2 screen_pos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddLine(ImVec2(screen_pos.x - glyphWidth, screen_pos.y), ImVec2(screen_pos.x - glyphWidth, screen_pos.y + lineHeight), ImColor(ImGui::GetStyle().Colors[ImGuiCol_Border]));

			addr = memInfo->baseAddr + ((i - memInfo->lineStart) * mNumCols);
			for (int n = 0; n < mNumCols && addr < mMemSize; n++, addr++)
			{
				if (n > 0) ImGui::SameLine();
				int c = memInfo->mem[addr - memInfo->baseAddr];
				ImGui::Text("%c", (c >= 32 && c < 128) ? c : '.');
			}

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
void MemoryViewer::CalculateMemInfo(bool _useBootable)
{
	// Bootable rom/rom
	if (_useBootable)
		mMemInfo[0].Set(mMmu.GetBootableRom(), MMU::BootableRomSize, 0, 0, string("BROM"));
	else
		mMemInfo[0].Set(mMmu.GetRom(), mMmu.GetRomSize(), 0, 0, string(" ROM"));

	// vram
	mMemInfo[1].Set(mMmu.GetVRam(), MMU::VRamSize, 0x8000, mMemInfo[0].lineEnd, string("VRAM"));

	// ram
	mMemInfo[2].Set(mMmu.GetRam(), MMU::RamSize, 0xC000, mMemInfo[1].lineEnd, string(" RAM"));

	// ...
	mAddrDigitCount = 0;

	for (int n = mMemInfo[2].baseAddr + mMemInfo[2].size - 1; n > 0; n >>= 4)
		++mAddrDigitCount;

	mLineTotalCount = 0;
	mMemSize = mMemInfo[2].baseAddr + mMemInfo[2].size;

	for each(auto &mi in mMemInfo)
		mLineTotalCount += mi.lineEnd - mi.lineStart;
}
