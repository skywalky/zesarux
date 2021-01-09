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


//Byte de config
//Bit 0: write protect, bit 1: cmos ram enabled, etc
z80_byte samram_settings_byte;



/*

5.6  The SamRam


    The SamRam contains a 32K static CMOS Ram chip, and some I/O logic for
    port 31.  If this port is read, it returns the position of the
    joystick, as a normal Kempston joystickinterface would.  If written to,
    the port controls a programmable latch chip (the 74LS259) which
    contains 8 latches:
 

       Bit    7   6   5   4   3   2   1   0
            ÚÄÄÄÂÄÄÄÂÄÄÄÂÄÄÄÂÄÄÄÂÄÄÄÂÄÄÄÂÄÄÄ¿
       WRITE³   ³   ³   ³   ³  address  ³bit³
            ÀÄÄÄÁÄÄÄÁÄÄÄÁÄÄÄÁÄÄÄÁÄÄÄÁÄÄÄÁÄÄÄÙ


    The address selects on of the eight latches; bit 0 is the new state of
    the latch.  The 16 different possibilities are collected in the diagram
    below:

        OUT 31,   ³  Latch  ³ Result
        ÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
            0     ³    0    ³ Switch on write protect of CMOS RAM
            1     ³    "    ³ Writes to CMOS RAM allowed
            2     ³    1    ³ turn on CMOS RAM (see also 6/7)
            3     ³    "    ³ turn off CMOS RAM (standard Spec.  ROM)
            4     ³    2    ³ -
            5     ³    "    ³ Ignore all OUT's to 31 hereafter
            6     ³    3    ³ Select CMOS bank 0 (Basic ROM)
            7     ³    "    ³ Select CMOS bank 1 (Monitor,...)
            8     ³    4    ³ Select interface 1
            9     ³    "    ³ Turn off IF 1 (IF1 rom won't be paged)
           10     ³    5    ³ Select 32K ram bank 0 (32768-65535)
           11     ³    "    ³ Select 32K ram bank 1 (32768-65535)
           12     ³    6    ³ Turn off beeper
           13     ³    "    ³ Turn on beeper
           14     ³    7    ³ -
           15     ³    "    ³ -

    At reset, all latches are 0.  If an OUT 31,5 is issued, only a reset
    will give you control over the latches again.  The write protect latch
    is not emulated; you're never able to write the emulated CMOS ram in
    the emulator.  Latch 4 pulls up the M1 output at the expansion port of
    the Spectrum.  The Interface I won't page its ROM anymore then.


*/

//retorna direccion a memoria samram que hace referencia dir
//NULL si no es memoria samram 

z80_byte *samram_check_if_sam_area(z80_int dir)
{

  //si espacio rom
  if (dir<16384) {
    if (samram_settings_byte & 2) return NULL;
    //rom mapeada, ver cual
    int romblock=samram_settings_byte & 8;
    romblock=romblock >> 3;
    
    int offset=dir+16384*romblock;
    return &samram_memory_pointer[offset];
    
  }
  
  //si mayor 32767
  if (dir>32767) {
  }
  
  return NULL;
}

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





z80_byte samram_poke_byte(z80_int dir,z80_byte valor)
{

	//samram_original_poke_byte(dir,valor);
        //Llamar a anterior
        debug_nested_poke_byte_call_previous(samram_nested_id_poke_byte,dir,valor);
        
        z80_byte *samdir;
        samdir=samram_check_if_sam_area(dir);
        if (samdir!=NULL) {
           *samdir=valor;
        }

	

        //Para que no se queje el compilador, aunque este valor de retorno no lo usamos
        return 0;


}

z80_byte samram_poke_byte_no_time(z80_int dir,z80_byte valor)
{
        //samram_original_poke_byte_no_time(dir,valor);
        //Llamar a anterior
        debug_nested_poke_byte_no_time_call_previous(samram_nested_id_poke_byte_no_time,dir,valor);


	z80_byte *samdir;
        samdir=samram_check_if_sam_area(dir);
        if (samdir!=NULL) {
           *samdir=valor;
        }

        //Para que no se queje el compilador, aunque este valor de retorno no lo usamos
        return 0;


}

z80_byte samram_peek_byte(z80_int dir,z80_byte value GCC_UNUSED)
{

	z80_byte valor_leido=debug_nested_peek_byte_call_previous(samram_nested_id_peek_byte,dir);

	

	z80_byte *samdir;
        samdir=samram_check_if_sam_area(dir);
        if (samdir!=NULL) {
           return *samdir;
        }

	//return samram_original_peek_byte(dir);
	return valor_leido;
}

z80_byte samram_peek_byte_no_time(z80_int dir,z80_byte value GCC_UNUSED)
{

	z80_byte valor_leido=debug_nested_peek_byte_no_time_call_previous(samram_nested_id_peek_byte_no_time,dir);

	
	z80_byte *samdir;
        samdir=samram_check_if_sam_area(dir);
        if (samdir!=NULL) {
           return *samdir;
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

                leidos=fread(samram_memory_pointer,1,32768,ptr_samram_romfile);
                fclose(ptr_samram_romfile);

        }



        if (leidos!=32768 || ptr_samram_romfile==NULL) {
                debug_printf (VERBOSE_ERR,"Error reading samram rom");
                return 1;
        }

        return 0;
}



void samram_enable(void)
{

  if (!MACHINE_IS_SPECTRUM_48) {
    debug_printf(VERBOSE_INFO,"Can not enable samram on non Spectrum 48 machine");
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

	
	samram_settings_byte=2; //no mapeado



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

	samram_settings_byte=0; //activar cmos ram y seleccionar bank 0

	reset_cpu();


}

void samram_write_port(z80_byte value)
{

  int bitvalue=value&1;
  
  int bitnumber=(value>>1)&7;
  
  //samram_settings_byte
  z80_byte maskzero=1;
  
  maskzero=maskzero<<bitnumber;
  
  maskzero ^=255;
  
  //poner a cero
  samram_settings_byte &=maskzero;
  
  //y OR con valor
  bitvalue=bitvalue<<bitnumber;
  
  samram_settings_byte |=bitvalue;


}
