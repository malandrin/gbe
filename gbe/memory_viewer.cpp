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
	CalculateMemInfo();
}

//--------------------------------------------
// --
//--------------------------------------------
void MemoryViewer::Render()
{
	ImGui::SetNextWindowPos(ImVec2(0, 468));

	if (ImGui::Begin("Memory", nullptr, ImVec2(1024, 300), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
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
			int memIdx = GetMemInfoByLine(i);
			MemInfo *memInfo = &mMemInfo[memIdx];

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

			ImVec2 screenPos = ImGui::GetCursorScreenPos();
			ImGui::GetWindowDrawList()->AddLine(ImVec2(screenPos.x - glyphWidth, screenPos.y), ImVec2(screenPos.x - glyphWidth, screenPos.y + lineHeight), ImColor(ImGui::GetStyle().Colors[ImGuiCol_Border]));

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
void MemoryViewer::CalculateMemInfo()
{
	mMemInfo.clear();

	// Bootable rom
	if (mMmu.IsInBootableRom())
		mMemInfo.push_back(MemInfo(mMmu.GetBootableRom(), MMU::BootableRomSize, 0, 0, string("BROM")));

	// cartridge rom
	mMemInfo.push_back(MemInfo(mMmu.GetRom(), mMmu.GetRomSize(), 0,
							   mMmu.IsInBootableRom() ? mMemInfo[0].lineEnd : 0, string(" ROM")));

	// vram
	mMemInfo.push_back(MemInfo(mMmu.GetVRam(), MMU::VRamSize, 0x8000, mMemInfo[mMemInfo.size() - 1].lineEnd, string("VRAM")));

	// ram
	mMemInfo.push_back(MemInfo(mMmu.GetRam(), MMU::RamSize, 0xC000, mMemInfo[mMemInfo.size() - 1].lineEnd, string(" RAM")));

	// IO registers
	mMemInfo.push_back(MemInfo(mMmu.GetIORegisters(), MMU::IORegistersSize, 0xFF00, mMemInfo[mMemInfo.size() - 1].lineEnd, string("IORG")));

	// high ram
	mMemInfo.push_back(MemInfo(mMmu.GetHighRam(), MMU::HighRamSize, 0xFF80, mMemInfo[mMemInfo.size() - 1].lineEnd, string("HRAM")));

	// ...
	mAddrDigitCount = 0;
	int numMemInfo = mMemInfo.size() - 1;

	for (int n = mMemInfo[numMemInfo].baseAddr + mMemInfo[numMemInfo].size - 1; n > 0; n >>= 4)
		++mAddrDigitCount;

	mLineTotalCount = 0;
	mMemSize = mMemInfo[numMemInfo].baseAddr + mMemInfo[numMemInfo].size;

	for each(auto &mi in mMemInfo)
		mLineTotalCount += mi.lineEnd - mi.lineStart;
}
