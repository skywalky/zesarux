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



#include <stdio.h>


#include "vdp_9918a.h"
#include "cpu.h"
#include "debug.h"

z80_byte vdp_9918a_registers[8];


//Ultimos dos bytes enviados al puerto de comando/status
z80_byte vdp_9918a_last_command_status_bytes[2];
//Contador a ultima posicion
int vdp_9918a_last_command_status_bytes_counter=0;

//Ultimos 3 bytes enviados al puerto de datos. Realmente el 0 y 1 es el address pointer actual, y el 3 el ultimo byte enviado
z80_byte vdp_9918a_last_vram_bytes[3];
//Contador a ultima posicion
//int vdp_9918a_last_vram_bytes_counter=0;

z80_int vdp_9918a_last_vram_position;


void vdp_9918a_out_vram_data(z80_byte *vram_memory,z80_byte value)
{

printf ("%c",(value>=32 && value<=126 ? value : '?') );
    int posicion_escribir=vdp_9918a_last_vram_position & 16383;

    vram_memory[posicion_escribir]=value;

    vdp_9918a_last_vram_position++;


}


void vdp_9918a_out_command_status(z80_byte *vram_memory,z80_byte value)
{
    printf ("vdp_9918a write status: %02XH position: %d\n",value,vdp_9918a_last_command_status_bytes_counter);

    switch (vdp_9918a_last_command_status_bytes_counter) {
        case 0:
            vdp_9918a_last_command_status_bytes[0]=value;
            vdp_9918a_last_command_status_bytes_counter=1;
        break;

        case 1:
            vdp_9918a_last_command_status_bytes[1]=value;
            vdp_9918a_last_command_status_bytes_counter=0;

            //Recibido los dos bytes. Ver que comando es

            if ( (vdp_9918a_last_command_status_bytes[1] &  (128+64)) == 64 ) {
                //printf ("Write VDP Address setup.\n");

                //vdp_9918a_last_vram_position=(vdp_9918a_last_command_status_bytes[1] & 63) | (vdp_9918a_last_command_status_bytes[0]<<6);


                vdp_9918a_last_vram_position=(vdp_9918a_last_command_status_bytes[0]) | ((vdp_9918a_last_command_status_bytes[1] & 63)<<8);
                printf ("Write VDP Address setup. address: %04XH\n",vdp_9918a_last_vram_position);
            }
        break;
    }


}