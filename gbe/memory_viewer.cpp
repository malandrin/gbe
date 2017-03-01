#include <imgui.h>
#include <imgui_impl_sdl.h>
#include "base.h"
#include "defines.h"
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
    if (mShowingBootRom && !mMmu.IsInBootRom())
        CalculateMemInfo();

    u8 romBank = mMmu.GetRomBank();

    if (romBank != mPrevRomBank)
    {
        mMemInfo[mRomBank1Idx] = mRomBanksMemInfo[romBank];
        mPrevRomBank = romBank;

        for (int i = mRomBank1Idx + 1; i < (int)mMemInfo.size(); ++i)
            mMemInfo[i].SetLineStart((i == 0) ? 0 : mMemInfo[i - 1].lineEnd);
    }

    // ...
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
			ImGui::Text((memInfo->memType + ":%0*X: ").c_str(), 4, addr);
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

	// Boot rom
    mShowingBootRom = mMmu.IsInBootRom();

	if (mShowingBootRom)
		mMemInfo.push_back(MemInfo(mMmu.GetBootRom(), Size::BootRom, 0, 0, string(" BROM")));

	// cartridge rom (bank 1-n share memory address)
    int kb16 = 1024 * 16;
    int numBlocks = (mMmu.GetRomSize() / kb16);

    // bank 0
    const u8 *rom = mMmu.GetRom();
    MemInfo bank0(rom, kb16, 0, mShowingBootRom ? mMemInfo[0].lineEnd : 0, string("ROM00"));

    mRomBanksMemInfo.push_back(bank0);
    mMemInfo.push_back(bank0);

    if (numBlocks > 1)
    {
        mRomBank1Idx = (int)mMemInfo.size();
        MemInfo bank1(rom + kb16, kb16, kb16, mMemInfo[mMemInfo.size() - 1].lineEnd, string("ROM01"));

        mRomBanksMemInfo.push_back(bank1);
        mMemInfo.push_back(bank1); // bank0 always is visible, in this position will be banks 1-n

        // bank 2-n
        for (int i = 2; i < numBlocks; ++i)
            mRomBanksMemInfo.push_back(MemInfo(rom + (kb16 * i), kb16, kb16, bank1.lineStart, string("ROM") + Int2Hex(i, 2, false)));
    }

	// vram
	mMemInfo.push_back(MemInfo(mMmu.GetVRam(), Size::VRam, Memory::VRamStartAddr, mMemInfo[mMemInfo.size() - 1].lineEnd, string(" VRAM")));

	// ram
	mMemInfo.push_back(MemInfo(mMmu.GetRam(), Size::Ram, Memory::RamStartAddr, mMemInfo[mMemInfo.size() - 1].lineEnd, string("  RAM")));

    // OAM
    mMemInfo.push_back(MemInfo(mMmu.GetOAM(), Size::OAM, Memory::OAMStartAddr, mMemInfo[mMemInfo.size() - 1].lineEnd, string( "  OAM")));

	// IO registers
	mMemInfo.push_back(MemInfo(mMmu.GetIORegisters(), Size::IORegisters, Memory::IORegsStartAddr, mMemInfo[mMemInfo.size() - 1].lineEnd, string(" IORG")));

	// high ram
	mMemInfo.push_back(MemInfo(mMmu.GetHighRam(), Size::HighRam, Memory::HighRamStartAddr, mMemInfo[mMemInfo.size() - 1].lineEnd, string(" HRAM")));

	// ...
	int numMemInfo = mMemInfo.size() - 1;
	mLineTotalCount = 0;
	mMemSize = mMemInfo[numMemInfo].baseAddr + mMemInfo[numMemInfo].size;

	for each(auto &mi in mMemInfo)
		mLineTotalCount += mi.lineEnd - mi.lineStart;
}
