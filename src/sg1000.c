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

#include "sg1000.h"
#include "vdp_9918a.h"
#include "cpu.h"
#include "debug.h"
#include "ay38912.h"
#include "tape.h"
#include "screen.h"
#include "audio.h"
#include "sn76489an.h"
#include "joystick.h"

z80_byte *sg1000_vram_memory=NULL;



//slots asignados, y sus 4 segmentos
//tipos: rom, ram, vacio
//int sg1000_memory_slots[4][4];





const char *sg1000_string_memory_type_rom="ROM";
const char *sg1000_string_memory_type_ram="RAM";
const char *sg1000_string_memory_type_empty="EMPTY";

char *sg1000_get_string_memory_type(int tipo)
{
    		
            

    switch (tipo) {

        case SG1000_SLOT_MEMORY_TYPE_ROM:
            return (char *)sg1000_string_memory_type_rom;
        break;

        case SG1000_SLOT_MEMORY_TYPE_RAM:
            return (char *)sg1000_string_memory_type_ram;
        break;

        default:
            return (char *)sg1000_string_memory_type_empty;
        break;

    }
}


//Retorna direccion de memoria donde esta mapeada la ram y su tipo
z80_byte *sg1000_return_segment_address(z80_int direccion,int *tipo)
{

/*
Region	Maps to
$0000-$bfff	Cartridge (ROM/RAM/etc)
$c000-$c3ff	System RAM
$c400-$ffff	System RAM (mirrored every 1KB)
*/

    //ROM
    if (direccion<=0xbfff) {
        *tipo=SG1000_SLOT_MEMORY_TYPE_ROM;
        return &memoria_spectrum[direccion];
    }

    //RAM 1 KB
    else {
        *tipo=SG1000_SLOT_MEMORY_TYPE_RAM;
        return &memoria_spectrum[0xc000 + (direccion & 1023)];
    }

    
    



}


void sg1000_init_memory_tables(void)
{




}


void sg1000_reset(void)
{



}

void sg1000_out_port_vdp_data(z80_byte value)
{
    vdp_9918a_out_vram_data(sg1000_vram_memory,value);
}


z80_byte sg1000_in_port_vdp_data(void)
{
    return vdp_9918a_in_vram_data(sg1000_vram_memory);
}



z80_byte sg1000_in_port_vdp_status(void)
{
    return vdp_9918a_in_vdp_status();
}

void sg1000_out_port_vdp_command_status(z80_byte value)
{
    vdp_9918a_out_command_status(sg1000_vram_memory,value);
}





void sg1000_alloc_vram_memory(void)
{
    if (sg1000_vram_memory==NULL) {
        sg1000_vram_memory=malloc(16384);
        if (sg1000_vram_memory==NULL) cpu_panic("Cannot allocate memory for sg1000 vram");
    }
}


z80_byte sg1000_read_vram_byte(z80_int address)
{
    //Siempre leer limitando a 16 kb
    return sg1000_vram_memory[address & 16383];
}



void sg1000_insert_rom_cartridge(char *filename)
{

	debug_printf(VERBOSE_INFO,"Inserting sg1000 rom cartridge %s",filename);

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

A ROM needs a header to be auto-executed by the system when the SG1000 is initialized.

After finding the RAM and initializing the system variables, the SG1000 looks for the ROM headers in all the slots 
on the memory pages 4000h-7FFFh and 8000h-FFFh. The search is done in ascending order. 
When a primary Slot is expanded, the search is done in the corresponding secondary Slots before going to the next Primary Slot.
When the system finds a header, it selects the ROM slot only on the memory page corresponding to the address specified in INIT then, runs the program in ROM at the same address. (In short, it makes an inter-slot call.)

        */
        int offset=bloque*16384;
        

		int leidos=fread(&memoria_spectrum[offset],1,16384,ptr_cartridge);
        if (leidos==16384) { 
            //sg1000_memory_slots[1][1+bloque]=SG1000_SLOT_MEMORY_TYPE_ROM;
            printf ("sg1000 loaded 16kb bytes of rom at slot 1 block %d\n",bloque);

            bloques_totales++;

        }
        else {
            salir=1;
        }

	}

    if (bloques_totales==1) {
            //Copiar en los otros segmentos

            //Antes, si es un bloque de 8kb, copiar 8kb bajos en parte alta
            if (tamanyo_archivo==8192) {
                memcpy(&memoria_spectrum[8192],&memoria_spectrum[0],8192);
            }

            memcpy(&memoria_spectrum[16384],&memoria_spectrum[0],16384);



    }
    


    
    //int i;


        fclose(ptr_cartridge);


        if (noautoload.v==0) {
                debug_printf (VERBOSE_INFO,"Reset cpu due to autoload");
                reset_cpu();
        }


}


void sg1000_empty_romcartridge_space(void)
{

//poner a 0
    int i;
    for (i=0;i<65536;i++) {
        memoria_spectrum[i]=0;
    }
}





//Refresco pantalla sin rainbow
void scr_refresca_pantalla_y_border_sg1000_no_rainbow(void)
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
            vdp_9918a_render_ula_no_rainbow(sg1000_vram_memory);
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
        vdp_9918a_render_sprites_no_rainbow(sg1000_vram_memory);
    }
        
        


}


//Refresco pantalla con rainbow
void scr_refresca_pantalla_y_border_sg1000_rainbow(void)
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


void scr_refresca_pantalla_y_border_sg1000(void)
{
    if (rainbow_enabled.v) {
        scr_refresca_pantalla_y_border_sg1000_rainbow();
    }
    else {
        scr_refresca_pantalla_y_border_sg1000_no_rainbow();
    }
}







//Almacenaje temporal de render de la linea actual
z80_int sg1000_scanline_buffer[512];


void screen_store_scanline_rainbow_sg1000_border_and_display(void) 
{

    screen_store_scanline_rainbow_vdp_9918a_border_and_display(sg1000_scanline_buffer,sg1000_vram_memory);


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
//z80_byte temp_sg1000_audio_frecuencies[6];

/*
int sg1000_get_frequency_channel(int canal)
{
    if (canal<0 || canal>2) return 0;

    z80_byte fino,aproximado;

    fino=sn_chip_registers[canal*2] & 0xF;
    aproximado=(sn_chip_registers[canal*2+1] & 63);

    int frecuencia=(aproximado<<4) | fino;

    return frecuencia;
}
*/


//Establecer frecuencia del AY con valor entrada de 10 bits. funcion TEMPORAL
/*
void sg1000_set_sn_freq(int canal,int frecuencia)
{

    canal=canal % 3; //0,1,2

    z80_byte fino,aproximado;

    fino=frecuencia & 0xF;
    aproximado=(frecuencia >>4) & 63;

    sn_set_channel_fine_tune(canal,fino);
   
    sn_set_channel_aprox_tune(canal,aproximado);
            
}

*/

/*




            Mascara de puertos 0b11000001 = 193 = 0xC1


            


            //puerto DC =  1101 1100  - mask 0b11000001 (193 decimal) = 1100000000 -> joypad A

 Player 1                        A       B
            up down left right fire/space Z

Player 2 
             q   a    o    p     m       n 


             A B cont reset 

             Z X   C    R


             //Puerto DE = 1101 1110 - mask 0b11000001 = 1100 0000

*/


z80_byte sg1000_get_joypad_a(void) 
{

    z80_byte valor_joystick=255;

/*
joypad_a (value after mask 0b11000000 = 192)
JOYPAD2_DOWN:   = 0b10000000;
JOYPAD2_UP:     = 0b01000000;
JOYPAD1_B:      = 0b00100000;
JOYPAD1_A:      = 0b00010000;
JOYPAD1_RIGHT:  = 0b00001000;
JOYPAD1_LEFT:   = 0b00000100;
JOYPAD1_DOWN:   = 0b00000010;
JOYPAD1_UP:     = 0b00000001;
}
*/

//puerto_63486    db              255  ; 5    4    3    2    1     ;3
//puerto_61438    db              255  ; 6    7    8    9    0     ;4

			//z80_byte puerto_especial_joystick=0; //Fire Up Down Left Right

			if ((puerto_especial_joystick&1)) valor_joystick &=(255-8);
			if ((puerto_especial_joystick&2)) valor_joystick &=(255-4);
			if ((puerto_especial_joystick&4)) valor_joystick &=(255-2);
			if ((puerto_especial_joystick&8)) valor_joystick &=(255-1);

			if ((puerto_especial_joystick&16)) valor_joystick &=(255-16);

            //Espacio tambien vale como Fire/A
            //puerto_32766    db              255  ; B    N    M    Simb Space ;7
            if ((puerto_32766 & 1)==0) valor_joystick &=(255-16);

            //B = Tecla Z

            //puerto_65278   db    255  ; V    C    X    Z    Sh    ;0
            if ((puerto_65278 & 2)==0) valor_joystick &=(255-32);


            //Player 2. Q
            //puerto_64510    db              255  ; T    R    E    W    Q     ;2            
            if ((puerto_64510 & 1)==0) valor_joystick &=(255-64);




            //Player 2. A
            //puerto_65022   db    255  ; G    F    D    S    A     ;1
            if ((puerto_65022 & 1)==0) valor_joystick &=(255-128);





    return valor_joystick;
}




z80_byte sg1000_get_joypad_b(void) 
{

    z80_byte valor_joystick=255;

/*
value after mask port = 0b11000001 = 193
B_TH:           = 0b10000000;
A_TH:           = 0b01000000;
CONT:           = 0b00100000;
RESET:          = 0b00010000;
JOYPAD2_B:      = 0b00001000;
JOYPAD2_A:      = 0b00000100;
JOYPAD2_RIGHT:  = 0b00000010;
JOYPAD2_LEFT:   = 0b00000001;
}
*/

//puerto_63486    db              255  ; 5    4    3    2    1     ;3
//puerto_61438    db              255  ; 6    7    8    9    0     ;4

			//z80_byte puerto_especial_joystick=0; //Fire Up Down Left Right


            //Player 2. O
            //puerto_57342    db              255  ; Y    U    I    O    P     ;5         
            if ((puerto_57342 & 2)==0) valor_joystick &=(255-1);


            //Player 2. P
            //puerto_57342    db              255  ; Y    U    I    O    P     ;5         
            if ((puerto_57342 & 1)==0) valor_joystick &=(255-2);


            //Player 2. M
            //puerto_32766    db              255  ; B    N    M    Simb Space ;7
            if ((puerto_32766 & 4)==0) valor_joystick &=(255-4);


            //Player 2. N
            //puerto_32766    db              255  ; B    N    M    Simb Space ;7
            if ((puerto_32766 & 8)==0) valor_joystick &=(255-8);            

/*
             A B cont reset 

             Z X   C    R
*/

            //Player 2. Reset (R)
            //puerto_64510    db              255  ; T    R    E    W    Q     ;2
            if ((puerto_64510 & 8)==0) valor_joystick &=(255-16);   


            //Player 2. Cont (C)
            //puerto_65278   db    255  ; V    C    X    Z    Sh    ;0
            if ((puerto_65278 & 8)==0) valor_joystick &=(255-32); 


            //A  (Z)
            //puerto_65278   db    255  ; V    C    X    Z    Sh    ;0
            if ((puerto_65278 & 2)==0) valor_joystick &=(255-64); 

            //B  (X)
            //puerto_65278   db    255  ; V    C    X    Z    Sh    ;0
            if ((puerto_65278 & 4)==0) valor_joystick &=(255-128);             


    return valor_joystick;
}