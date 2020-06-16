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

#ifndef VDP_9918A_H
#define VDP_9918A_H

#include "cpu.h"

extern z80_byte vdp_9918a_registers[];

extern char *get_vdp_9918_string_video_mode(void);

extern z80_int vdp_9918a_get_sprite_attribute_table(void);

extern z80_byte vdp_9918a_get_video_mode(void);

extern void vdp_9918a_out_vram_data(z80_byte *vram_memory,z80_byte value);

extern z80_int vdp_9918a_get_pattern_name_table(void);

extern z80_byte vdp_9918a_in_vram_data(z80_byte *vram_memory);

extern z80_byte vdp_9918a_get_border_color(void);

extern z80_byte vdp_9918a_in_vdp_status(void);

extern void vdp_9918a_out_command_status(z80_byte *vram_memory,z80_byte value);

extern int vdp_9918a_get_tile_heigth(void);

extern int vdp_9918a_get_tile_width(void);

#define VDP_9918A_MAX_SPRITES 32

#endif
