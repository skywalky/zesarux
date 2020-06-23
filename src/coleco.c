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

/*
0000-1FFF = BIOS ROM
2000-3FFF = Expansion port
4000-5FFF = Expansion port
6000-7FFF = RAM (1K mapped into an 8K spot)
8000-9FFF = Cart ROM 
A000-BFFF = Cart ROM 
C000-DFFF = Cart ROM      
E000-FFFF = Cart ROM 
*/

    //ROM
    if (direccion<=0x1FFF || direccion>=0x8000) {
        *tipo=COLECO_SLOT_MEMORY_TYPE_ROM;
        return &memoria_spectrum[direccion];
    }

    //RAM 1 KB
    else if (direccion>=0x6000 && direccion<=0x7FFF) {
        *tipo=COLECO_SLOT_MEMORY_TYPE_RAM;
        return &memoria_spectrum[0x6000 + (direccion & 1023)];
    }

    //Vacio
    else {
        *tipo=COLECO_SLOT_MEMORY_TYPE_EMPTY;
        return &memoria_spectrum[direccion];
    }


    
    



}


void coleco_init_memory_tables(void)
{




}


void coleco_reset(void)
{



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
        int offset=32768+bloque*16384;
		int leidos=fread(&memoria_spectrum[offset],1,16384,ptr_cartridge);
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
            //Copiar en los otros 2 segmentos

            //Antes, si es un bloque de 8kb, copiar 8kb bajos en parte alta
            if (tamanyo_archivo==8192) {
                memcpy(&memoria_spectrum[32768+8192],&memoria_spectrum[32768],8192);
            }

            memcpy(&memoria_spectrum[49152],&memoria_spectrum[32768],16384);



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

//TODO: poner a 0
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



/*
RO � Ajuste fino del tono, canal A
R1 � Ajuste aproximado del tono, canal A-
R2 � Ajuste fino del tono, canal B
R3 � Ajuste aproximado del tono, canal B
R4 � Ajuste fino del tono, canal C
R5 � Ajuste aproximado del tono, canal C
*/


//Fino: xxxxx|D3|D2|D1|D0|
//Aproximado: |xx|xx|D9|D8|D7|D6|D5|D4|

//Valores de 10 bits
z80_byte temp_coleco_audio_frecuencies[6];

int coleco_get_frequency_channel(int canal)
{
    if (canal<0 || canal>2) return 0;

    z80_byte fino,aproximado;

    fino=temp_coleco_audio_frecuencies[canal*2] & 0xF;
    aproximado=(temp_coleco_audio_frecuencies[canal*2+1] & 63);

    int frecuencia=(aproximado<<4) | fino;

    return frecuencia;
}


//Establecer frecuencia del AY con valor entrada de 10 bits. funcion TEMPORAL
void coleco_set_sn_freq(int canal,int frecuencia)
{
    z80_byte fino,aproximado;

    fino=frecuencia & 0xFF;
    aproximado=(frecuencia >>8) & 0xF;

            out_port_sn(65533,2*canal);
            out_port_sn(49149,fino);      

            out_port_sn(65533,1+2*canal);
            out_port_sn(49149,aproximado);                
}

int temp_last_coleco_audio_channel=0;

void coleco_out_port_sound(z80_byte value)
{

    //Ugly test simulating it with the AY Chip

    if (value & 128) {
        //|1 |R2|R1|R0|D3|D2|D1|D0|
        z80_byte sound_register=(value >>4) &7;
        z80_byte sound_data=value & 15;

        /*
        1: This denotes that this is a control word
R2-R0 the register number:

000 Tone 1 Frequency
001 Tone 1 Volume
010 Tone 2 Frequency
011 Tone 2 Volume
100 Tone 3 Frequency
101 Tone 3 Volume
110 Noise Control
111 Noise Volume
        */

        int cambio_frecuencia=0;
        int cambio_volumen=0;
        //int canal=0;
        int volumen_final;
        int frecuencia_final;

        int canal=sound_register/2;

        printf ("Canal: %d\n",canal);

        int tipo=sound_register & 1;



        if (canal==3) {
            /*
            110 Noise Control
            111 Noise Volume
            */
            //ruido
            if (tipo==0) {
                //|1 |1 |1 |0 |xx|FB|M1|M0|
                
                //establecer frecuencia ruido
                out_port_sn(65533,6);
                out_port_sn(49149,15); //mitad del maximo aprox (31/2)
                /*
                R6 � Control del generador de ruido, D4-DO
El periodo del generador de ruido se toma contando los cinco bits inferiores del regis-
tro de ruido cada periodo del reloj de sonido dividido por 16.
                */


            }
            if (tipo==1) {
                //Noise volume
                printf ("ruido\n");
                sn_set_volume_noise(sound_data);
            }

            return;
        }

        

        if (tipo==0) {
            cambio_frecuencia=1;
            frecuencia_final=sound_data;
            temp_last_coleco_audio_channel=canal;
        }

        if (tipo==1) {
            cambio_volumen=1;
            volumen_final=sound_data;
        }

        if (cambio_volumen) {
            out_port_sn(65533,7);
            out_port_sn(49149,255-1-2-4);

            out_port_sn(65533,8+canal);
            out_port_sn(49149,volumen_final);            
        }

        if (cambio_frecuencia) {
            temp_coleco_audio_frecuencies[canal*2]=frecuencia_final;

            out_port_sn(65533,7);
            out_port_sn(49149,255-1-2-4);

            int frecuencia=coleco_get_frequency_channel(canal);
            coleco_set_sn_freq(canal,frecuencia);
          
        }



/*
RO � Ajuste fino del tono, canal A
R1 � Ajuste aproximado del tono, canal A-
R2 � Ajuste fino del tono, canal B
R3 � Ajuste aproximado del tono, canal B
R4 � Ajuste fino del tono, canal C
R5 � Ajuste aproximado del tono, canal C

El tono de cada canal es un valor de 12 bits que se forma combinando los bits D3-DO
del registro de ajuste aproximado y los bits D7-DO del registro de ajuste fino. La uni-
dad b~sica del tono es la frecuencia de reloj ~ividida por 16 (es decir, 110.83 KHz).
Como el contador es de 12 bits, se puede g ~erar frecuencias de 27 Hz a 110 KHz.

R6 � Control del generador de ruido, D4-DO
El periodo del generador de ruido se toma contando los cinco bits inferiores del regis-
tro de ruido cada periodo del reloj de sonido dividido por 16.

R7 � Control del mezclador y de E/S
D7 No utilizado
D6 1=puerta de entrada, 0=puerta de salida
D5 Ruido en el canal C
D4 Ruido en el canal B
D3 Ruido en el canal A
D2 Tono en el canal C
D1 Tono en el canal B
DO Tono en el canal A
Seccion 30. Informacion de referencia
309
Este registro controla la mezcla de ruido y tono para cada canal y la direccion
puerta de E/S de ocho bits. Un cero en un bit de mezcla indica que la funcion
activada.

R8 � Control de amplitud del ca~al A
R9 � Control de amplitud del canal B
RA � Control de amplitud del canal C
D4
*/

    }

    else {

        //Parte alta de la frecuencia
        /*
Here's the second frequency register:

+--+--+--+--+--+--+--+--+
|0 |xx|D9|D8|D7|D6|D5|D4|
+--+--+--+--+--+--+--+--+

0: This denotes that we are sending the 2nd part of the frequency

D9-D4 is 6 more bits of frequency 


To write a 10-bit word for frequenct into the sound chip you must first
send the control word, then the second frequency register.  Note that the
second frequency register doesn't have a register number.  When you write
to it, it uses which ever register you used in the control word.

        */

       //temp_last_coleco_audio_channel


            temp_coleco_audio_frecuencies[temp_last_coleco_audio_channel*2+1]=value;


            int frecuencia=coleco_get_frequency_channel(temp_last_coleco_audio_channel);
            coleco_set_sn_freq(temp_last_coleco_audio_channel,frecuencia);
    }
}
					