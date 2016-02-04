#include <stdio.h>
#include "base.h"
#include "cpu.h"
#include "mmu.h"
#include "gb.h"

int main(int argn, char *argv[])
{
	GB gb;
    gb.PowerUp(argv[1], argv[2]);

    return 0;
}