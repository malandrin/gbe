#pragma once

class MMU;

class VRAMViewer
{
public:

            VRAMViewer  (const MMU &_mmu);

    void    Init        ();
    void    Render      ();

private:

    const MMU &mMmu;
    int        mActiveVRam {0};
    u32        mTextureArray[256 * 256];
    GLuint     mTextureId {0};

    void    BuildTileData   (u16 _addr);
    void    RenderTile      (u16 _tileDataAddr, u8 _numTile, u8 _x, u8 _y);
};
