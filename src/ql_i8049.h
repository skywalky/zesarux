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

#ifndef QL_I8049_H
#define QL_I8049_H

#include "ql.h"

extern int ql_ipc_reading_bit_ready;

extern unsigned char ql_read_ipc(void);
extern void ql_write_ipc(unsigned char Data); 
extern char ql_audio_da_output(void);
extern void ql_audio_next_cycle(void);

extern void ql_ipc_reset(void);

extern void qltraps_init_fopen_files_array(void);

extern moto_int ql_current_sound_duration;

extern int ql_mantenido_pulsada_tecla;
extern int ql_mantenido_pulsada_tecla_timer;

extern int ql_pressed_backspace;

extern int ql_pulsado_tecla(void);

extern z80_byte ql_keyboard_table[];




#endif
