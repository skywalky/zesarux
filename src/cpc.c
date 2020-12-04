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
#include <string.h>

#include "cpc.h"
#include "cpu.h"
#include "screen.h"
#include "debug.h"
#include "contend.h"
#include "joystick.h"
#include "menu.h"
#include "operaciones.h"
#include "utils.h"
#include "audio.h"
#include "ay38912.h"
#include "tape.h"


//Direcciones donde estan cada pagina de rom. 2 paginas de 16 kb
z80_byte *cpc_rom_mem_table[2];

//Direcciones donde estan cada pagina de ram. 8 paginas de 16 kb cada una
z80_byte *cpc_ram_mem_table[8];

//Direcciones actuales mapeadas para lectura, bloques de 16 kb
z80_byte *cpc_memory_paged_read[4];
//Direcciones actuales mapeadas para escritura, bloques de 16 kb
z80_byte *cpc_memory_paged_write[4];

//Constantes definidas en CPC_MEMORY_TYPE_ROM, _RAM. Se solamente usa en menu debug y breakpoints, no para el core de emulacion
z80_byte debug_cpc_type_memory_paged_read[4];

//Paginas mapeadas en cada zona de RAM. Se solamente usa en menu debug y breakpoints, no para el core de emulacion
z80_byte debug_cpc_paginas_memoria_mapeadas_read[4];

//Offset a cada linea de pantalla
z80_int cpc_line_display_table[200];

//Forzar modo video para algunos juegos (p.ej. Paperboy)
z80_bit cpc_forzar_modo_video={0};
z80_byte cpc_forzar_modo_video_modo=0;


z80_byte cpc_gate_registers[4];


//Contador de linea para lanzar interrupcion.
z80_byte cpc_scanline_counter;

//Pendiente interrupcion de cpc crtc
z80_bit cpc_crt_pending_interrupt={0};

/*

Registro 2:

Bit	Value	Function
7	1	Gate Array function
6	0
5	-	not used
4	x	Interrupt generation control
3	x	1=Upper ROM area disable, 0=Upper ROM area enable
2	x	1=Lower ROM area disable, 0=Lower ROM area enable
1	x	Screen Mode slection
0	x

*/

z80_byte cpc_border_color;

z80_byte cpc_crtc_registers[32];

z80_byte cpc_crtc_last_selected_register=0;

//0=Port A
//1=Port B
//2=Port C
//3=Control
z80_byte cpc_ppi_ports[4];

//Paleta actual de CPC
z80_byte cpc_palette_table[16];

//tabla de conversion de colores de rgb cpc (8 bit) a 32 bit
//Cada elemento del array contiene el valor rgb real, por ejemplo,
//un valor rgb 11 de cpc, en la posicion 11 de este array retorna el color en formato rgb de 32 bits
int cpc_rgb_table[32]={

//0x000000,  //negro
//0x0000CD,  //azul
//0xCD0000,  //rojo
//0xCD00CD,  //magenta
//0x00CD00,  //verde
//0x00CDCD,  //cyan

0x808080, //0
0x808080, //1
0x00FF80, //2
0xFFFF80, //3

0x000080, //4 Azul
0xFF0080, //5
0x008080, //6
0xFF8080, //7

0xFF0080, //8
0xFFFF80, //9  //Pastel Yellow
0xFFFF00, //10 //Bright Yellow
0xFFFFFF, //11

0xFF0000, //12
0xFF00FF, //13
0xFF8000, //14
0xFF80FF, //15

0x0000FF, //16
0x00FF80, //17
0x00FF00, //18
0x00FFFF, //19

0x000000, //20
0x0000FF, //21
0x008000, //22
0x0080FF, //23

0x800080, //24 
0x80FF80, //25 
0x80FF00, //26
0x80FFFF, //27

0x800000, //28
0x8000FF, //29
0x808000, //30
0x8080FF //31

};



/*Tablas teclado
Bit:
Line	7	6	5	4	3	2	1	0
&40	F Dot	ENTER	F3	F6	F9	CURDOWN	CURRIGHT	CURUP
&41	F0	F2	F1	F5	F8	F7	COPY	CURLEFT
&42	CONTROL	\	SHIFT	F4	]	RETURN	[	CLR
&43	.	/	 :	 ;	P	@	-	^
&44	,	M	K	L	I	O	9	0
&45	SPACE	N	J	H	Y	U	7	8
&46	V	B	F	G (Joy2 fire)	T (Joy2 right)	R (Joy2 left)	5 (Joy2 down)	6 (Joy 2 up)
&47	X	C	D	S	W	E	3	4
&48	Z	CAPSLOCK	A	TAB	Q	ESC	2	1
&49	DEL	Joy 1 Fire 3 (CPC only)	Joy 1 Fire 2	Joy1 Fire 1	Joy1 right	Joy1 left	Joy1 down	Joy1 up

*/


//Aunque solo son 10 filas, metemos array de 16 pues es el maximo valor de indice seleccionable por el PPI
z80_byte cpc_keyboard_table[16]={
255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255
};


z80_bit cpc_send_double_vsync={0};

z80_bit cpc_vsync_signal={0};

void cpc_set_memory_pages()
{

	//Array de paginas que entran. Por defecto en 64kb, paginas 0,1,2,3
	int pages_array[4];

	//Por defecto
	pages_array[0]=0;
	pages_array[1]=1;
	pages_array[2]=2;
	pages_array[3]=3;


	//Si es maquina de 128kb, reasignar paginas
	if (MACHINE_IS_CPC_4128) {
		z80_byte ram_config=cpc_gate_registers[3] & 7;

		//printf ("Setting 128k ram config value %d\n",ram_config);


/*
memoria extendida mas alla de 64 kb

Register 3 - RAM Banking
This register exists only in CPCs with 128K RAM (like the CPC 6128, or CPCs with Standard Memory Expansions). Note: In the CPC 6128, the register is a separate PAL that assists the Gate Array chip.

Bit	Value	Function
7	1	Gate Array function 3
6	1
5	b	64K bank number (0..7); always 0 on an unexpanded CPC6128, 0-7 on Standard Memory Expansions
4	b
3	b
2	x	RAM Config (0..7)
1	x
0	x


The 3bit RAM Config value is used to access the second 64K of the total 128K RAM that is built into the CPC 6128 
or the additional 64K-512K of standard memory expansions. These contain up to eight 64K ram banks, 
which are selected with bit 3-5. A standard CPC 6128 only contains bank 0. 
Normally the register is set to 0, so that only the first 64K RAM are used (identical to the CPC 464 and 664 models). 
The register can be used to select between the following eight predefined configurations only:

 -Address-     0      1      2      3      4      5      6      7
 0000-3FFF   RAM_0  RAM_0  RAM_4  RAM_0  RAM_0  RAM_0  RAM_0  RAM_0
 4000-7FFF   RAM_1  RAM_1  RAM_5  RAM_3  RAM_4  RAM_5  RAM_6  RAM_7
 8000-BFFF   RAM_2  RAM_2  RAM_6  RAM_2  RAM_2  RAM_2  RAM_2  RAM_2
 C000-FFFF   RAM_3  RAM_7  RAM_7  RAM_7  RAM_3  RAM_3  RAM_3  RAM_3
The Video RAM is always located in the first 64K, VRAM is in no way affected by this register.

*/	
		switch (ram_config) {

			case 1:
				pages_array[3]=7;			
			break;

			case 2:
				pages_array[0]=4;
				pages_array[1]=5;
				pages_array[2]=6;
				pages_array[3]=7;			
			break;

			case 3:
				pages_array[1]=3;
				pages_array[3]=7;			
			break;						

			case 4:
				pages_array[1]=4;
			break;	

			case 5:
				pages_array[1]=5;		
			break;	

			case 6:
				pages_array[1]=6;	
			break;	

			case 7:
				pages_array[1]=7;		
			break;													

		}


	}


	//printf ("paginas que entran: %d %d %d %d\n",pages_array[0],pages_array[1],pages_array[2],pages_array[3]);

	//Escritura siempre en RAM
	int i;
	for (i=0;i<4;i++) {
		//cpc_memory_paged_write[i]=cpc_ram_mem_table[i];
		int pagina_entra=pages_array[i];
		cpc_memory_paged_write[i]=cpc_ram_mem_table[pagina_entra];
	}



	int pagina_entra;

	//Bloque 0-16383
	if (cpc_gate_registers[2] &4 ) {
		//Entra RAM
		pagina_entra=pages_array[0];
		cpc_memory_paged_read[0]=cpc_ram_mem_table[pagina_entra];
		debug_cpc_type_memory_paged_read[0]=CPC_MEMORY_TYPE_RAM;
		debug_cpc_paginas_memoria_mapeadas_read[0]=pagina_entra;
	}
	else {
		//Entra ROM
		cpc_memory_paged_read[0]=cpc_rom_mem_table[0];
		debug_cpc_type_memory_paged_read[0]=CPC_MEMORY_TYPE_ROM;
		debug_cpc_paginas_memoria_mapeadas_read[0]=0;
	}




	//Bloque 16384-32767
	//RAM
	pagina_entra=pages_array[1];
	cpc_memory_paged_read[1]=cpc_ram_mem_table[pagina_entra];	
	debug_cpc_type_memory_paged_read[1]=CPC_MEMORY_TYPE_RAM;		
	debug_cpc_paginas_memoria_mapeadas_read[1]=pagina_entra;
	

	//Bloque 32768-49151
	//RAM
	pagina_entra=pages_array[2];
	cpc_memory_paged_read[2]=cpc_ram_mem_table[pagina_entra];	
	debug_cpc_type_memory_paged_read[2]=CPC_MEMORY_TYPE_RAM;
	debug_cpc_paginas_memoria_mapeadas_read[2]=2;
	
    //Bloque 49152-65535
    if (cpc_gate_registers[2] &8 ) {
    	//Entra RAM
		pagina_entra=pages_array[3];
        cpc_memory_paged_read[3]=cpc_ram_mem_table[pagina_entra];
		debug_cpc_type_memory_paged_read[3]=CPC_MEMORY_TYPE_RAM;
		debug_cpc_paginas_memoria_mapeadas_read[3]=pagina_entra;
    }
    else {
        //Entra ROM 
        cpc_memory_paged_read[3]=cpc_rom_mem_table[1];
		debug_cpc_type_memory_paged_read[3]=CPC_MEMORY_TYPE_ROM;
		debug_cpc_paginas_memoria_mapeadas_read[3]=1;
    }

}


void cpc_init_memory_tables()
{
	debug_printf (VERBOSE_DEBUG,"Initializing cpc memory tables");

        z80_byte *puntero;
        puntero=memoria_spectrum;

        cpc_rom_mem_table[0]=puntero;
        puntero +=16384;
        cpc_rom_mem_table[1]=puntero;
        puntero +=16384;


        int i;
        for (i=0;i<8;i++) {
                cpc_ram_mem_table[i]=puntero;
                puntero +=16384;
        }

}

void cpc_out_port_gate(z80_byte value)
{
	/*
Data Bit 7	Data Bit 6	Function
0	0	Select pen
0	1	Select colour for selected pen
1	0	Select screen mode, ROM configuration and interrupt control
1	1	RAM Memory Management (note 1)

note 1: This function is not available in the Gate-Array, but is performed by a device at the same I/O port address location. In the CPC464, CPC664 and KC compact, this function is performed in a memory-expansion (e.g. Dk'Tronics 64K RAM Expansion), if this expansion is not present then the function is not available. In the CPC6128, this function is performed by a PAL located on the main PCB, or a memory-expansion. In the 464+ and 6128+ this function is performed by the ASIC or a memory expansion. Please read the document on RAM management for more information.
	*/


        z80_byte modo_video_antes=cpc_gate_registers[2] &3;


	z80_byte funcion=(value>>6)&3;


	cpc_gate_registers[funcion]=value;


	z80_byte modo_video_despues=cpc_gate_registers[2] &3;

	if (modo_video_despues!=modo_video_antes) cpc_splash_videomode_change();


	//printf ("Changing register %d of gate array\n",funcion);

	switch (funcion) {
		case 0:
			//Seleccion indice color a paleta o border
			if (value&16) {
				//printf ("Seleccion border. TODO\n");
				z80_byte color=value&15;
				if (cpc_border_color!=color) {
					cpc_border_color=color;
					//printf ("Setting border color with value %d\n",color);
					modificado_border.v=1;
				}
			}

			else {
				//Seleccion indice. Guardado en cpc_gate_registers[0] el indice en los 4 bits inferiores
				//printf ("Setting index palette %d\n",cpc_gate_registers[0]&15);
			}
		break;

		case 1:
                        if (cpc_gate_registers[0] & 16) {
                                //printf ("Seleccion border. sin sentido aqui\n");
                        }

                        else {
                                //Seleccion color para indice. Guardado en cpc_gate_registers[0] el indice en los 4 bits inferiores
				z80_byte indice=cpc_gate_registers[0]&15;

				z80_byte color=value&31;

				cpc_palette_table[indice]=color;

				//printf ("Setting index color %d with value %d\n",indice,color);


				//Si se cambia color para indice de color de border, refrescar border
				if (indice==cpc_border_color) modificado_border.v=1;

			}


		break;
		
		case 2:
			//Cambio paginacion y modo video y gestion interrupcion
			cpc_set_memory_pages();

			if (value&16) {
				//Esto resetea bit 5 de contador de scanline
				//printf ("Resetting bit 5 of cpc_scanline_counter\n");
                //If bit 4 of the "Select screen mode and rom configuration" register of the Gate-Array 
                //(bit 7="1" and bit 6="0") is set to "1" then the interrupt request is cleared and the 6-bit counter is reset to "0".
				cpc_scanline_counter &=(255-32);
                cpc_crt_pending_interrupt.v=0;
			}
		break;

		case 3:
			//printf ("Memory management only on cpc 6128. Setting value %02XH\n",value);
			//Cambio paginacion en modos 128kb ram
			cpc_set_memory_pages();
/*
Register 3 - RAM Banking
This register exists only in CPCs with 128K RAM (like the CPC 6128, or CPCs with Standard Memory Expansions). Note: In the CPC 6128, the register is a separate PAL that assists the Gate Array chip.

Bit	Value	Function
7	1	Gate Array function 3
6	1
5	b	64K bank number (0..7); always 0 on an unexpanded CPC6128, 0-7 on Standard Memory Expansions
4	b
3	b
2	x	RAM Config (0..7)
1	x
0	x


The 3bit RAM Config value is used to access the second 64K of the total 128K RAM that is built into the CPC 6128 
or the additional 64K-512K of standard memory expansions. 
These contain up to eight 64K ram banks, which are selected with bit 3-5. A standard CPC 6128 only contains bank 0. 
Normally the register is set to 0, so that only the first 64K RAM are used (identical to the CPC 464 and 664 models). 
The register can be used to select between the following eight predefined configurations only:

 -Address-     0      1      2      3      4      5      6      7
 0000-3FFF   RAM_0  RAM_0  RAM_4  RAM_0  RAM_0  RAM_0  RAM_0  RAM_0
 4000-7FFF   RAM_1  RAM_1  RAM_5  RAM_3  RAM_4  RAM_5  RAM_6  RAM_7
 8000-BFFF   RAM_2  RAM_2  RAM_6  RAM_2  RAM_2  RAM_2  RAM_2  RAM_2
 C000-FFFF   RAM_3  RAM_7  RAM_7  RAM_7  RAM_3  RAM_3  RAM_3  RAM_3
The Video RAM is always located in the first 64K, VRAM is in no way affected by this register.



*/			
		break;
	}
}




//cpc_line_display_table

void init_cpc_line_display_table(void)
{
	debug_printf (VERBOSE_DEBUG,"Init CPC Line Display Table");
	int y;
	z80_int offset;
	for (y=0;y<200;y++) {
		offset=((y / 8) * 80) + ((y % 8) * 2048);
		cpc_line_display_table[y]=offset;
		debug_printf (VERBOSE_PARANOID,"CPC Line: %d Offset: 0x%04X",y,offset);
	}
}


//  return (unsigned char *)0xC000 + ((nLine / 8) * 80) + ((nLine % 8) * 2048);








//Decir si vsync está activo o no, según en qué posición de pantalla estamos,
//y resetear t_scanline_draw a 0 cuando finaliza dicha vsync
//Ver http://www.cpcwiki.eu/index.php/CRTC#HSYNC_and_VSYNC
/*
The VSYNC is also modified before being sent to the monitor. It happens two lines* after the VSYNC from the CRTC 
and stay two lines (same cut rule if VSYNC is lower than 4). PAL (50Hz) does need two lines VSYNC_width, and 4us HSYNC_width.
*/
void cpc_handle_vsync_state(void)
{
	//Duracion vsync
	int vsync_lenght=cpc_crtc_registers[3]&15;

	//Si es 0, en algunos chips significa 16
	if (vsync_lenght==0) vsync_lenght=16;
	//cpc_ppi_ports[1];

	if (cpc_send_double_vsync.v) vsync_lenght *=2;	

	int vsync_position=cpc_crtc_registers[7]&127;
	//esta en caracteres
	vsync_position *=8;


	int final_vsync=vsync_position+vsync_lenght;

	//Lo modificamos
	//vsync_position +=2;

	//final_vsync +=2;


	if (t_scanline_draw>=vsync_position && t_scanline_draw<final_vsync) {
        cpc_vsync_signal.v=1;

/*
The Gate-Array senses the VSYNC signal. If two HSYNCs have been detected following the start of the VSYNC 
then there are two possible actions:

If the top bit of the 6-bit counter is set to "1" (i.e. the counter >=32), then there is no interrupt request, 
and the 6-bit counter is reset to "0". (If a interrupt was requested and acknowledged it would be closer than 3
2 HSYNCs compared to the position of the previous interrupt).
If the top bit of the 6-bit counter is set to "0" (i.e. the counter <32), then a interrupt request is issued, 
and the 6-bit counter is reset to "0".
In both cases the following interrupt requests are synchronised with the VSYNC.
*/

        //TODO: no estoy del todo seguro con esto
        //No seria la condicion al reves? cpc_scanline_counter<32

        //O querra decir que se resetea el interrupt request si >=32, pero en caso contrario, no se activa un interrupt request?

        /*
        http://cpctech.cpc-live.com/source/split.html
        ;; The interrupt counter is updated every HSYNC.
        ;; The interrupt counter reset is synchronised to the start of the VSYNC.
        ;; A interrupt request is issued when the interrupt counter reaches 52.
        ;; 
        ;; The next interrupt could occur in two HSYNC times, assuming that
        ;; the previous interrupt was not serviced less than 32 lines ago.
        ;;
        ;; Otherwise the next interrupt will occur in 52+2 HSYNC times.
        ;;
        ;; A perfect split relies on predicting the position of the start of the VSYNC
        ;; and the position of the interrupts, as these are the signals we use to
        ;; synchronise with the display, and this means that we can setup the next split
        ;; block at the correct position).
        */

       //Por tanto creo que vsync solo resetea cpc_scanline_counter y nada mas
        

        if (t_scanline_draw==vsync_position+2) {
            if (cpc_scanline_counter>=32) {
            	//cpc_crt_pending_interrupt.v=0;
            }
            else {
                //cpc_crt_pending_interrupt.v=1;
                //printf("Generating vsync en counter: %d t: %d\n",cpc_scanline_counter,t_estados);
            }
        
            cpc_scanline_counter=0;
        }
        

    }
	else {
        cpc_vsync_signal.v=0;
    }

	//Y si está justo después, resetear posicion
	if (t_scanline_draw==final_vsync) t_scanline_draw=0;

	//printf ("vsync %d scanline %d scanline_draw %d\n",cpc_vsync_signal.v,t_scanline,t_scanline_draw);

}

z80_byte cpc_get_vsync_bit(void)
{

	//printf ("get vsync scanline %d scanline_draw %d : vsync %d\n",t_scanline,t_scanline_draw,cpc_vsync_signal.v);

	//if (cpc_vsync_signal.v) printf ("1111111######\n");

	return cpc_vsync_signal.v;
}

z80_byte old_cpc_get_vsync_bit(void)
{
				//Bit de vsync
			//Duracion vsync
			z80_byte vsync_lenght=cpc_crtc_registers[3]&15;

			//Si es 0, en algunos chips significa 16
			if (vsync_lenght==0) vsync_lenght=16;
			//cpc_ppi_ports[1];

		int vsync_position=cpc_crtc_registers[7]&127;
		//esta en caracteres
		vsync_position *=8;

		int vertical_total=cpc_crtc_registers[4]+1; //en R0 tambien se suma 1
		vertical_total *=8;

		int vertical_displayed=cpc_crtc_registers[6];
		vertical_displayed *=8;



		//Dynamite dan 1 se pone a comprobar bit de rsync en lineas:
		//0,52,104,156,208,260
		//y vsync empieza en linea 240 y dura 14 lineas... no se cumple nunca
		//esto sera debido a que los timings los tengo mal o las lineas se empiezan a contar diferente... anyway
		//O al obtener la linea actual, no deberia ser t_scanline, sino t_scanline quitando la duracion del ultimo vsync

		//workaround para algunos juegos, como bubble bobble. Lo hacemos durar mas
		if (cpc_send_double_vsync.v) vsync_lenght *=2;		


		int linea_actual=t_scanline;

		//linea actual hay que evitar la zona no visible de arriba (precisamente el vsync)
		linea_actual -=vsync_lenght;

		if (linea_actual<0) {
			//esta en vsync
			return 1;
		}

		//Ver si esta en zona de vsync
		//printf ("linea %d. lenght: %d vsync pos: %d vertical total: %d vertical displayed: %d\n",t_scanline,vsync_lenght,vsync_position,vertical_total,vertical_displayed);
			if (linea_actual>=vsync_position && linea_actual<=vsync_position+vsync_lenght-1) {
			//if (t_scanline>=0 && t_scanline<=7) {
				//printf ("Enviando vsync\n");
				return 1;
			}

			else {
				//printf ("No Enviando vsync\n");
				return 0;
			}

}

//http://www.cpcwiki.eu/index.php/Programming:Keyboard_scanning
//http://www.cpcwiki.eu/index.php/8255
z80_byte cpc_in_ppi(z80_byte puerto_h)
{
	
/*
Bit 9	Bit 8	PPI Function	Read/Write status
0	0	Port A data	Read/Write
0	1	Port B data	Read/Write
1	0	Port C data	Read/Write
1	1	Control	Write Only


I/O address	A9	A8	Description	Read/Write status	Used Direction	Used for
&F4xx		0	0	Port A Data	Read/Write		In/Out		PSG (Sound/Keyboard/Joystick)
&F5xx		0	1	Port B Data	Read/Write		In		Vsync/Jumpers/PrinterBusy/CasIn/Exp
&F6xx		1	0	Port C Data	Read/Write		Out		KeybRow/CasOut/PSG
&F7xx		1	1	Control		Write Only		Out		Control
*/

	z80_byte port_number=puerto_h&3;
	z80_byte valor;

	z80_byte psg_function;

	switch (port_number) {
		case 0:
			//printf ("Reading PPI port A\n");
			// cpc_ppi_ports[3]
			psg_function=(cpc_ppi_ports[2]>>6)&3;
                        if (psg_function==1) {
				//printf ("Leer de registro PSG. Registro = %d\n",ay_3_8912_registro_sel);
				if (ay_3_8912_registro_sel[ay_chip_selected]==14) {
					//leer teclado
					//Linea a leer
					z80_byte linea_teclado=cpc_ppi_ports[2] & 15;
					//printf ("Registro 14. Lee fila teclado: 0x%02X\n",linea_teclado | 0x40);


					if (initial_tap_load.v==1 && initial_tap_sequence) {
						return envia_load_ctrlenter_cpc(linea_teclado);
					}

			                //si estamos en el menu, no devolver tecla
			                if (zxvision_key_not_sent_emulated_mach() ) return 255;

           //Si esta spool file activo, generar siguiente tecla
                if (input_file_keyboard_is_playing() ) {
                                input_file_keyboard_get_key();
                }


					
					return cpc_keyboard_table[linea_teclado];
					//if (linea_teclado==8) {
					//	return 255-32;
					//}
					//return 255;
				}


				else if (ay_3_8912_registro_sel[ay_chip_selected]<14) {
					//Registros chip ay
					return in_port_ay(0xFF);
				}
			}
		break;

		case 1:
			//printf ("Reading PPI port B\n");
			valor=cpc_ppi_ports[1];

			//Metemos fabricante amstrad
			valor |= (2+4+8);

			//Refresco 50 hz
			valor |=16;

			//Parallel, expansion port a 0
			valor &=(255-64-32);

			//Bit 0 (vsync) 
			valor &=(255-1);
			valor |=cpc_get_vsync_bit();

 			if (realtape_inserted.v && realtape_playing.v) {
                        	if (realtape_last_value>=realtape_volumen) {
                                	valor=valor|128;
	                                //printf ("1 ");
        	                }
                	        else {
                        	        valor=(valor & (255-128));
                                	//printf ("0 ");
	                        }
			}
			return valor;

			
		break;

		case 2:
			//printf ("Reading PPI port C\n");
			return cpc_ppi_ports[2];
		break;

		case 3:
			//printf ("Reading PPI port control write only\n");
		break;

	}

	return 255;

}


void cpc_cassette_motor_control (int valor_bit) {
                                        //primero vemos si hay cinta insertada
                                        if (realtape_name!=NULL && realtape_inserted.v) {

                                                if (valor_bit) {
                                                        //Activar motor si es que no estaba ya activado
                                                        if (realtape_playing.v==0) {
                                                                debug_printf (VERBOSE_INFO,"CPC motor on function received. Start playing real tape");
                                                                realtape_start_playing();
                                                        }
                                                }
                                                else {
                                                        //Desactivar motor si es que estaba funcionando
                                                        if (realtape_playing.v) {
                                                                debug_printf (VERBOSE_INFO,"CPC motor off function received. Stop playing real tape");
                                                                //desactivado, hay juegos que envian motor off cuando no conviene realtape_stop_playing();
                                                        }
                                                }
                                        }
}


void cpc_out_ppi(z80_byte puerto_h,z80_byte value)
{
/*
Bit 9   Bit 8   PPI Function    Read/Write status
0       0       Port A data     Read/Write
0       1       Port B data     Read/Write
1       0       Port C data     Read/Write
1       1       Control Write Only
*/

        z80_byte port_number=puerto_h&3;
	z80_byte psg_function;

        switch (port_number) {
                case 0:
                        //printf ("Writing PPI port A value 0x%02X\n",value);
			cpc_ppi_ports[0]=value;
                break;

                case 1:
                        //printf ("Writing PPI port B value 0x%02X\n",value);
			cpc_ppi_ports[1]=value;
                break;

                case 2:
                        //printf ("Writing PPI port C value 0x%02X\n",value);
/*
Bit 7	Bit 6	Function
0	0	Inactive
0	1	Read from selected PSG register
1	0	Write to selected PSG register
1	1	Select PSG register

*/
			psg_function=(value>>6)&3;
			if (psg_function==3) {
				//Seleccionar ay chip registro indicado en port A
				//printf ("Seleccionamos PSG registro %d\n",cpc_ppi_ports[0]);
				out_port_ay(65533,cpc_ppi_ports[0]);
			}


			//temp prueba sonido AY
			if (psg_function==2) {
				//Enviar valor a psg
				//printf ("Enviamos PSG valor 0x%02X\n",cpc_ppi_ports[0]);
                                out_port_ay(49149,cpc_ppi_ports[0]);
                        }

			cpc_ppi_ports[2]=value;


                break;

                case 3:
                        //printf ("Writing PPI port control write only value 0x%02X\n",value);
			cpc_ppi_ports[3]=value;
			//CAUTION: Writing to PIO Control Register (with Bit7 set), automatically resets PIO Ports A,B,C to 00h each!
			if (value&128) {
				cpc_ppi_ports[0]=cpc_ppi_ports[1]=cpc_ppi_ports[2]=0;
				//tambien motor off
				//cpc_cassette_motor_control(0);
			}

			else {
				//Bit 7 a 0.
				/*
Otherwise, if Bit 7 is "0" then the register is used to set or clear a single bit in Port C:

 Bit 0    B        New value for the specified bit (0=Clear, 1=Set)
 Bit 1-3  N0,N1,N2 Specifies the number of a bit (0-7) in Port C
 Bit 4-6  -        Not Used
 Bit 7    SF       Must be "0" in this case
				*/
				z80_byte valor_bit=value&1;
				z80_byte mascara=1;
				z80_byte numero_bit=(value>>1)&7;
				if (numero_bit) {
					valor_bit=valor_bit << numero_bit;
					mascara=mascara<<numero_bit;
				}
				mascara = ~mascara;

				//printf ("Estableciendo Reg C: numero bit: %d valor: %d. valor antes Reg C: %d\n",numero_bit,valor_bit,cpc_ppi_ports[2]);

				cpc_ppi_ports[2] &=mascara;
				cpc_ppi_ports[2] |=valor_bit;

				//printf ("Valor despues Reg C: %d\n",cpc_ppi_ports[2]);

				if (numero_bit==4) {
					//motor control	
					cpc_cassette_motor_control(valor_bit);
				}
			}
				
                break;


        }

}



void cpc_out_port_crtc(z80_int puerto,z80_byte value)
{

	z80_byte puerto_h=(puerto>>8)&0xFF;

	z80_byte funcion=puerto_h&3;

	switch (funcion) {
		case 0:
			cpc_crtc_last_selected_register=value&31;
			//printf ("Select 6845 register %d\n",cpc_crtc_last_selected_register);
		break;

		case 1:
			//printf ("Write 6845 register %d with 0x%02X\n",cpc_crtc_last_selected_register,value);
			cpc_crtc_registers[cpc_crtc_last_selected_register]=value;
		break;

		case 2:
		break;

		case 3:
		break;
	}

}


void cpc_splash_videomode_change(void) {

	z80_byte modo_video=cpc_gate_registers[2] &3;

        char mensaje[32*24];

        switch (modo_video) {
                case 0:
                        sprintf (mensaje,"Setting screen mode 0, 160x200, 16 colours");
                break;

                case 1:
                        sprintf (mensaje,"Setting screen mode 1, 320x200, 4 colours");
                break;

                case 2:
                        sprintf (mensaje,"Setting screen mode 2, 640x200, 2 colours");
                break;

                case 3:
                        sprintf (mensaje,"Setting screen mode 3, 160x200, 4 colours (undocumented)");
                break;

                default:
                        //Esto no deberia suceder nunca
                        sprintf (mensaje,"Setting unknown vide mode");
                break;

        }

        screen_print_splash_text_center(ESTILO_GUI_TINTA_NORMAL,ESTILO_GUI_PAPEL_NORMAL,mensaje);

}




z80_int cpc_refresca_ajusta_offet(z80_int direccion_pixel)
{

	//de momento no hacemos nada
	//hasta entender que significa:
/*
On a hardware scrolling screen, there is a problem:
C7FF->C000
CFFF->C800
D7FF->D000
DFFF->D800
E7FF->E000
EFFF->E800
F7FF->F000
FFFF->F800
*/

	return direccion_pixel++;

                                        switch (direccion_pixel) {
                                                case 0x07FF:
                                                        direccion_pixel=0x0000;
                                                break;

                                                case 0x0FFF:
                                                        direccion_pixel=0x0800;
                                                break;

                                                case 0x17FF:
                                                        direccion_pixel=0x1000;
                                                break;

                                                case 0x1FFF:
                                                        direccion_pixel=0x1800;
                                                break;

                                                case 0x27FF:
                                                        direccion_pixel=0x2000;
                                                break;

                                                case 0x2FFF:
                                                        direccion_pixel=0x2800;
                                                break;

                                                case 0x37FF:
                                                        direccion_pixel=0x3000;
                                                break;

                                                case 0x3FFF:
                                                        direccion_pixel=0x3800;
                                                break;

						default:
							direccion_pixel++;
						break;


                                        }

	return direccion_pixel;
}

void scr_cpc_return_ancho_alto(int *an,int *al,int *al_car,int *off_x)
{

   int alto_caracter=(cpc_crtc_registers[9]&7)+1;




        int ancho_total=cpc_crtc_registers[1]*16;
        int total_alto=cpc_crtc_registers[6]*alto_caracter;

        //temp para living daylights
        //if (total_alto<192) total_alto=200;


        //CRTC registro: 2 valor: 46 . Normal
        //CRTC registro: 2 valor: 42. En dynamite dan 2. Esto significa mover el offset 4*16  (4 sale de 46-42)
        int offset_x=(46-cpc_crtc_registers[2])*16;

        //printf ("offset_x: %d\n",offset_x);

        //Controlar maximos
        if (ancho_total>640) ancho_total=640;
        if (total_alto>200) total_alto=200;
        if (offset_x<0) offset_x=0;
        if (offset_x+ancho_total>640) offset_x=640-ancho_total;
        //printf ("offset_x: %d\n",offset_x);



        *an=ancho_total;
        *al=total_alto;
        *al_car=alto_caracter;
        *off_x=offset_x;
}


//Hace putpixel en y e y+1, ya que pantalla cpc es de 640x200 pero hacemos 640x400 la ventana
void cpc_putpixel_zoom(int x,int y,unsigned int color)
{

	int dibujar=0;

	//if (x>255) dibujar=1;
	//else if (y>191) dibujar=1;
	if (scr_ver_si_refrescar_por_menu_activo(x/8,y/8)) dibujar=1;

	if (dibujar) {
		scr_putpixel_zoom(x,y,color);
		scr_putpixel_zoom(x,y+1,color);
	}
}

void cpc_putpixel_border(int x,int y,unsigned int color)
{

	scr_putpixel(x,y,color);

}

void scr_refresca_border_cpc(unsigned int color)
{
//      printf ("Refresco border cpc\n");


	  int alto_caracter,ancho_pantalla,alto_pantalla,offset_x_pantalla;
	scr_cpc_return_ancho_alto(&ancho_pantalla,&alto_pantalla,&alto_caracter,&offset_x_pantalla);

	//Control minimos
	if (ancho_pantalla==0) ancho_pantalla=640;
	if (alto_pantalla==0) alto_pantalla=200;


	int ancho_border=CPC_LEFT_BORDER_NO_ZOOM+   (640-ancho_pantalla)/2;
	int alto_border=CPC_TOP_BORDER_NO_ZOOM+ (200-alto_pantalla)/2;

	//printf ("ancho pantalla: %d alto_pantalla: %d offset_x_pantalla: %d anchoborder: %d altoborder: %d\n",ancho_pantalla,alto_pantalla,offset_x_pantalla,ancho_border,alto_border);



        int x,y;

	int x_borde_derecho=(ancho_border+ancho_pantalla)*zoom_x;
	//printf ("x borde derecho: %d total ventana: %d\n",x_borde_derecho,(640+CPC_LEFT_BORDER_NO_ZOOM*2)*zoom_x);


        //parte superior e inferior
        for (y=0;y<alto_border*zoom_y;y++) {
                for (x=0;x<(CPC_DISPLAY_WIDTH+CPC_LEFT_BORDER_NO_ZOOM*2)*zoom_x;x++) {
				//printf ("x: %d y: %d\n",x,y);
                                cpc_putpixel_border(x,y,color);
				cpc_putpixel_border(x,(alto_border+alto_pantalla*2)*zoom_y+y,color);
                }
        }

        //laterales
        for (y=0;y<alto_pantalla*2*zoom_y;y++) {
                for (x=0;x<ancho_border*zoom_x;x++) {
                        cpc_putpixel_border(x,alto_border*zoom_y+y,color);
                        cpc_putpixel_border(x_borde_derecho+x,alto_border*zoom_y+y,color);
                }

        }

}


//Refresco de pantalla CPC sin rainbow
void scr_refresca_pantalla_cpc(void)
{
	z80_byte modo_video=cpc_gate_registers[2] &3;

	if (cpc_forzar_modo_video.v) {
		modo_video=cpc_forzar_modo_video_modo;
	}

	switch (modo_video) {
		case 0:
			//printf ("Mode 0, 160x200 resolution, 16 colours\n");
		break;

		case 1:
			//printf ("Mode 1, 320x200 resolution, 4 colours\n");
		break;

		case 2:
			//printf ("Mode 2, 640x200 resolution, 2 colours\n");
		break;

		case 3:
			//printf ("Mode 3, 160x200 resolution, 4 colours (undocumented)\n");

		break;
	}


                                //for (bit=0;bit<8;bit++) {

                                  //      color= ( byte_leido & 128 ? ink : paper );
                                    //    scr_putpixel_zoom(x_hi+bit,y,color);

                                      //  byte_leido=byte_leido<<1;
                                //}

	z80_int x,y;
	//z80_byte *puntero;
	z80_int offset_tabla;
	z80_byte byte_leido;

	z80_int direccion_pixel;

	int color0,color1,color2,color3;

	int yfinal;
	int bit;

/*
http://www.cpcwiki.eu/index.php/CRTC
The 6845 Registers
The Internal registers of the 6845 are:

Register Index	Register Name	Range	CPC Setting	Notes
0	Horizontal Total	00000000	63	Width of the screen, in characters. Should always be 63 (64 characters). 1 character == 1μs.
1	Horizontal Displayed	00000000	40	Number of characters displayed. Once horizontal character count (HCC) matches this value, DISPTMG is set to 1.
2	Horizontal Sync Position	00000000	46	When to start the HSync signal.
3	Horizontal and Vertical Sync Widths	VVVVHHHH	128+14	HSync pulse width in characters (0 means 16 on some CRTC), should always be more than 8; VSync width in scan-lines. (0 means 16 on some CRTC. Not present on all CRTCs, fixed to 16 lines on these)
4	Vertical Total	x0000000	38	Height of the screen, in characters.
5	Vertical Total Adjust	xxx00000	0	Measured in scanlines, can be used for smooth vertical scrolling on CPC.
6	Vertical Displayed	x0000000	25	Height of displayed screen in characters. Once vertical character count (VCC) matches this value, DISPTMG is set to 1.
7	Vertical Sync position	x0000000	30	When to start the VSync signal, in characters.
8	Interlace and Skew	xxxxxx00	0	00: No interlace; 01: Interlace Sync Raster Scan Mode; 10: No Interlace; 11: Interlace Sync and Video Raster Scan Mode
9	Maximum Raster Address	xxx00000	7	Maximum scan line address on CPC can hold between 0 and 7, higher values' upper bits are ignored
10	Cursor Start Raster	xBP00000	0	Cursor not used on CPC. B = Blink On/Off; P = Blink Period Control (Slow/Fast). Sets first raster row of character that cursor is on to invert.
11	Cursor End Raster	xxx00000	0	Sets last raster row of character that cursor is on to invert
12	Display Start Address (High)	xx000000	32
13	Display Start Address (Low)	00000000	0	Allows you to offset the start of screen memory for hardware scrolling, and if using memory from address &0000 with the firmware.
14	Cursor Address (High)	xx000000	0
15	Cursor Address (Low)	00000000	0
16	Light Pen Address (High)	xx000000		Read Only
17	Light Pen Address (Low)	00000000		Read Only

*/


//http://www.grimware.org/doku.php/documentations/devices/crtc

	z80_int crtc_offset_videoram=(cpc_crtc_registers[13])  + (256*(cpc_crtc_registers[12]&3));
	z80_byte crtc_video_page=(cpc_crtc_registers[12]>>4)&3;
	//printf ("offset: %d video page: %d\n",crtc_offset_videoram,crtc_video_page);

	crtc_offset_videoram *=2;

	int alto_caracter,ancho_total,total_alto,offset_x;

	/*

	int alto_caracter=(cpc_crtc_registers[9]&7)+1;



	int ancho_total=cpc_crtc_registers[1]*16;
	int total_alto=cpc_crtc_registers[6]*alto_caracter;

	//temp para living daylights
	//if (total_alto<192) total_alto=200;


	//CRTC registro: 2 valor: 46 . Normal
	//CRTC registro: 2 valor: 42. En dynamite dan 2. Esto significa mover el offset 4*16  (4 sale de 46-42)
	int offset_x=(46-cpc_crtc_registers[2])*16;

	//printf ("offset_x: %d\n",offset_x);

	//Controlar maximos
	if (ancho_total>640) ancho_total=640;
	if (total_alto>200) total_alto=200;
	if (offset_x<0) offset_x=0;
	if (offset_x+ancho_total>640) offset_x=640-ancho_total;
	//printf ("offset_x: %d\n",offset_x);
	*/


	scr_cpc_return_ancho_alto(&ancho_total,&total_alto,&alto_caracter,&offset_x);


/*
For a 16k screen:

screen start byte offset = (((R12 & 0x03)*256) + (R13 & 255))*2

For a 32k screen:

screen start byte offset = (((R12 & 15)*256) + (R13 & 255))*2

On a hardware scrolling screen, there is a problem:
C7FF->C000
CFFF->C800
D7FF->D000
DFFF->D800
E7FF->E000
EFFF->E800
F7FF->F000
FFFF->F800
*/



      	//Debug registros crtc
	/*
        int debug_regs;

        debug_regs_muestra++;

        if ((debug_regs_muestra%50)==0) {
                for (debug_regs=0;debug_regs<18;debug_regs++) {
                        printf ("CRTC registro: %d valor: %d\n",debug_regs,cpc_crtc_registers[debug_regs]);
                }
		printf ("Alto: %d Ancho: %d Offset_x: %d\n",total_alto,ancho_total,offset_x);
        }
	*/
        //Fin Debug registros CRTC



	//Temporal. Quiza no tener que inicializar la tabla cada vez??? Esta tabla
	//sale tal cual de init_cpc_line_display_table pero cambiando valor 80
        int yy;
        z80_int offset;
        for (yy=0;yy<200;yy++) {
                //offset=((yy / 8) * cpc_crtc_registers[1]*2) + ((yy % 8) * 2048);
                offset=((yy / alto_caracter) * cpc_crtc_registers[1]*2) + ((yy % alto_caracter) * 2048);
                cpc_line_display_table[yy]=offset;
        }




	for (y=0;y<total_alto;y++){
		yfinal=y*2;
		offset_tabla=cpc_line_display_table[y];
		direccion_pixel=offset_tabla+crtc_offset_videoram;

		//puntero=cpc_ram_mem_table[3]+offset_tabla;

		//x lo incrementa cada modo por separado
		//for (x=0;x<640;) {
		for (x=offset_x;x<ancho_total+offset_x;) {
			switch (modo_video) {
		                case 0:
                		        //printf ("Mode 0, 160x200 resolution, 16 colours\n");
					//Cada pixel por cuaduplicado
					byte_leido=*(cpc_ram_mem_table[crtc_video_page]+(direccion_pixel&16383) ); //Solo offset dentro de 16kb

					//if (crtc_offset_videoram) direccion_pixel=cpc_refresca_ajusta_offet(direccion_pixel);
					//else direccion_pixel++;

					direccion_pixel++;

					color0=(byte_leido&128)>>7 | (byte_leido&8)>>2 | (byte_leido&32)>>3 | (byte_leido&2)<<2;
					color1=(byte_leido&64)>>6  | (byte_leido&4)>>1 | (byte_leido&16)>>2 | (byte_leido&1)<<3;

					color0=cpc_palette_table[color0];
                                        color0 +=CPC_INDEX_FIRST_COLOR;
                                        cpc_putpixel_zoom(x++,yfinal,color0);
                                        cpc_putpixel_zoom(x++,yfinal,color0);
                                        cpc_putpixel_zoom(x++,yfinal,color0);
                                        cpc_putpixel_zoom(x++,yfinal,color0);

					color1=cpc_palette_table[color1];
                                        color1 +=CPC_INDEX_FIRST_COLOR;
                                        cpc_putpixel_zoom(x++,yfinal,color1);
                                        cpc_putpixel_zoom(x++,yfinal,color1);
                                        cpc_putpixel_zoom(x++,yfinal,color1);
                                        cpc_putpixel_zoom(x++,yfinal,color1);


		                break;

                		case 1:

/*
Mode 1, 320×200, 4 colors (each byte of video memory represents 4 pixels):
bit 7	        bit 6	        bit 5	        bit 4	        bit 3	        bit 2	        bit 1	        bit 0
pixel 0 (bit 1)	pixel 1 (bit 1)	pixel 2 (bit 1)	pixel 3 (bit 1)	pixel 0 (bit 0)	pixel 1(bit 0)	pixel 2 (bit 0)	pixel 3 (bit 0)
*/
		                        //printf ("Mode 1, 320x200 resolution, 4 colours\n");
					//Duplicamos cada pixel en ancho
					byte_leido=*(cpc_ram_mem_table[crtc_video_page]+(direccion_pixel&16383) ); //Solo offset dentro de 16kb

					//if (crtc_offset_videoram) direccion_pixel=cpc_refresca_ajusta_offet(direccion_pixel);
					//else direccion_pixel++;
					direccion_pixel++;

					color0=(byte_leido&128)>>7 | ((byte_leido&8)>>2);
					//if (cpc_palette_table[color0]!=4) printf ("color0: %d valor: %d\n",color0,cpc_palette_table[color0]);
					color0=cpc_palette_table[color0];
					color0 +=CPC_INDEX_FIRST_COLOR;
					cpc_putpixel_zoom(x++,yfinal,color0);
					cpc_putpixel_zoom(x++,yfinal,color0);


					color1=(byte_leido&64)>>6 | ((byte_leido&4)>>1);
					color1=cpc_palette_table[color1];
					color1 +=CPC_INDEX_FIRST_COLOR;
					cpc_putpixel_zoom(x++,yfinal,color1);
					cpc_putpixel_zoom(x++,yfinal,color1);


					color2=(byte_leido&32)>>5 | ((byte_leido&2));
					color2=cpc_palette_table[color2];
					color2 +=CPC_INDEX_FIRST_COLOR;
					cpc_putpixel_zoom(x++,yfinal,color2);
					cpc_putpixel_zoom(x++,yfinal,color2);


					color3=(byte_leido&16)>>4 | ((byte_leido&1)<<1   );
					color3=cpc_palette_table[color3];
					color3 +=CPC_INDEX_FIRST_COLOR;
					cpc_putpixel_zoom(x++,yfinal,color3);
					cpc_putpixel_zoom(x++,yfinal,color3);


                		break;

		                case 2:
                		        //printf ("Mode 2, 640x200 resolution, 2 colours\n");
					byte_leido=*(cpc_ram_mem_table[crtc_video_page]+(direccion_pixel&16383) ); //Solo offset dentro de 16kb

					//if (crtc_offset_videoram) direccion_pixel=cpc_refresca_ajusta_offet(direccion_pixel);
					//else direccion_pixel++;
					direccion_pixel++;

					for (bit=0;bit<8;bit++) {
						color0=(byte_leido&128)>>7;
						color0=cpc_palette_table[color0];
	                                        color0 +=CPC_INDEX_FIRST_COLOR;
        	                                cpc_putpixel_zoom(x++,yfinal,color0);
						byte_leido=byte_leido<<1;
					}
		                break;

                		case 3:
		                        //printf ("Mode 3, 160x200 resolution, 4 colours (undocumented)\n");
					//temp
					//http://cpctech.cpc-live.com/docs/graphics.html
					if (crtc_offset_videoram) direccion_pixel=cpc_refresca_ajusta_offet(direccion_pixel);
					byte_leido=*(cpc_ram_mem_table[crtc_video_page]+(direccion_pixel&16383) ); //Solo offset dentro de 16kb
                                        direccion_pixel++;

                                        color0=(byte_leido&128)>>7 | ((byte_leido&8)>>2);
                                        //if (cpc_palette_table[color0]!=4) printf ("color0: %d valor: %d\n",color0,cpc_palette_table[color0]);
                                        color0=cpc_palette_table[color0];
                                        color0 +=CPC_INDEX_FIRST_COLOR;
                                        cpc_putpixel_zoom(x++,yfinal,color0);
                                        cpc_putpixel_zoom(x++,yfinal,color0);
                                        cpc_putpixel_zoom(x++,yfinal,color0);
                                        cpc_putpixel_zoom(x++,yfinal,color0);


                                        color1=(byte_leido&64)>>6 | ((byte_leido&4)>>1);
                                        color1=cpc_palette_table[color1];
                                        color1 +=CPC_INDEX_FIRST_COLOR;
                                        cpc_putpixel_zoom(x++,yfinal,color1);
                                        cpc_putpixel_zoom(x++,yfinal,color1);
                                        cpc_putpixel_zoom(x++,yfinal,color1);
                                        cpc_putpixel_zoom(x++,yfinal,color1);

                		break;
		        }

		}
	}


}




void scr_refresca_pantalla_y_border_cpc_no_rainbow(void)
{

        //Refrescar border si conviene
        if (border_enabled.v) {
	        if (modificado_border.v) {
        	        //Dibujar border. Color 0
			unsigned int color=cpc_border_color;
			color=cpc_palette_table[color];
			color +=CPC_INDEX_FIRST_COLOR;

	                scr_refresca_border_cpc(color);
        	        modificado_border.v=0;
	        }
	}


        scr_refresca_pantalla_cpc();
}



//Refresco pantalla con rainbow
void scr_refresca_pantalla_y_border_cpc_rainbow(void)
{



	int ancho,alto;

	ancho=get_total_ancho_rainbow();
	alto=get_total_alto_rainbow();

	int x,y;



	z80_int color_pixel;
	z80_int *puntero;

	puntero=rainbow_buffer;




	for (y=0;y<alto;y++) {




	
		for (x=0;x<ancho;x++) {





            color_pixel=*puntero++;
            scr_putpixel_zoom_rainbow(x,y,color_pixel);



		}
		
	}




}



void scr_refresca_pantalla_y_border_cpc(void)
{
    if (rainbow_enabled.v) {
        scr_refresca_pantalla_y_border_cpc_rainbow();
    }
    else {
        scr_refresca_pantalla_y_border_cpc_no_rainbow();
    }
}


void screen_store_scanline_rainbow_solo_border_cpc(void)
{
    //TODO
}


void cpc_putpixel_zoom_rainbow(int x,z80_int *puntero_buf_rainbow,int color)
{
    puntero_buf_rainbow[x]=color;
    //Siempre es x2 de alto
    puntero_buf_rainbow[x+get_total_ancho_rainbow()]=color;
}

//Guardar en buffer rainbow la linea actual. 
//Tener en cuenta que si border esta desactivado, la primera linea del buffer sera de display,
//en cambio, si border esta activado, la primera linea del buffer sera de border
//void screen_store_scanline_rainbow_solo_display_cpc(z80_int *scanline_buffer,z80_byte *vram_memory_pointer)
void screen_store_scanline_rainbow_solo_display_cpc(void)
{

    //TODO: cuadrar esto con el tamanyo del border actual segun CRTC
    //de momento:
    int inicio_pantalla;
    int final_pantalla;
    int borde_izq=CPC_LEFT_BORDER_NO_ZOOM;

    inicio_pantalla=CPC_TOP_BORDER_NO_ZOOM/2; //porque aqui consideramos scanlines, no tamanyo final
    final_pantalla=inicio_pantalla+CPC_DISPLAY_HEIGHT/2;

  //Si en zona pantalla (no border superior ni inferior)
  printf("margenes: %d %d\n",inicio_pantalla,final_pantalla);
  if (t_scanline_draw>=inicio_pantalla && t_scanline_draw<final_pantalla) {


        //linea en coordenada display (no border) que se debe leer
        int y_display=t_scanline_draw-inicio_pantalla;

 




//Renderiza una linea de display (pantalla y sprites, pero no border)
//void vdp_9918a_render_rainbow_display_line(int scanline,z80_int *scanline_buffer,z80_byte *vram)



    //Nos ubicamos ya en la zona de pixeles, saltando el border
    //En esta capa, si color=0, no lo ponemos como transparente sino como color negro
    z80_int *puntero_buf_rainbow;

    printf("%d\n",t_scanline_draw);

   int y_destino_rainbow=t_scanline_draw*2;

    
    //TODO: calculo con border desactivado
    //y_destino_rainbow=t_scanline_draw-screen_invisible_borde_superior;
    //if (border_enabled.v==0) y_destino_rainbow=y_destino_rainbow-screen_borde_superior;



    //Limite inferior y superior. Sobretodo el inferior, pues puede ser negativo (en zona border invisible)
    //En teoria superior no deberia ser mayor, pero por si acaso
    int max_y=get_total_alto_rainbow();

    if (y_destino_rainbow<0 || y_destino_rainbow>=max_y) return;

    puntero_buf_rainbow=&rainbow_buffer[borde_izq+y_destino_rainbow*get_total_ancho_rainbow()];


    //*puntero_buf_rainbow=99;



	z80_byte modo_video=cpc_gate_registers[2] &3;

	if (cpc_forzar_modo_video.v) {
		modo_video=cpc_forzar_modo_video_modo;
	}

	switch (modo_video) {
		case 0:
			//printf ("Mode 0, 160x200 resolution, 16 colours\n");
		break;

		case 1:
			//printf ("Mode 1, 320x200 resolution, 4 colours\n");
		break;

		case 2:
			//printf ("Mode 2, 640x200 resolution, 2 colours\n");
		break;

		case 3:
			//printf ("Mode 3, 160x200 resolution, 4 colours (undocumented)\n");

		break;
	}

    z80_byte byte_leido;

    //TODO
    int alto_caracter,ancho_total,total_alto,offset_x;


    offset_x=0;
    ancho_total=640;
    int x;
    int color0,color1,color2,color3;

        z80_int crtc_offset_videoram=(cpc_crtc_registers[13])  + (256*(cpc_crtc_registers[12]&3));
        z80_byte crtc_video_page=(cpc_crtc_registers[12]>>4)&3;
        //printf ("offset: %d video page: %d\n",crtc_offset_videoram,crtc_video_page);

        crtc_offset_videoram *=2;

        
        /*

        int alto_caracter=(cpc_crtc_registers[9]&7)+1;



        int ancho_total=cpc_crtc_registers[1]*16;
        int total_alto=cpc_crtc_registers[6]*alto_caracter;

        //temp para living daylights
        //if (total_alto<192) total_alto=200;


        //CRTC registro: 2 valor: 46 . Normal
        //CRTC registro: 2 valor: 42. En dynamite dan 2. Esto significa mover el offset 4*16  (4 sale de 46-42)
        int offset_x=(46-cpc_crtc_registers[2])*16;

        //printf ("offset_x: %d\n",offset_x);

        //Controlar maximos
        if (ancho_total>640) ancho_total=640;
        if (total_alto>200) total_alto=200;
        if (offset_x<0) offset_x=0;
        if (offset_x+ancho_total>640) offset_x=640-ancho_total;
        */


        scr_cpc_return_ancho_alto(&ancho_total,&total_alto,&alto_caracter,&offset_x);

        int bit;

        //Temporal. Quiza no tener que inicializar la tabla cada vez??? Esta tabla
        //sale tal cual de init_cpc_line_display_table pero cambiando valor 80
        int yy;
        z80_int offset;
        for (yy=0;yy<200;yy++) {
                //offset=((yy / 8) * cpc_crtc_registers[1]*2) + ((yy % 8) * 2048);
                offset=((yy / alto_caracter) * cpc_crtc_registers[1]*2) + ((yy % alto_caracter) * 2048);
                cpc_line_display_table[yy]=offset;
        }
        z80_int direccion_pixel;
        int yfinal;
        z80_int offset_tabla;


                yfinal=y_display;
                offset_tabla=cpc_line_display_table[yfinal];
                direccion_pixel=offset_tabla+crtc_offset_videoram;

                //puntero=cpc_ram_mem_table[3]+offset_tabla;

                //x lo incrementa cada modo por separado
                //for (x=0;x<640;) {
                    
                                                

	for (x=offset_x;x<ancho_total+offset_x;) {
			switch (modo_video) {
		                case 0:  //160x200

    

                		        //printf ("Mode 0, 160x200 resolution, 16 colours\n");
					//Cada pixel por cuaduplicado
					byte_leido=*(cpc_ram_mem_table[crtc_video_page]+(direccion_pixel&16383) ); //Solo offset dentro de 16kb

					//if (crtc_offset_videoram) direccion_pixel=cpc_refresca_ajusta_offet(direccion_pixel);
					//else direccion_pixel++;

					direccion_pixel++;

					color0=(byte_leido&128)>>7 | (byte_leido&8)>>2 | (byte_leido&32)>>3 | (byte_leido&2)<<2;
					color1=(byte_leido&64)>>6  | (byte_leido&4)>>1 | (byte_leido&16)>>2 | (byte_leido&1)<<3;

					color0=cpc_palette_table[color0];
                                        color0 +=CPC_INDEX_FIRST_COLOR;
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color0);
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color0);
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color0);
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color0);

					color1=cpc_palette_table[color1];
                                        color1 +=CPC_INDEX_FIRST_COLOR;
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color1);
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color1);
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color1);
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color1);


		    break;


               		case 1:

/*
Mode 1, 320×200, 4 colors (each byte of video memory represents 4 pixels):
bit 7	        bit 6	        bit 5	        bit 4	        bit 3	        bit 2	        bit 1	        bit 0
pixel 0 (bit 1)	pixel 1 (bit 1)	pixel 2 (bit 1)	pixel 3 (bit 1)	pixel 0 (bit 0)	pixel 1(bit 0)	pixel 2 (bit 0)	pixel 3 (bit 0)
*/
		                        //printf ("Mode 1, 320x200 resolution, 4 colours\n");
					//Duplicamos cada pixel en ancho
					byte_leido=*(cpc_ram_mem_table[crtc_video_page]+(direccion_pixel&16383) ); //Solo offset dentro de 16kb

					//if (crtc_offset_videoram) direccion_pixel=cpc_refresca_ajusta_offet(direccion_pixel);
					//else direccion_pixel++;
					direccion_pixel++;

					color0=(byte_leido&128)>>7 | ((byte_leido&8)>>2);
					//if (cpc_palette_table[color0]!=4) printf ("color0: %d valor: %d\n",color0,cpc_palette_table[color0]);
					color0=cpc_palette_table[color0];
					color0 +=CPC_INDEX_FIRST_COLOR;
					cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color0);
					cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color0);


					color1=(byte_leido&64)>>6 | ((byte_leido&4)>>1);
					color1=cpc_palette_table[color1];
					color1 +=CPC_INDEX_FIRST_COLOR;
					cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color1);
					cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color1);


					color2=(byte_leido&32)>>5 | ((byte_leido&2));
					color2=cpc_palette_table[color2];
					color2 +=CPC_INDEX_FIRST_COLOR;
					cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color2);
					cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color2);


					color3=(byte_leido&16)>>4 | ((byte_leido&1)<<1   );
					color3=cpc_palette_table[color3];
					color3 +=CPC_INDEX_FIRST_COLOR;
					cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color3);
					cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color3);


                		break;            


		                case 2:
                		        //printf ("Mode 2, 640x200 resolution, 2 colours\n");
					byte_leido=*(cpc_ram_mem_table[crtc_video_page]+(direccion_pixel&16383) ); //Solo offset dentro de 16kb

					//if (crtc_offset_videoram) direccion_pixel=cpc_refresca_ajusta_offet(direccion_pixel);
					//else direccion_pixel++;
					direccion_pixel++;

					for (bit=0;bit<8;bit++) {
						color0=(byte_leido&128)>>7;
						color0=cpc_palette_table[color0];
	                                        color0 +=CPC_INDEX_FIRST_COLOR;
        	                                cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color0);
						byte_leido=byte_leido<<1;
					}
		                break;

                		case 3:
		                        //printf ("Mode 3, 160x200 resolution, 4 colours (undocumented)\n");
					//temp
					//http://cpctech.cpc-live.com/docs/graphics.html
					if (crtc_offset_videoram) direccion_pixel=cpc_refresca_ajusta_offet(direccion_pixel);
					byte_leido=*(cpc_ram_mem_table[crtc_video_page]+(direccion_pixel&16383) ); //Solo offset dentro de 16kb
                                        direccion_pixel++;

                                        color0=(byte_leido&128)>>7 | ((byte_leido&8)>>2);
                                        //if (cpc_palette_table[color0]!=4) printf ("color0: %d valor: %d\n",color0,cpc_palette_table[color0]);
                                        color0=cpc_palette_table[color0];
                                        color0 +=CPC_INDEX_FIRST_COLOR;
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color0);
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color0);
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color0);
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color0);


                                        color1=(byte_leido&64)>>6 | ((byte_leido&4)>>1);
                                        color1=cpc_palette_table[color1];
                                        color1 +=CPC_INDEX_FIRST_COLOR;
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color1);
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color1);
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color1);
                                        cpc_putpixel_zoom_rainbow(x++,puntero_buf_rainbow,color1);

                		break;
            


            default:

            //Aqui no deberia llegar nunca. pero por si acaso

            x++;
            break;

    }
    }
    /*

	z80_byte video_mode=vdp_9918a_get_video_mode();

	//printf ("video_mode: %d\n",video_mode);


	int x,bit; 
	z80_int direccion_name_table;
	z80_byte byte_leido;
    z80_byte byte_color;
	int color=0;
	
	//int zx,zy;

	z80_byte ink,paper;


	z80_int pattern_base_address; //=2048; //TODO: Puesto a pelo
	z80_int pattern_name_table; //=0; //TODO: puesto a pelo

	pattern_name_table=vdp_9918a_get_pattern_name_table(); //(vdp_9918a_registers[2]&15) * 0x400; 



	pattern_base_address=vdp_9918a_get_pattern_base_address();


	z80_int pattern_color_table=vdp_9918a_get_pattern_color_table();


    //Sumar el offset por linea

    int fila=scanline/8;

    //entre 0 y 7 dentro de la fila
    int scanline_fila=scanline % 8;    

    int offset_sumar_linea;


	int chars_in_line;
	int char_width;



			chars_in_line=32;
			char_width=8;

            //printf ("pattern base address before mask: %d\n",pattern_base_address);

            //printf ("pattern color table before mask:  %d\n",pattern_color_table);            


			pattern_base_address &=8192; //Cae en offset 0 o 8192
          
			pattern_color_table &=8192; //Cae en offset 0 o 8192


            //printf ("pattern base address after mask: %d\n",pattern_base_address);

            //printf ("pattern color table after mask:  %d\n",pattern_color_table);

			direccion_name_table=pattern_name_table;  

            offset_sumar_linea=chars_in_line*fila;

            direccion_name_table +=offset_sumar_linea;            

			//for (y=0;y<24;y++) {

				int tercio=fila/8;

				for (x=0;x<chars_in_line;x++) {  
					
					
					z80_byte caracter=vdp_9918a_read_vram_byte(vram,direccion_name_table);
					

					//int scanline;

					z80_int pattern_address=(caracter*8+2048*tercio) ;
					pattern_address +=pattern_base_address+scanline_fila;
					
					


					z80_int color_address=(caracter*8+2048*tercio) ;
					color_address +=pattern_color_table+scanline_fila;

	
			

					//for (scanline=0;scanline<8;scanline++) {

						byte_leido=vdp_9918a_read_vram_byte(vram,pattern_address);

						byte_color=vdp_9918a_read_vram_byte(vram,color_address);


						ink=(byte_color>>4) &15;
						paper=byte_color &15;

							
						for (bit=0;bit<char_width;bit++) {

							//int columna=(x*char_width+bit)/8;

													
							//Ver en casos en que puede que haya menu activo y hay que hacer overlay
							//if (scr_ver_si_refrescar_por_menu_activo(columna,fila)) {
								color= ( byte_leido & 128 ? ink : paper );
								//scr_putpixel_zoom(x*char_width+bit,y*8+scanline,VDP_9918_INDEX_FIRST_COLOR+color);
                                *destino_scanline_buffer=VDP_9918_INDEX_FIRST_COLOR+color;
                                destino_scanline_buffer++;                                
							//}

							byte_leido=byte_leido<<1;
						}
					//}

						
					direccion_name_table++;

				}
		   //}



}



        




        
*/
  }    
  

}



void screen_store_scanline_rainbow_cpc_border_and_display(void)
{
    //Renderizar border y pantalla en buffer rainbow

    screen_store_scanline_rainbow_solo_display_cpc();
    screen_store_scanline_rainbow_solo_border_cpc();
}