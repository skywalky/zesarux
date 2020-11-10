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
#include "contend.h"

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

//Estado de la Z80 principal
struct gs_machine_state normal_z80_cpu;

//Estado de la Z80 del general sound
struct gs_machine_state general_sound_z80_cpu;


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



z80_byte gs_peek_byte_no_time(z80_int dir)
{
    int segmento=dir/16384;

    dir &=16383;

    z80_byte *puntero;
    puntero=gs_memory_mapped[segmento];

    puntero +=dir;

    return *puntero;
}


z80_byte gs_peek_byte(z80_int dir)
{

    t_estados +=3;

    return gs_peek_byte_no_time(dir);

}


void gs_poke_byte_no_time(z80_int dir,z80_byte valor)
{
    int segmento=dir/16384;

    //Pagina de rom?
    if (!gs_memory_mapped_types[segmento]) return;

    dir &=16383;

    z80_byte *puntero;
    puntero=gs_memory_mapped[segmento];

    puntero +=dir;

    *puntero=valor;
}

void gs_poke_byte(z80_int dir,z80_byte valor)
{
    t_estados += 3;

    gs_poke_byte_no_time(dir,valor);
    
}

z80_byte gs_lee_puerto(z80_byte puerto_h,z80_byte puerto_l)
{

    //De momento
    return 255;
}


void gs_out_port(z80_int puerto,z80_byte value)
{
    //Solo los 4 bits inferiores
    value &=0xf;

    switch(puerto) {
        case 0:
            //Mapeo memoria
            gs_memory_mapping_value=value;
            printf("Setting GS mapping value: %d\n",value);
            gs_set_memory_mapping();
        break;
    }
    
}

z80_byte gs_fetch_opcode(void)
{
    return peek_byte_no_time(reg_pc);
}


void gs_contend_read(z80_int direccion,int time)
{
    t_estados += time;
}
void gs_contend_read_no_mreq(z80_int direccion,int time)
{
    t_estados += time;
}

void gs_contend_write_no_mreq(z80_int direccion,int time)
{
    t_estados += time;
}


void gs_init_peek_poke_etc(void)
{
    general_sound_z80_cpu.peek_byte_no_time=gs_peek_byte_no_time;
    general_sound_z80_cpu.peek_byte=gs_peek_byte;
    general_sound_z80_cpu.poke_byte_no_time=gs_poke_byte_no_time;
    general_sound_z80_cpu.poke_byte=gs_poke_byte;
    general_sound_z80_cpu.lee_puerto=gs_lee_puerto;
    general_sound_z80_cpu.out_port=gs_out_port;
    general_sound_z80_cpu.fetch_opcode=fetch_opcode;
    general_sound_z80_cpu.contend_read=gs_contend_read;
    general_sound_z80_cpu.contend_read_no_mreq=gs_contend_read_no_mreq;
    general_sound_z80_cpu.contend_write_no_mreq=gs_contend_write_no_mreq;
}

void gs_init_memory_tables(void)
{
    gs_rom_memory_tables[0]=gs_memory_pointer;
    gs_rom_memory_tables[1]=&gs_memory_pointer[16384];

    int i;

    for (i=0;i<GS_RAM_BLOCKS;i++) {
        gs_ram_memory_tables[i]=&gs_memory_pointer[32768+16384*i];
    }
}




void gs_reset(void)
{
    gs_memory_mapping_value=0;
    gs_set_memory_mapping();

    general_sound_z80_cpu.r_pc=0;
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
    gs_init_peek_poke_etc();
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
    m->contend_read=contend_read;
    m->contend_read_no_mreq=contend_read_no_mreq;
    m->contend_write_no_mreq=contend_write_no_mreq;

}



void gs_restore_machine_state(struct gs_machine_state *m)
{
    reg_pc=m->r_pc=reg_pc;
    reg_sp=m->r_sp=reg_sp;

    BC=m->r_bc;
    DE=m->r_de;
    HL=m->r_hl=HL;

    reg_a=value_16_to_8h(m->r_af);
    Z80_FLAGS=value_16_to_8l(m->r_af);

    reg_b_shadow=value_16_to_8h(m->r_bc_shadow);
    reg_c_shadow=value_16_to_8l(m->r_bc_shadow);

    reg_d_shadow=value_16_to_8h(m->r_de_shadow);
    reg_e_shadow=value_16_to_8l(m->r_de_shadow);

    reg_h_shadow=value_16_to_8h(m->r_hl_shadow);
    reg_l_shadow=value_16_to_8l(m->r_hl_shadow);

    reg_a_shadow=value_16_to_8h(m->r_af_shadow);
    Z80_FLAGS_SHADOW=value_16_to_8l(m->r_af_shadow);

    
    reg_ix=m->r_ix;
    reg_iy=m->r_iy;

//header[20]=(reg_r&127) | (reg_r_bit7&128);

    reg_i=value_16_to_8h(m->r_ir);
    reg_r=value_16_to_8l(m->r_ir) & 127;
    reg_r_bit7=value_16_to_8l(m->r_ir) & 128;
    

    iff1.v=m->iff1.v;
    iff2.v=m->iff2.v;
    im_mode=m->im_mode;

    t_estados=m->t_estados;

    z80_ejecutando_halt.v=m->z80_ejecutando_halt.v;


    poke_byte=m->poke_byte;
    poke_byte_no_time=m->poke_byte_no_time;
    peek_byte=m->peek_byte;
    peek_byte_no_time=m->peek_byte_no_time;
    lee_puerto=m->lee_puerto;
    out_port=m->out_port;
    fetch_opcode=m->fetch_opcode;

    contend_read=m->contend_read;
    contend_read_no_mreq=m->contend_read_no_mreq;
    contend_write_no_mreq=m->contend_write_no_mreq;

/*
    void (*contend_read)(z80_int direccion,int time);
    void (*contend_read_no_mreq)(z80_int direccion,int time);
    void (*contend_write_no_mreq)(z80_int direccion,int time);
    */

}



void gs_run_scanline_cycles(void)
{
    //de momento unos pocos ciclos y salir

    int i;

    for (i=0;i<50;i++) {

        printf("Fetch GS opcode at PC=%04XH\n",reg_pc);

        t_estados +=4;
        z80_byte byte_leido=fetch_opcode();



        reg_pc++;

        reg_r++;




        codsinpr[byte_leido]  () ;


    }
}


//Ejecutar los opcodes de todo un scanline
void gs_fetch_opcodes_scanlines(void)
{
    //Guardar estado maquina spectrum (registros cpu, testados, interrupciones)
    gs_save_machine_state(&normal_z80_cpu);

    //Restaurar estado a la cpu de general sound
    gs_restore_machine_state(&general_sound_z80_cpu);

    //Ejecutar todos los ciclos...
    gs_run_scanline_cycles();

    //Guardar estado maquina general sound
    gs_save_machine_state(&general_sound_z80_cpu);

    //Restaurar estado maquina spectrum
    gs_restore_machine_state(&normal_z80_cpu);
}