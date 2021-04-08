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

#ifndef VDP_9918A_SMS_H
#define VDP_9918A_SMS_H

#include "cpu.h"

#define VDP_9918A_SMS_MODE4_MAX_SPRITES 64

extern int sms_writing_cram;
extern int index_sms_escritura_cram;
extern z80_byte vdp_9918a_sms_cram[];
extern void vdp_9918a_sms_reset(void);
extern int vdp_9918a_si_sms_video_mode4(void);
extern void vdp_9918a_render_sprites_sms_video_mode4_no_rainbow(z80_byte *vram);
extern void vdp_9918a_render_ula_no_rainbow_sms(z80_byte *vram);
extern z80_int vdp_9918a_sms_get_pattern_name_table(void);

#endif
