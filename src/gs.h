/*
    ZEsarUX  ZX Second-Emulator And Released for UniX 
    Copyright (C) 2013 Cesar Hernandez Bano

    This file is part of ZEsarUX.

    ZEsarUX is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef GS_H
#define GS_H

#include "cpu.h"


//Tamanyos en KB de la rom y ram
#define GS_ROM_SIZE 32
#define GS_ROM_BLOCKS (GS_ROM_SIZE/16)
#define GS_RAM_SIZE 512
#define GS_RAM_BLOCKS (GS_RAM_SIZE/16)

#define GS_ROM_NAME "gs105a.rom"


extern z80_bit gs_enabled;
extern void gs_enable(void);
extern void gs_disable(void);
extern void gs_fetch_opcodes_scanlines(void);

#endif
