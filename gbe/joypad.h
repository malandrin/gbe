#pragma once

#include "mmu.h"
#include "mmu_listener.h"

class Joypad : public IMmuListener
{
public:

    enum Key
    {
        // P14
        Right = 0,
        Left,
        Up,
        Down,

        // P15
        A,
        B,
        Select,
        Start,

        Num
    };

            Joypad    (MMU &_mmu);

    void    OnKeyDown (Key _key) { mKeyPressed[_key] = true; }
    void    OnKeyUp   (Key _key) { mKeyPressed[_key] = false; }

    // inherited IMmuListener
    void OnMemoryWrittenU8  (u16 _virtAdd, u8 _value);
    void OnMemoryWrittenU16 (u16 _virtAdd, u16 _value) { }

private:

    bool mKeyPressed[Key::Num] {false};
    bool mIgnoreWritten {false};
    MMU& mMmu;
};
