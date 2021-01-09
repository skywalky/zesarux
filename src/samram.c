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

#if defined(__APPLE__)
        #include <sys/syslimits.h>
#endif


#include "samram.h"
#include "cpu.h"
#include "debug.h"
#include "utils.h"
#include "operaciones.h"
#include "ula.h"


z80_bit samram_enabled={0};

z80_bit samram_protected={0};

char samram_rom_file_name[PATH_MAX]="";

z80_byte *samram_memory_pointer;



int samram_nested_id_poke_byte;
int samram_nested_id_poke_byte_no_time;
int samram_nested_id_peek_byte;
int samram_nested_id_peek_byte_no_time;

//Banco activo. 0..31
z80_byte samram_active_bank=0;


int samram_check_if_rom_area(z80_int dir)
{
                if (dir<16384) {
			return 1;
                }
	return 0;
}

z80_byte samram_read_byte(z80_int dir)
{
	//Si no, memoria samram
	int puntero=samram_active_bank*16384+dir;
	return samram_memory_pointer[puntero];
}


void samram_handle_special_dirs(z80_int dir)
{

	if (samram_protected.v) return;

	if (dir>=0x3FFC && dir<=0x3FFF) {
		//Valor bit A0
		z80_byte value_a0=dir&1;

		//Mover banco * 2
		samram_active_bank = samram_active_bank << 1;

		samram_active_bank |=value_a0;

		//Mascara final
		samram_active_bank=samram_active_bank&31;



		//Si se habilita proteccion
		if (dir&2) samram_protected.v=1;
	}

}


z80_byte samram_poke_byte(z80_int dir,z80_byte valor)
{

	//samram_original_poke_byte(dir,valor);
        //Llamar a anterior
        debug_nested_poke_byte_call_previous(samram_nested_id_poke_byte,dir,valor);

	samram_handle_special_dirs(dir);

        //Para que no se queje el compilador, aunque este valor de retorno no lo usamos
        return 0;


}

z80_byte samram_poke_byte_no_time(z80_int dir,z80_byte valor)
{
        //samram_original_poke_byte_no_time(dir,valor);
        //Llamar a anterior
        debug_nested_poke_byte_no_time_call_previous(samram_nested_id_poke_byte_no_time,dir,valor);


	samram_handle_special_dirs(dir);

        //Para que no se queje el compilador, aunque este valor de retorno no lo usamos
        return 0;


}

z80_byte samram_peek_byte(z80_int dir,z80_byte value GCC_UNUSED)
{

	z80_byte valor_leido=debug_nested_peek_byte_call_previous(samram_nested_id_peek_byte,dir);

	samram_handle_special_dirs(dir);

	if (samram_check_if_rom_area(dir)) {
		return samram_read_byte(dir);
	}

	//return samram_original_peek_byte(dir);
	return valor_leido;
}

z80_byte samram_peek_byte_no_time(z80_int dir,z80_byte value GCC_UNUSED)
{

	z80_byte valor_leido=debug_nested_peek_byte_no_time_call_previous(samram_nested_id_peek_byte_no_time,dir);

	samram_handle_special_dirs(dir);

	if (samram_check_if_rom_area(dir)) {
                return samram_read_byte(dir);
        }

	return valor_leido;
}



//Establecer rutinas propias
void samram_set_peek_poke_functions(void)
{
                debug_printf (VERBOSE_DEBUG,"Setting samram poke / peek functions");

	//Asignar mediante nuevas funciones de core anidados
	samram_nested_id_poke_byte=debug_nested_poke_byte_add(samram_poke_byte,"Samram poke_byte");
	samram_nested_id_poke_byte_no_time=debug_nested_poke_byte_no_time_add(samram_poke_byte_no_time,"Samram poke_byte_no_time");
	samram_nested_id_peek_byte=debug_nested_peek_byte_add(samram_peek_byte,"Samram peek_byte");
	samram_nested_id_peek_byte_no_time=debug_nested_peek_byte_no_time_add(samram_peek_byte_no_time,"Samram peek_byte_no_time");

}

//Restaurar rutinas de samram
void samram_restore_peek_poke_functions(void)
{
                debug_printf (VERBOSE_DEBUG,"Restoring original poke / peek functions before samram");


	debug_nested_poke_byte_del(samram_nested_id_poke_byte);
	debug_nested_poke_byte_no_time_del(samram_nested_id_poke_byte_no_time);
	debug_nested_peek_byte_del(samram_nested_id_peek_byte);
	debug_nested_peek_byte_no_time_del(samram_nested_id_peek_byte_no_time);
}



void samram_alloc_memory(void)
{
        int size=SAMRAM_SIZE;  

        debug_printf (VERBOSE_DEBUG,"Allocating %d kb of memory for samram emulation",size/1024);

        samram_memory_pointer=malloc(size);
        if (samram_memory_pointer==NULL) {
                cpu_panic ("No enough memory for samram emulation");
        }


}

int samram_load_rom(void)
{

        FILE *ptr_samram_romfile;
        int leidos=0;

        debug_printf (VERBOSE_INFO,"Loading samram rom %s",samram_rom_file_name);

  			ptr_samram_romfile=fopen(samram_rom_file_name,"rb");
                if (!ptr_samram_romfile) {
                        debug_printf (VERBOSE_ERR,"Unable to open ROM file");
                }

        if (ptr_samram_romfile!=NULL) {

                leidos=fread(samram_memory_pointer,1,SAMRAM_SIZE,ptr_samram_romfile);
                fclose(ptr_samram_romfile);

        }



        if (leidos!=SAMRAM_SIZE || ptr_samram_romfile==NULL) {
                debug_printf (VERBOSE_ERR,"Error reading samram rom");
                return 1;
        }

        return 0;
}



void samram_enable(void)
{

  if (!MACHINE_IS_SPECTRUM && !MACHINE_IS_CPC) {
    debug_printf(VERBOSE_INFO,"Can not enable samram on non Spectrum or CPC machine");
    return;
  }

	if (samram_enabled.v) {
		debug_printf (VERBOSE_DEBUG,"Already enabled");
		return;
	}

	if (samram_rom_file_name[0]==0) {
		debug_printf (VERBOSE_ERR,"Trying to enable Samram but no ROM file selected");
		return;
	}

	samram_alloc_memory();
	if (samram_load_rom()) return;

	samram_set_peek_poke_functions();

	samram_enabled.v=1;

	samram_press_button();

	//samram_active_bank=0;
	//samram_protected.v=0;
	



}

void samram_disable(void)
{
	if (samram_enabled.v==0) return;

	samram_restore_peek_poke_functions();

	free(samram_memory_pointer);

	samram_enabled.v=0;
}


void samram_press_button(void)
{

        if (samram_enabled.v==0) {
                debug_printf (VERBOSE_ERR,"Trying to press Samram button when it is disabled");
                return;
        }

	samram_active_bank=0;
	samram_protected.v=0;

	reset_cpu();


}
