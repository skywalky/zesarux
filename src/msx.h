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

#ifndef MSX_H
#define MSX_H

#include "cpu.h"

extern z80_byte *msx_vram_memory;

extern z80_byte msx_ppi_register_a;
extern z80_byte msx_ppi_register_b;
extern z80_byte msx_ppi_register_c;

extern z80_byte msx_keyboard_table[];
extern z80_byte msx_read_vram_byte(z80_int address);

extern int msx_memory_slots[4][4];

extern z80_byte *msx_return_segment_address(z80_int direccion,int *tipo);

#define MSX_SLOT_MEMORY_TYPE_ROM 0
#define MSX_SLOT_MEMORY_TYPE_RAM 1
#define MSX_SLOT_MEMORY_TYPE_EMPTY 2

#endif
