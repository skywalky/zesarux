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
#include <stdlib.h>
#include <dirent.h>


#include "gs.h"
#include "cpu.h"
#include "debug.h"
#include "utils.h"
#include "operaciones.h"

z80_bit gs_enabled={0};



z80_byte *gs_memory_pointer;




void gs_alloc_memory(void)
{
        int size=(GS_ROM_SIZE+GS_RAM_SIZE)*1024;  

        debug_printf (VERBOSE_DEBUG,"Allocating %d kb of memory for general sound emulation",size/1024);

        gs_memory_pointer=malloc(size);
        if (gs_memory_pointer==NULL) {
                cpu_panic ("No enough memory for general sound emulation");
        }


}

int gs_load_rom(void)
{

        FILE *ptr_gs_romfile;
        int leidos=0;

        debug_printf (VERBOSE_INFO,"Loading gs rom %s",GS_ROM_NAME);

  			ptr_gs_romfile=fopen(GS_ROM_NAME,"rb");
                if (!ptr_gs_romfile) {
                        debug_printf (VERBOSE_ERR,"Unable to open ROM file");
                }

        if (ptr_gs_romfile!=NULL) {

                leidos=fread(gs_memory_pointer,1,GS_ROM_SIZE*1024,ptr_gs_romfile);
                fclose(ptr_gs_romfile);

        }



        if (leidos!=GS_ROM_SIZE*1024 || ptr_gs_romfile==NULL) {
                debug_printf (VERBOSE_ERR,"Error reading general sound rom");
                return 1;
        }

        return 0;
}



void gs_enable(void)
{

  if (!MACHINE_IS_PENTAGON) {
    debug_printf(VERBOSE_INFO,"Can not enable general sound on non Pentagon machine");
    return;
  }

	if (gs_enabled.v) return;



	gs_alloc_memory();
	if (gs_load_rom()) return;


	gs_enabled.v=1;


}

void gs_disable(void)
{
	if (gs_enabled.v==0) return;


	free(gs_memory_pointer);


	gs_enabled.v=0;
}

//Ejecutar los opcodes de todo un scanline
void gs_fetch_opcodes_scanlines(void)
{
    //Guardar estado maquina spectrum (registros cpu, testados, interrupciones)

    //Restaurar estado a la cpu de general sound

    //Ejecutar todos los ciclos...

    //Guardar estado maquina general sound

    //Restaurar estado maquina spectrum
}