#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <SDL_opengl.h>
#include "base.h"
#include "mmu.h"
#include "vram_viewer.h"

static string sRadioButtonsText[4] { "Tile Data 1", "Tile Data 2", "Tile Map 1", "Tile Map 2" };
static u32 sPalette[4] = { 0xFF000000, 0xFF404040, 0xFF7F7F7F, 0xFFFFFFFF};

//--------------------------------------------
// --
//--------------------------------------------
VRAMViewer::VRAMViewer(const MMU &_mmu) : mMmu(_mmu)
{
}

//--------------------------------------------
// --
//--------------------------------------------
void VRAMViewer::Init()
{
    glGenTextures(1, &mTextureId);

    glBindTexture(GL_TEXTURE_2D, mTextureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

//--------------------------------------------
// --
//--------------------------------------------
void VRAMViewer::Render()
{
    ImGui::SetNextWindowPos(ImVec2(560, 0));

    if (ImGui::Begin("VRAM", nullptr, ImVec2(464, 468), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
    {
        for (int i = 0; i < 4; ++i)
        {
            ImGui::SameLine();
            ImGui::RadioButton(sRadioButtonsText[i].c_str(), &mActiveVRam, i);
        }
        
        switch(mActiveVRam)
        {
            case 0:
                BuildTileData(0x8000);
                break;
        }

        ImGui::Image((ImTextureID)mTextureId, ImVec2(512, 512));
    }
    ImGui::End();
}

//--------------------------------------------
// --
//--------------------------------------------
void VRAMViewer::BuildTileData(u16 _addr)
{
    fill_n(mTextureArray, 256 * 256, 0x00000000);

    int r = 0;
    int c = 0;

    for (int t = 0; t < 256; ++t)
    {
        RenderTile(_addr, t, c * 8, r * 8);

        ++c;
        if (c == 16)
        {
            c = 0;
            ++r;
        }
    }

    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, mTextureArray);
}

//--------------------------------------------
// --
//--------------------------------------------
void VRAMViewer::RenderTile(u16 _tileDataAddr, u8 _numTile, u8 _x, u8 _y)
{
    u16 _tileAddr = _tileDataAddr + (_numTile * 16);

    for (int r = 0; r < 8; ++r)
    {
        u8 b1 = mMmu.ReadU8(_tileAddr++);
        u8 b2 = mMmu.ReadU8(_tileAddr++);

        for (int b = 7; b >= 0; --b)
        {
            u8 cp = (((b1 >> b) & 1) << 1) | (b2 >> b);
            u32 c = sPalette[cp];

            int p = ((_y + r) * 256) + _x + (7 - b);
            mTextureArray[((_y + r) * 256) + _x + (7 - b)] = c;
        }
    }
}