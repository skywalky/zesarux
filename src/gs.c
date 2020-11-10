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

z80_byte gs_command_register;
z80_byte gs_data_register;
z80_byte gs_state_register;

z80_byte gs_port3_from_gs;

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
    gs_memory_mapped_types[1]=1;

    z80_byte mapped_value=gs_memory_mapping_value & 15;

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

        printf("Mapping RAMS %d and %d in upper segment. reg_pc=%04XH\n",mapped_value,mapped_value+1,reg_pc);
    }
}



z80_byte gs_peek_byte_no_time(z80_int dir_orig)
{

    z80_int dir=dir_orig;

    int segmento=dir/16384;

    dir &=16383;

    z80_byte *puntero;
    puntero=gs_memory_mapped[segmento];

    puntero +=dir;

    z80_byte valor=*puntero;

    /*
Data are entered into channels when the processor reads RAM at addresses # 6000
- # 7FFF automatically.
Data for channels should be located at the following addresses:
╔═════════════╤═══════════════╗
║address bit  │ for channel   ║ # 6000 - # 60FF - channel 1 data
║             ├───┬───┬───┬───╢ # 6100 - # 61FF - channel 2 data
║             │ 1 │ 2 │ 3 │ 4 ║ # 6200 - # 62FF - channel 3 data
╟─────────────┼───┼───┼───┼───╢ # 6300 - # 63FF - channel 4 data
║ A0 - A7     │ X │ X │ X │ X ║ # 6400 - # 64FF - channel 1 data    
    */

   if (dir_orig>=0x6000 && dir_orig<=0x7fff) {
       //write DAC

       //temporal
       printf ("Send DAC %d\n",valor);
       audiodac_send_sample_value(valor);
   }


    return valor;
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
    if (!gs_memory_mapped_types[segmento]) {
        printf ("Trying to write to %04XH but mapped ROM\n",dir);
        return;
    }

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

//#define gs->pstate gs_state_register
//#define gs->pb3_gs gs_port3_from_gs
//#define gs->pbb_zx gs_command_register

z80_byte gs_rp0,gs_vol1;

z80_byte gs_lee_puerto(z80_byte puerto_h,z80_byte puerto_l)
{
    //Solo los 4 bits inferiores
    puerto_l &=0xf;

	switch (puerto_l) {
		case 0: break;
		case 1: return gs_command_register; break;
		case 2: gs_state_register &= 0x7f; return gs_port3_from_gs; break;
		case 3: gs_state_register |= 0x80; break;
		case 4: return gs_state_register; break;
		case 5: gs_state_register &= 0xfe; break;
		case 6: break;
		case 7: break;
		case 8: break;
		case 9: break;

		case 10: if (gs_rp0 & 0x01) gs_state_register &= 0x7f; else gs_state_register |= 0x80; break;
		case 11: if (gs_vol1 & 0x20) gs_state_register |= 1; else gs_state_register &= 0xfe; break;
	}

    return 255;
}



void gs_out_port(z80_int puerto,z80_byte value)
{
    //Solo los 4 bits inferiores
    puerto &=0xf;

    switch(puerto) {
        case 0:
            //Mapeo memoria
            gs_memory_mapping_value=value;
            printf("Setting GS mapping value: %d\n",value);
            gs_set_memory_mapping();
        break;

        /*case 3:
		    gs_state_register |= 0x80;
			gs_port3_from_gs = value;
			break;        
        break;*/

        case 2: gs_state_register &= 0x7f;
			break;
		case 3: gs_state_register |= 0x80;
			gs_port3_from_gs = value & 0xff;
			break;
		case 4: break;
		case 5: gs_state_register &= 0xfe;
			break;
            /*
		case 6: gs->vol1 = value & 0x3f;
			break;
		case 7: gs->vol2 = value & 0x3f;
			break;
		case 8: gs->vol3 = value & 0x3f;
			break;
		case 9: gs->vol4 = value & 0x3f;
			break;
            */
            
		case 10: if (gs_rp0 & 0x01)
				gs_state_register &= 0x7f;
			else
				gs_state_register |= 0x80;
			break;
		case 11: if (gs_vol1 & 0x20)
				gs_state_register |= 1;
			else
				gs_state_register &= 0xfe;
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
    general_sound_z80_cpu.fetch_opcode=gs_fetch_opcode;
    general_sound_z80_cpu.contend_read=gs_contend_read;
    general_sound_z80_cpu.contend_read_no_mreq=gs_contend_read_no_mreq;
    general_sound_z80_cpu.contend_write_no_mreq=gs_contend_write_no_mreq;
    general_sound_z80_cpu.memoria_spectrum=gs_memory_pointer;
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
    gs_state_register=0x7e;
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

    m->memoria_spectrum=memoria_spectrum;

}



void gs_restore_machine_state(struct gs_machine_state *m)
{
    reg_pc=m->r_pc;
    reg_sp=m->r_sp;

    BC=m->r_bc;
    DE=m->r_de;
    HL=m->r_hl;

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

    memoria_spectrum=m->memoria_spectrum;


}



void gs_run_scanline_cycles(void)
{
    //de momento unos pocos ciclos y salir

    int i;

    for (i=0;i<50;i++) {

        //printf("Fetch GS opcode at PC=%04XH\n",reg_pc);

        t_estados +=4;
        z80_byte byte_leido=fetch_opcode();



        reg_pc++;

        reg_r++;




        codsinpr[byte_leido]  () ;




    }

    //prueba generar interrupcion
    if (iff1.v==1) {
        printf("Generar interrupcion en GS\n");
					

			if (z80_ejecutando_halt.v) {
				z80_ejecutando_halt.v=0;
				reg_pc++;
			}

				push_valor(reg_pc,PUSH_VALUE_TYPE_MASKABLE_INTERRUPT);

				reg_r++;

		

                //IM0??

                
				//desactivar interrupciones al generar una
				iff1.v=iff2.v=0;
				//Modelos spectrum

				if (im_mode==0 || im_mode==1) {
					cpu_common_jump_im01();
				}   
                else {
                    printf("IM 2----------\n");
                    
                    
				//IM 2.

					z80_int temp_i;
					z80_byte dir_l,dir_h;

					

					temp_i=reg_i*256+255;
					dir_l=peek_byte(temp_i++);
					dir_h=peek_byte(temp_i);
					reg_pc=value_8_to_16(dir_h,dir_l);
					t_estados += 7;


					
				
                    
                    
                }

               //NMI
				/*
				iff1.v=0;
				//printf ("Calling NMI with pc=0x%x\n",reg_pc);

				//Otros 6 estados
				t_estados += 6;

				//Total NMI: NMI WAIT 14 estados + NMI CALL 12 estados
				reg_pc= 0x66;   */                                         
    }
}


//Ejecutar los opcodes de todo un scanline
void gs_fetch_opcodes_scanlines(void)
{

    //printf("reg pc antes de ciclo GS %04XH\n",reg_pc);

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

    //printf("reg pc despues de ciclo GS %04XH\n",reg_pc);
}


//z80_byte gs_command_register;
//z80_byte gs_data_register;
//z80_byte gs_state_register;

//Lectura y escritura desde la parte de spectrum
void gs_write_port_bb_from_speccy(z80_byte value)
{
    printf("Write port BB from speccy side. value %02XH\n",value);

    gs_command_register=value;

	gs_state_register |= 1;    
}

void gs_write_port_b3_from_speccy(z80_byte value)
{
    printf("Write port B3 from speccy side. value %02XH\n",value);
    gs_data_register=value;

	gs_state_register |= 0x80;		

}

z80_byte gs_read_port_bb_from_speccy(void)
{
    printf("Read port BB from speccy side. value = %d.\n",gs_state_register);    

	


    return gs_state_register;
}

z80_byte gs_read_port_b3_from_speccy(void)
{
    printf("Read port B3 from speccy side.\n");    

    gs_state_register &= 0x7f;
    return gs_port3_from_gs;
}


