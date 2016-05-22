#pragma once
#ifndef _MMU_LISTENER_H
#define _MMU_LISTENER_H

class IMmuListener
{
public:

    virtual void OnMemoryWrittenU8  (u16 _virtAdd, u8 _value) = 0;
    virtual void OnMemoryWrittenU16 (u16 _virtAdd, u16 _value) = 0;
};

#endif
