#pragma once
#ifndef _GB_H
#define _GB_H

class GB
{
public:
    void    PowerUp    (const string& _bootableRom, const string& _cartridge);


private:
	MMU mMmu;
    CPU mCpu;
};

#endif
