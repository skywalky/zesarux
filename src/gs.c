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


z80_byte *gs_rom_memory_tables[GS_ROM_BLOCKS];
z80_byte *gs_ram_memory_tables[GS_RAM_BLOCKS];

//Ultimo valor enviado a puerto 00 desde el GS
z80_byte gs_memory_mapping_value;

//Memoria mapeada actual
z80_byte *gs_memory_mapped[4];

//Y tipos mapeados: 0=read only, 1=writable
int gs_memory_mapped_types[4];

void gs_init_memory_tables(void)
{
    gs_rom_memory_tables[0]=gs_memory_pointer;
    gs_rom_memory_tables[1]=&gs_memory_pointer[16384];

    int i;

    for (i=0;i<GS_RAM_BLOCKS;i++) {
        gs_ram_memory_tables[i]=&gs_memory_pointer[32768+16384*i];
    }
}

void gs_set_memory_mapping(void)
{
/*

port 0 "extended memory"
bits D0 - D3 switch pages by 32Kb
page 0 - ROM
digits D4 - D7 are not used

                         Memory allocation

# 0000 - # 3FFF - first 16Kb of ROM
# 4000 - # 7FFF - first 16Kb of the first page of RAM
# 8000 - #FFFF - turnable pages of 32Kb
                  page 0 - ROM,
                  page 1 - first page of RAM
                  page 2 ... RAM
*/

    gs_memory_mapped[0]=gs_rom_memory_tables[0];
    gs_memory_mapped_types[0]=0;

    gs_memory_mapped[1]=gs_ram_memory_tables[0];
    gs_memory_mapped_types[0]=1;

    z80_byte mapped_value=gs_memory_mapping_value & 31;

    if (!mapped_value) {
        //ROM en segmentos altos
        gs_memory_mapped[2]=gs_rom_memory_tables[0];
        gs_memory_mapped_types[2]=0;
        gs_memory_mapped[3]=gs_rom_memory_tables[1];
        gs_memory_mapped_types[3]=0;
    }
    else {
        //RAM en segmentos altos
        //1= ram 0,1
        //2= ram 2,3
        //3= ram 4,5
        mapped_value--;

        mapped_value *=2;
        gs_memory_mapped[2]=gs_ram_memory_tables[mapped_value];
        gs_memory_mapped_types[2]=1;
        gs_memory_mapped[3]=gs_ram_memory_tables[mapped_value+1];
        gs_memory_mapped_types[3]=1;        
    }
}

void gs_reset(void)
{
    gs_memory_mapping_value=0;
    gs_set_memory_mapping();
}

void gs_alloc_memory(void)
{
    int size=(GS_ROM_SIZE+GS_RAM_SIZE)*1024;  

    debug_printf (VERBOSE_DEBUG,"Allocating %d kb of memory for general sound emulation",size/1024);

    gs_memory_pointer=malloc(size);
    if (gs_memory_pointer==NULL) {
            cpu_panic ("No enough memory for general sound emulation");
    }

    gs_init_memory_tables();
    gs_reset();
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

/*

        
				fetch_opcode=fetch_opcode_coleco;
*/                


//Guarda estado maquina actual (Z80, otras variables) en estructura
void gs_save_machine_state(struct gs_machine_state *m)
{
    m->r_pc=reg_pc;
    m->r_sp=reg_sp;

    m->r_bc=BC;
    m->r_de=DE;
    m->r_hl=HL;
    m->r_af=REG_AF;

    m->r_bc_shadow=REG_BC_SHADOW;
    m->r_de_shadow=REG_DE_SHADOW;
    m->r_hl_shadow=REG_HL_SHADOW;
    m->r_af_shadow=REG_AF_SHADOW;    


    m->r_ix=reg_ix;
    m->r_iy=reg_iy;

//header[20]=(reg_r&127) | (reg_r_bit7&128);

    m->r_ir=IR;

    m->iff1.v=iff1.v;
    m->iff2.v=iff2.v;
    m->im_mode=im_mode;

    m->t_estados=t_estados;

    m->z80_ejecutando_halt.v=z80_ejecutando_halt.v;


    m->poke_byte=poke_byte;
    m->poke_byte_no_time=poke_byte_no_time;
    m->peek_byte=peek_byte;
    m->peek_byte_no_time=peek_byte_no_time;
    m->lee_puerto=lee_puerto;
    m->out_port=out_port;
    m->fetch_opcode=fetch_opcode;
}


//Estado de la Z80 principal
struct gs_machine_state normal_z80_cpu;

//Estado de la Z80 del general sound
struct gs_machine_state general_sound_z80_cpu;


//Ejecutar los opcodes de todo un scanline
void gs_fetch_opcodes_scanlines(void)
{
    //Guardar estado maquina spectrum (registros cpu, testados, interrupciones)
    gs_save_machine_state(&normal_z80_cpu);

    //Restaurar estado a la cpu de general sound

    //Ejecutar todos los ciclos...

    //Guardar estado maquina general sound
    gs_save_machine_state(&general_sound_z80_cpu);

    //Restaurar estado maquina spectrum
}