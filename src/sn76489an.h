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

#ifndef SN76489AN_H
#define SN76489AN_H

#include "cpu.h"

#define MAX_SN_CHIPS 1

extern void out_port_sn(z80_int puerto,z80_byte value);



extern z80_byte sn_retorna_mixer_register(int chip);

//1'7734*1000000 (Hz) en Spectrum
//En Amstrad, 1 MHz
//En zonx zx81 , 1625000
//En zxpand 1625 tambien?
//#define FRECUENCIA_SN   1773400


#define FRECUENCIA_SN (sn_chip_frequency)

#define FRECUENCIA_SPECTRUM_SN 1773400
#define FRECUENCIA_CPC_SN      1000000
#define FRECUENCIA_ZX81_SN     1625000
#define FRECUENCIA_MSX_SN      1789772





#define SN_FRECUENCIA_NOISE (FRECUENCIA_SN/2)

#define FRECUENCIA_ENVELOPE 6927*10


extern void sn_chip_siguiente_ciclo(void);
extern void init_chip_sn(void);



extern char da_output_sn_izquierdo(void);
extern char da_output_sn_derecho(void);





extern void sn_init_filters(void);

#endif
