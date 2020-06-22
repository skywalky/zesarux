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
#include <string.h>

#include "coleco.h"
#include "vdp_9918a.h"
#include "cpu.h"
#include "debug.h"
#include "ay38912.h"
#include "tape.h"
#include "screen.h"
#include "audio.h"

z80_byte *coleco_vram_memory=NULL;

z80_byte coleco_ppi_register_a;
//bit 0-1:slot segmento 0 (0000h-3fffh)
//bit 2-3:slot segmento 1 (4000h-7fffh)
//bit 4-5:slot segmento 2 (8000h-bfffh)
//bit 6-7:slot segmento 3 (C000h-ffffh)


z80_byte coleco_ppi_register_b;
z80_byte coleco_ppi_register_c;


//Aunque solo son 10 filas, metemos array de 16 pues es el maximo valor de indice seleccionable por el PPI
z80_byte coleco_keyboard_table[16]={
255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255
};


//slots asignados, y sus 4 segmentos
//tipos: rom, ram, vacio
int coleco_memory_slots[4][4];





const char *coleco_string_memory_type_rom="ROM";
const char *coleco_string_memory_type_ram="RAM";
const char *coleco_string_memory_type_empty="EMPTY";

char *coleco_get_string_memory_type(int tipo)
{
    		

    switch (tipo) {

        case COLECO_SLOT_MEMORY_TYPE_ROM:
            return (char *)coleco_string_memory_type_rom;
        break;

        case COLECO_SLOT_MEMORY_TYPE_RAM:
            return (char *)coleco_string_memory_type_ram;
        break;

        default:
            return (char *)coleco_string_memory_type_empty;
        break;

    }
}


//Retorna direccion de memoria donde esta mapeada la ram y su tipo
z80_byte *coleco_return_segment_address(z80_int direccion,int *tipo)
{
    int segmento=direccion/16384;
   
    z80_byte slot=coleco_ppi_register_a;

    int rotar=segmento*2;

    if (segmento>0) {
        slot=slot>>rotar;
    }

    slot &=3;

     

    *tipo=coleco_memory_slots[slot][segmento];

    int offset=((slot*4)+segmento)*16384;

    //if (slot>0) printf ("direccion %6d segmento %d slot %d offset %d\n",direccion,segmento,slot,offset);

    return &memoria_spectrum[offset+(direccion&16383)];



}


void coleco_init_memory_tables(void)
{

    //inicio con todos los slots vacios
    int slot,segment;
    for (slot=0;slot<4;slot++) {
        for (segment=0;segment<4;segment++) {
            coleco_memory_slots[slot][segment]=COLECO_SLOT_MEMORY_TYPE_EMPTY;
        }

    }


    //De momento meter 32 kb rom, 32 kb ram
    coleco_memory_slots[0][0]=COLECO_SLOT_MEMORY_TYPE_ROM;
    coleco_memory_slots[0][1]=COLECO_SLOT_MEMORY_TYPE_ROM;


    coleco_memory_slots[2][0]=COLECO_SLOT_MEMORY_TYPE_RAM;
    coleco_memory_slots[2][1]=COLECO_SLOT_MEMORY_TYPE_RAM;
    coleco_memory_slots[2][2]=COLECO_SLOT_MEMORY_TYPE_RAM;
    coleco_memory_slots[2][3]=COLECO_SLOT_MEMORY_TYPE_RAM;



 


}


void coleco_reset(void)
{
    //Mapear inicialmente todo a slot 0
    coleco_ppi_register_a=0;



}

void coleco_out_port_vdp_data(z80_byte value)
{
    vdp_9918a_out_vram_data(coleco_vram_memory,value);
}


z80_byte coleco_in_port_vdp_data(void)
{
    return vdp_9918a_in_vram_data(coleco_vram_memory);
}



z80_byte coleco_in_port_vdp_status(void)
{
    return vdp_9918a_in_vdp_status();
}

void coleco_out_port_vdp_command_status(z80_byte value)
{
    vdp_9918a_out_command_status(coleco_vram_memory,value);
}


void coleco_out_port_ppi(z80_byte puerto_l,z80_byte value)
{
    //printf ("Out port ppi. Port %02XH value %02XH\n",puerto_l,value);

    //int slot,segment;

    switch (puerto_l) {
        case 0xA8:
            coleco_ppi_register_a=value;
            //printf ("Out port ppi. Port %02XH value %02XH\n",puerto_l,value);

    //temporal mostrar mapeos
    
/*
    for (slot=0;slot<4;slot++) {
        for (segment=0;segment<4;segment++) {
            printf ("%d %d : %d\n",slot,segment,coleco_memory_slots[slot][segment]);
        }
    }
*/

        break;

        case 0xA9:
            coleco_ppi_register_b=value;
        break;


        case 0xAA:
            coleco_ppi_register_c=value;

            
                //printf ("Posible beep: %d\n",value&128);
            
			//set_value_beeper_on_array(da_amplitud_speaker_coleco() );


        break;
    }
}

z80_byte coleco_in_port_ppi(z80_byte puerto_l)
{
    //printf ("In port ppi. Port %02XH\n",puerto_l);

    //z80_byte valor;

    switch (puerto_l) {

        case 0xA8:
            return coleco_ppi_register_a;
        break;        
 
        case 0xA9:
            //Leer registro B (filas teclado)
            //que fila? coleco_ppi_register_c

            //si estamos en el menu, no devolver tecla
            if (zxvision_key_not_sent_emulated_mach() ) return 255;
            
            return coleco_keyboard_table[coleco_ppi_register_c & 0x0F];

        break;

        case 0xAA:
        //printf ("read tape??\n");


		/*valor=0;
                if (realtape_inserted.v && realtape_playing.v) {
                        if (realtape_last_value>=realtape_volumen) {
                                valor=valor|128;
                                printf ("1 ");
                        }
                        else {
                                valor=(valor & (255-128));
                                printf ("0 ");
                        }
                }	
		return valor;*/



            //Devolver lo mismo que se ha escrito? TODO revisar esto
            return coleco_ppi_register_c;
        break;

    }

    return 255; //temp
}


void coleco_out_port_psg(z80_byte puerto_l,z80_byte value)
{
    //printf ("Out port psg. Port %02XH value %02XH\n",puerto_l,value);


        //Registro
        if (puerto_l==0xA0) {
                        activa_ay_chip_si_conviene();
                        if (ay_chip_present.v==1) out_port_ay(65533,value);
                }
        //Datos
        if (puerto_l==0xA1) {
                        activa_ay_chip_si_conviene();
                        if (ay_chip_present.v==1) {
                            
                            if (ay_3_8912_registro_sel[ay_chip_selected]==14 || ay_3_8912_registro_sel[ay_chip_selected]==15) {

                                //de momento registros 14 y 15 nada
                            }
                            else out_port_ay(49149,value);

                        }
        }    
 
}


void coleco_alloc_vram_memory(void)
{
    if (coleco_vram_memory==NULL) {
        coleco_vram_memory=malloc(16384);
        if (coleco_vram_memory==NULL) cpu_panic("Cannot allocate memory for coleco vram");
    }
}


z80_byte coleco_read_vram_byte(z80_int address)
{
    //Siempre leer limitando a 16 kb
    return coleco_vram_memory[address & 16383];
}



void coleco_insert_rom_cartridge(char *filename)
{

	debug_printf(VERBOSE_INFO,"Inserting coleco rom cartridge %s",filename);

    long tamanyo_archivo=get_file_size(filename);

    if (tamanyo_archivo!=8192 && tamanyo_archivo!=16384 && tamanyo_archivo!=32768) {
        debug_printf(VERBOSE_ERR,"Only 8k, 16k and 32k rom cartridges are allowed");
        return;
    }

        FILE *ptr_cartridge;
        ptr_cartridge=fopen(filename,"rb");

        if (!ptr_cartridge) {
		debug_printf (VERBOSE_ERR,"Unable to open cartridge file %s",filename);
                return;
        }



	//Leer cada bloque de 16 kb si conviene. Esto permite tambien cargar cartucho de 8kb como si fuera de 16kb

	int bloque;

    int salir=0;

    int bloques_totales=0;

	for (bloque=0;bloque<2 && !salir;bloque++) {
        /*
        The ROM Header

A ROM needs a header to be auto-executed by the system when the COLECO is initialized.

After finding the RAM and initializing the system variables, the COLECO looks for the ROM headers in all the slots 
on the memory pages 4000h-7FFFh and 8000h-FFFh. The search is done in ascending order. 
When a primary Slot is expanded, the search is done in the corresponding secondary Slots before going to the next Primary Slot.
When the system finds a header, it selects the ROM slot only on the memory page corresponding to the address specified in INIT then, runs the program in ROM at the same address. (In short, it makes an inter-slot call.)

        */
        int offset=65536+bloque*16384;
		int leidos=fread(&memoria_spectrum[offset+16384],1,16384,ptr_cartridge);
        if (leidos==16384) {
            coleco_memory_slots[1][1+bloque]=COLECO_SLOT_MEMORY_TYPE_ROM;
            printf ("loaded 16kb bytes of rom at slot 1 block %d\n",bloque);

            bloques_totales++;


        }
        else {
            salir=1;
        }

	}

    if (bloques_totales==1) {
            //Copiar en los otros 3 segmentos

            //Antes, si es un bloque de 8kb, copiar 8kb bajos en parte alta
            if (tamanyo_archivo==8192) {
                memcpy(&memoria_spectrum[65536+16384+8192],&memoria_spectrum[65536+16384],8192);
            }

            memcpy(&memoria_spectrum[65536],&memoria_spectrum[65536+16384],16384);
            memcpy(&memoria_spectrum[65536+32768],&memoria_spectrum[65536+16384],16384);
            memcpy(&memoria_spectrum[65536+49152],&memoria_spectrum[65536+16384],16384);

            coleco_memory_slots[1][0]=COLECO_SLOT_MEMORY_TYPE_ROM;
            coleco_memory_slots[1][2]=COLECO_SLOT_MEMORY_TYPE_ROM;
            coleco_memory_slots[1][3]=COLECO_SLOT_MEMORY_TYPE_ROM;
    }


    
    //int i;


        fclose(ptr_cartridge);


        if (noautoload.v==0) {
                debug_printf (VERBOSE_INFO,"Reset cpu due to autoload");
                reset_cpu();
        }


}


void coleco_empty_romcartridge_space(void)
{

    int i;
    for (i=0;i<4;i++) {
        coleco_memory_slots[1][i]=COLECO_SLOT_MEMORY_TYPE_EMPTY;
    }

}





//Refresco pantalla sin rainbow
void scr_refresca_pantalla_y_border_coleco_no_rainbow(void)
{

 

    if (border_enabled.v && vdp_9918a_force_disable_layer_border.v==0) {
            //ver si hay que refrescar border
            if (modificado_border.v)
            {
                    vdp_9918a_refresca_border();
                    modificado_border.v=0;
            }

    }


    if (vdp_9918a_force_disable_layer_ula.v==0) {

        //Capa activada. Pero tiene reveal?

        if (vdp_9918a_reveal_layer_ula.v) {
            //En ese caso, poner fondo tramado
            int x,y;
            for (y=0;y<192;y++) {
                for (x=0;x<256;x++) {
                    int posx=x&1;
			        int posy=y&1;
                    int si_blanco_negro=posx ^ posy;
                    int color=si_blanco_negro*15;
                    scr_putpixel_zoom(x,y,  VDP_9918_INDEX_FIRST_COLOR+color);
                }
            }
        }
        else {
            vdp_9918a_render_ula_no_rainbow(coleco_vram_memory);
        }
    }

    else {
        //En ese caso, poner fondo negro
        int x,y;
        for (y=0;y<192;y++) {
            for (x=0;x<256;x++) {
                scr_putpixel_zoom(x,y,  VDP_9918_INDEX_FIRST_COLOR+0);
            }
        }
    }



    if (vdp_9918a_force_disable_layer_sprites.v==0) {
        vdp_9918a_render_sprites_no_rainbow(coleco_vram_memory);
    }
        
        


}


//Refresco pantalla con rainbow
void scr_refresca_pantalla_y_border_coleco_rainbow(void)
{


	//aqui no tiene sentido (o si?) el modo simular video zx80/81 en spectrum
	int ancho,alto;

	ancho=get_total_ancho_rainbow();
	alto=get_total_alto_rainbow();

	int x,y,bit;

	//margenes de zona interior de pantalla. Para overlay menu
	int margenx_izq=screen_total_borde_izquierdo*border_enabled.v;
	int margenx_der=screen_total_borde_izquierdo*border_enabled.v+256;
	int margeny_arr=screen_borde_superior*border_enabled.v;
	int margeny_aba=screen_borde_superior*border_enabled.v+192;



	//para overlay menu tambien
	//int fila;
	//int columna;

	z80_int color_pixel;
	z80_int *puntero;

	puntero=rainbow_buffer;
	int dibujar;



	for (y=0;y<alto;y++) {


		int altoborder=screen_borde_superior;

		
		for (x=0;x<ancho;x+=8) {
			dibujar=1;

			//Ver si esa zona esta ocupada por texto de menu u overlay

			if (y>=margeny_arr && y<margeny_aba && x>=margenx_izq && x<margenx_der) {
				if (!scr_ver_si_refrescar_por_menu_activo( (x-margenx_izq)/8, (y-margeny_arr)/8) )
					dibujar=0;
			}


			if (dibujar==1) {
					for (bit=0;bit<8;bit++) {
						color_pixel=*puntero++;
						scr_putpixel_zoom_rainbow(x+bit,y,color_pixel);
					}
			}
			else puntero+=8;

		}
		
	}




}


void scr_refresca_pantalla_y_border_coleco(void)
{
    if (rainbow_enabled.v) {
        scr_refresca_pantalla_y_border_coleco_rainbow();
    }
    else {
        scr_refresca_pantalla_y_border_coleco_no_rainbow();
    }
}







//Almacenaje temporal de render de la linea actual
z80_int coleco_scanline_buffer[512];


void screen_store_scanline_rainbow_coleco_border_and_display(void) 
{

    screen_store_scanline_rainbow_vdp_9918a_border_and_display(coleco_scanline_buffer,coleco_vram_memory);


}
					