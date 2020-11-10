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

struct gs_machine_state {

    z80_int r_pc,r_sp;

    z80_int r_bc,r_de,r_hl,r_af;
    z80_int r_bc_shadow,r_de_shadow,r_hl_shadow,r_af_shadow;


    z80_int r_ix,r_iy;

//header[20]=(reg_r&127) | (reg_r_bit7&128);

    z80_int r_ir;

    z80_bit iff1,iff2;
    z80_byte im_mode;

    int t_estados;

    z80_bit z80_ejecutando_halt;

    void (*poke_byte)(z80_int dir,z80_byte valor);
    void (*poke_byte_no_time)(z80_int dir,z80_byte valor);
    z80_byte (*peek_byte)(z80_int dir);
    z80_byte (*peek_byte_no_time)(z80_int dir);    
    z80_byte (*lee_puerto)(z80_byte puerto_h,z80_byte puerto_l);
    void (*out_port)(z80_int puerto,z80_byte value);
    z80_byte (*fetch_opcode)(void);

};

#endif
