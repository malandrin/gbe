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
    CreateMemBlockInfo();
}

//--------------------------------------------
// --
//--------------------------------------------
InstructionsViewer::~InstructionsViewer()
{
    if (mMemBlocksInfo != nullptr)
    {
        for (int i = 0; i < mNumBlocksInfo; ++i)
            delete []mMemBlocksInfo[i].addr2Line;

        delete[] mMemBlocksInfo;
    }
}

//--------------------------------------------
// --
//--------------------------------------------
bool InstructionsViewer::OnStep()
{
    u16 pc = mGb.GetCpu().GetRegPC();

    if (mShowingBootRom && !mGb.GetMmu().IsInBootRom())
        CreateMemBlockInfo();

    if (pc >= Memory::HighRamStartAddr && pc <= Memory::HighRamEndAddr)
    {
        if (!mHighRamWalked)
        {
            CreateBlockInfo(&mMemBlocksInfo[mNumBlocksInfo - 1], Memory::HighRamStartAddr, Size::HighRam, "HRAM:", mGb.GetMmu().GetHighRam(), Memory::HighRamStartAddr);
            mHighRamWalked = true;
        }
    }

    // ...
    u8 romBank = mGb.GetMmu().GetRomBank();

    if (romBank != mPrevRomBank)
    {
        mTotalNumLines = mMemBlocksInfo[0].lines.size() + mMemBlocksInfo[romBank].lines.size() + mMemBlocksInfo[mNumBlocksInfo - 1].lines.size();
        mPrevRomBank = romBank;
    }

    mPrevActiveLineIdx = mActiveLineIdx;

    u16 addr = pc;
    int base = 0;
    MemBlockInfo *block = nullptr;

    if (addr >= Memory::RomStartAddr && addr <= Memory::RomEndAddr)
        block = &mMemBlocksInfo[0];
    else if (addr >= Memory::RomBankNStartAddr && addr <= Memory::RomBankNEndAddr)
    {
        block = &mMemBlocksInfo[romBank];
        base = (int)mMemBlocksInfo[0].lines.size();
    }
    else if (addr >= Memory::HighRamStartAddr && addr <= Memory::HighRamEndAddr)
    {
        block = &mMemBlocksInfo[mNumBlocksInfo - 1];
        base = (int)mMemBlocksInfo[0].lines.size() + (int)mMemBlocksInfo[romBank].lines.size();
    }

    if (block == nullptr)
        return false;

    int lineInfo = block->addr2Line[addr - block->addr];
    mActiveLineIdx = base + (lineInfo >> 1);

    if ((lineInfo & 1) != 0)
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

        ImGuiListClipper clipper(mTotalNumLines, lineHeight);

        // ...
        if (mGb.GetCpu().IsOnDebugMode())
        {
		    if ((mPrevActiveLineIdx != mActiveLineIdx) && ((mActiveLineIdx < clipper.DisplayStart) || (mActiveLineIdx >= clipper.DisplayEnd)))
		    {
                if (((mActiveLineIdx - mPrevActiveLineIdx) == 1) && (abs(mActiveLineIdx - clipper.DisplayEnd) <= 2))
                    ImGui::SetScrollY(ImGui::GetScrollY() + (lineHeight * 3));
                else
                    ImGui::SetScrollY(mActiveLineIdx * lineHeight);

			    mPrevActiveLineIdx = mActiveLineIdx;
		    }
        }

        // ...
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            int blockInitialLine = 0;
            MemBlockInfo *block = GetMemBlockInfoByLine(i, blockInitialLine);

            assert(block != nullptr);

            MemBlockInfo::Line *line = &block->lines[i - blockInitialLine];
			ImVec2 screenPos = ImGui::GetCursorScreenPos();
            int blockLine = line->addr - block->addr;

            // colors
			if (i == mSelectedLineIdx)
				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0, screenPos.y), ImVec2(wndWidth, screenPos.y + lineHeight), ImColor(127, 127, 127, 255));

            if ((block->addr2Line[blockLine] & 1) != 0)
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
                    ToggleBreakpoint(block, blockLine);
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
            ImGui::Text(" %s%0*X: ", block->name.c_str(), 4, line->addr);
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
InstructionsViewer::MemBlockInfo *InstructionsViewer::GetMemBlockInfoByLine(int _line, int &_blockInitialLine)
{
    _blockInitialLine = 0;

    if (_line >= 0 && _line < (int)mMemBlocksInfo[0].lines.size())
        return &mMemBlocksInfo[0];

    // ...
    int accum = (int)mMemBlocksInfo[0].lines.size();
    int romBank = mGb.GetMmu().GetRomBank();

    if (_line >= accum && _line < (int)(accum + mMemBlocksInfo[romBank].lines.size()))
    {
        _blockInitialLine = accum;
        return &mMemBlocksInfo[romBank];
    }

    // highram
    _blockInitialLine = accum + mMemBlocksInfo[romBank].lines.size();
    return &mMemBlocksInfo[mNumBlocksInfo - 1];
}

//--------------------------------------------
// --
//--------------------------------------------
void InstructionsViewer::ToggleBreakpoint(MemBlockInfo *_info, u16 _addr)
{
    u8 val = _info->addr2Line[_addr] & 1;

    if (val == 0)
        _info->addr2Line[_addr] |= 1;
    else
        _info->addr2Line[_addr] &= ~1;
}

//--------------------------------------------
// --
//--------------------------------------------
void InstructionsViewer::CreateMemBlockInfo()
{
    if (mMemBlocksInfo != nullptr)
        delete[] mMemBlocksInfo;

    int romSize = mGb.GetMmu().GetActiveRomSize();

    if (mGb.GetMmu().IsInBootRom())
    {
        mNumBlocksInfo = 2;
        mMemBlocksInfo = new MemBlockInfo[mNumBlocksInfo];

        CreateBlockInfo(&mMemBlocksInfo[0], 0, romSize, " BOOT:", mGb.GetMmu().GetBootRom(), 0);

        mShowingBootRom = true;
    }
    else
    {
        int kb16 = 1024 * 16;
        mNumBlocksInfo = (romSize / kb16) + 1; // 1 = highram
        mMemBlocksInfo = new MemBlockInfo[mNumBlocksInfo];

        // bank 0
        CreateBlockInfo(&mMemBlocksInfo[0], 0, kb16, "ROM00:", mGb.GetMmu().GetRom(), 0);

        // banks 1-n
        for (int i = 1; i < mNumBlocksInfo - 1; ++i)
            CreateBlockInfo(&mMemBlocksInfo[i], kb16, kb16, "ROM" + Int2Hex(i, 2, false) + ":", mGb.GetMmu().GetRom() + (kb16 * i), kb16 * i);

        // highram
        CreateBlockInfo(&mMemBlocksInfo[mNumBlocksInfo - 1], Memory::HighRamStartAddr, Size::HighRam, " HRAM:", mGb.GetMmu().GetHighRam(), Memory::HighRamStartAddr);

        mShowingBootRom = false;
    }

    // ...
    u8 romBank = mGb.GetMmu().GetRomBank();
    mActiveLineIdx = mMemBlocksInfo[0].addr2Line[mGb.GetCpu().GetRegPC()] >> 1;
    mTotalNumLines = mMemBlocksInfo[0].lines.size() + mMemBlocksInfo[romBank].lines.size() + mMemBlocksInfo[mNumBlocksInfo - 1].lines.size();
    mPrevRomBank = romBank;
}

//--------------------------------------------
// --
//--------------------------------------------
void InstructionsViewer::CreateBlockInfo(MemBlockInfo *_info, u16 _virtAddr, int _size, const string &_name, const u8 *_mem, u16 _realAddr)
{
    _info->addr = _virtAddr;
    _info->name = _name;

    if (_info->addr2Line != nullptr)
        delete[] _info->addr2Line;

    _info->addr2Line = new int[_size] { 0 };

    int i = 0;

    while(i < _size)
    {
        u16 addr = _virtAddr + i;
        u8 opcode = _mem[i++];

        // TODO: add romwalker 
        if (opcode == 0xCB)
        {
            u8 opcodeCb = _mem[i];
            _info->lines.push_back(MemBlockInfo::Line(addr, 2, OpcodesInfo::cb[opcodeCb].asmCode(mGb, _realAddr + i)));
            ++i;
        }
        else
        {
            u8 bl = OpcodesInfo::primary[opcode].bytesLength;
            _info->lines.push_back(MemBlockInfo::Line(addr, bl, OpcodesInfo::primary[opcode].asmCode(mGb, _realAddr + i)));
            i += bl - 1;
        }

        _info->addr2Line[addr - _virtAddr] = (_info->lines.size() - 1) << 1;
    }
}