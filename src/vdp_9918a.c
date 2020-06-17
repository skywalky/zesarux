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


#include "vdp_9918a.h"
#include "cpu.h"
#include "debug.h"

#include "screen.h"

z80_byte vdp_9918a_registers[8];


//Ultimos dos bytes enviados al puerto de comando/status
z80_byte vdp_9918a_last_command_status_bytes[2];
//Contador a ultima posicion
int vdp_9918a_last_command_status_bytes_counter=0;

//Ultimos 3 bytes enviados al puerto de datos. Realmente el 0 y 1 es el address pointer actual, y el 3 el ultimo byte enviado
z80_byte vdp_9918a_last_vram_bytes[3];
//Contador a ultima posicion
//int vdp_9918a_last_vram_bytes_counter=0;

z80_int vdp_9918a_last_vram_position;


void vdp_9918a_out_vram_data(z80_byte *vram_memory,z80_byte value)
{

//printf ("%c",(value>=32 && value<=126 ? value : '?') );
    int posicion_escribir=vdp_9918a_last_vram_position & 16383;

    vram_memory[posicion_escribir]=value;

    vdp_9918a_last_vram_position++;


}


z80_byte vdp_9918a_in_vram_data(z80_byte *vram_memory)
{

//printf ("%c",(value>=32 && value<=126 ? value : '?') );
    int posicion_leer=vdp_9918a_last_vram_position & 16383;

    z80_byte value=vram_memory[posicion_leer];

    vdp_9918a_last_vram_position++;

    return value;


}

z80_byte vdp_9918a_in_vdp_status(void)
{
    //7 6  5 43210
    //F 5S C Fifth sprite number

    //TODO
    return 255;

}


void vdp_9918a_out_command_status(z80_byte *vram_memory,z80_byte value)
{
    //printf ("vdp_9918a write status: %02XH position: %d\n",value,vdp_9918a_last_command_status_bytes_counter);

    switch (vdp_9918a_last_command_status_bytes_counter) {
        case 0:
            vdp_9918a_last_command_status_bytes[0]=value;
            vdp_9918a_last_command_status_bytes_counter=1;
        break;

        case 1:
            vdp_9918a_last_command_status_bytes[1]=value;
            vdp_9918a_last_command_status_bytes_counter=0;

            //Recibido los dos bytes. Ver que comando es
            if ( (vdp_9918a_last_command_status_bytes[1] &  (128+64)) == 64 ) {
                //printf ("Write VDP Address setup.\n");

                //vdp_9918a_last_vram_position=(vdp_9918a_last_command_status_bytes[1] & 63) | (vdp_9918a_last_command_status_bytes[0]<<6);


                vdp_9918a_last_vram_position=(vdp_9918a_last_command_status_bytes[0]) | ((vdp_9918a_last_command_status_bytes[1] & 63)<<8);
                //printf ("Write VDP Address setup to VRAM write. address: %04XH\n",vdp_9918a_last_vram_position);
            }

            if ( (vdp_9918a_last_command_status_bytes[1] &  (128+64)) == 0 ) {
                

                vdp_9918a_last_vram_position=(vdp_9918a_last_command_status_bytes[0]) | ((vdp_9918a_last_command_status_bytes[1] & 63)<<8);
                //printf ("Write VDP Address setup to VRAM read. address: %04XH\n",vdp_9918a_last_vram_position);
            }            

            if ( (vdp_9918a_last_command_status_bytes[1] &  (128+64)) == 128 ) {
                //printf ("Write VDP Register setup.\n");

                //vdp_9918a_last_vram_position=(vdp_9918a_last_command_status_bytes[1] & 63) | (vdp_9918a_last_command_status_bytes[0]<<6);
                z80_byte vdp_register=vdp_9918a_last_command_status_bytes[1] & 7; //TODO: cuantos registros?


                vdp_9918a_registers[vdp_register]=vdp_9918a_last_command_status_bytes[0];

                //Cambio color o bits de modo, actualizar border
                if (vdp_register==0 || vdp_register==1 || vdp_register==7) {
                    modificado_border.v=1;
                    //printf ("modificado border: %d\n",vdp_9918a_registers[7] &15);
                }

                //printf ("Write VDP Register register: %02XH value %02XH\n",vdp_register,vdp_9918a_last_command_status_bytes[0]);

	//z80_byte video_mode_m3=(vdp_9918a_registers[0]>>1)&1;

	//z80_byte video_mode_m12=(vdp_9918a_registers[1]>>2)&(2+4);

	//z80_byte video_mode=video_mode_m12 | video_mode_m3;

	//printf ("video_mode: %d\n",video_mode);  

            }            
        break;
    }


}

/*
  M1 M2 M3 Screen format
  0  0  0  Half text  32x24             (Mode 0 - Graphic 1)
  1  0  0  Text       40x24             (Mode 1 - Text)
  0  0  1  Hi resolution 256x192        (Mode 2 - Graphic 2)
  0  1  0  Multicolour  4x4pix blocks   (Mode 3 - Multicolor)
*/

//                              01234567890123456789012345678901
const char *s_msx_video_mode_0="0 - Text 40x24";
const char *s_msx_video_mode_1="1 - Text 32x24";
const char *s_msx_video_mode_2="2 - Graphic 256x192";
const char *s_msx_video_mode_3="3 - Graphic 64x48";
                                               

z80_byte vdp_9918a_get_video_mode(void)
{

	z80_byte video_mode_m3=(vdp_9918a_registers[0]>>1)&1;

	z80_byte video_mode_m12=(vdp_9918a_registers[1]>>2)&(2+4);

	z80_byte video_mode=video_mode_m12 | video_mode_m3;


    return video_mode;
}


z80_int vdp_9918a_get_pattern_name_table(void)
{
    return (vdp_9918a_registers[2]&15) * 0x400; 
}



char *get_vdp_9918_string_video_mode(void) 
{


	//Por defecto
	const char *string_mode=s_msx_video_mode_0;


	z80_byte video_mode=vdp_9918a_get_video_mode();

	
	switch(video_mode) {

		case 0:
            string_mode=s_msx_video_mode_1;
        break;


		case 1:

			string_mode=s_msx_video_mode_2;
		break;


		case 2:
			string_mode=s_msx_video_mode_3;
		break;
    }

    return (char *)string_mode;

}

//Funciones que se usan en el tile navigator
int vdp_9918a_get_tile_width(void)
{
    z80_byte video_mode=vdp_9918a_get_video_mode();

    //por defecto
    int width=40;

	switch(video_mode) {

		case 0:
        case 1:
        case 2:
        //Incluso en 64x48, la definicion del tile es 32x24

            width=32;
		break;


    }

    return width;   

    
}

z80_byte vdp_9918a_get_border_color(void)
{

    return vdp_9918a_registers[7] & 15;
}

int vdp_9918a_get_tile_heigth(void)
{
    z80_byte video_mode=vdp_9918a_get_video_mode();

    //por defecto
    int heigth=24;

    return heigth;

    //Incluso en 64x48, la definicion del tile es 32x24

/*
	switch(video_mode) {


		case 2:
			heigth=48;
		break;
    }

    return heigth;   
    */

    
}

z80_int vdp_9918a_get_sprite_attribute_table(void)
{

    z80_int sprite_attribute_table=(vdp_9918a_registers[5]) * 0x80;

    //En boundary de 128
    sprite_attribute_table &=(65535-128);

    return sprite_attribute_table;
}


z80_byte vdp_9918a_read_vram_byte(z80_byte *vram,z80_int address)
{
    //Siempre leer limitando a 16 kb
    return vram[address & 16383];
}


void vdp_9918a_render_ula_no_rainbow(z80_byte *vram)
{


	z80_byte video_mode=vdp_9918a_get_video_mode();

	//printf ("video_mode: %d\n",video_mode);


	int x,y,bit; 
	z80_int direccion_name_table;
	z80_byte byte_leido;
    z80_byte byte_color;
	int color=0;
	
	//int zx,zy;

	z80_byte ink,paper;


	z80_int pattern_base_address; //=2048; //TODO: Puesto a pelo
	z80_int pattern_name_table; //=0; //TODO: puesto a pelo

	pattern_name_table=vdp_9918a_get_pattern_name_table(); //(vdp_9918a_registers[2]&15) * 0x400; 



	pattern_base_address=(vdp_9918a_registers[4]&7) * 0x800; 


	z80_int pattern_color_table=(vdp_9918a_registers[3]) * 0x40;

    //z80_int sprite_attribute_table=(vdp_9918a_registers[5]) * 0x80;

     

	//z80_byte *screen=get_base_mem_pantalla();



	int chars_in_line;
	int char_width;

	switch(video_mode) {

		case 4:
		case 0:
		//"screen 0": Text, characters of 6 x 8	40 x 24 characters
		//video_mode: 4		

	

		//pattern_base_address=0; //TODO: Puesto a pelo		
		//"screen 1": Text, characters of 8 x 8	, 32 x 24 characters
		//video_mode: 0	



		if (video_mode==4) {
			chars_in_line=40;
			char_width=6;

			//En modo texto 40x24, color tinta y papel fijos

			ink=(vdp_9918a_registers[7]>>4)&15;
			paper=(vdp_9918a_registers[7])&15;			
		}

		else {
			chars_in_line=32;
			char_width=8;
		}


		direccion_name_table=pattern_name_table;  

        for (y=0;y<24;y++) {
			for (x=0;x<chars_in_line;x++) {  
       
            		
				z80_byte caracter=vdp_9918a_read_vram_byte(vram,direccion_name_table);
                
				if (video_mode==0) {
					int posicion_color=caracter/8;

					z80_byte byte_color=vdp_9918a_read_vram_byte(vram,pattern_color_table+posicion_color);

					ink=(byte_color >> 4) & 15;
					paper=(byte_color ) & 15;
				}


				int scanline;

				z80_int pattern_address=pattern_base_address+caracter*8;

				for (scanline=0;scanline<8;scanline++) {

					byte_leido=vdp_9918a_read_vram_byte(vram,pattern_address++);
	                       

                    for (bit=0;bit<char_width;bit++) {

						int fila=(x*char_width+bit)/8;
						
						
						//Ver en casos en que puede que haya menu activo y hay que hacer overlay
						if (scr_ver_si_refrescar_por_menu_activo(fila,y)) {
							color= ( byte_leido & 128 ? ink : paper );
							scr_putpixel_zoom(x*char_width+bit,y*8+scanline,VDP_9918_INDEX_FIRST_COLOR+color);
						}

						byte_leido=byte_leido<<1;
        	        }
				}


				direccion_name_table++;

			}
			

   		 }

		break;

		case 2:
			//Screen 3. multicolor mode. 64x48
			//video_mode: 2


			direccion_name_table=pattern_name_table;  

			for (y=0;y<24;y++) {
				for (x=0;x<32;x++) {  
		
							
					z80_byte caracter=vdp_9918a_read_vram_byte(vram,direccion_name_table++);
					
								
					int incremento_byte=(y&3)*2;


					pattern_base_address &=(65536-1023); //Cae offsets de 1kb

					//printf ("pattern_address: %d\n",pattern_base_address);

					z80_int pattern_address=pattern_base_address+caracter*8+incremento_byte;

					int row;
					for (row=0;row<2;row++) {

						byte_leido=vdp_9918a_read_vram_byte(vram,pattern_address++);
						
						int col;
						for (col=0;col<2;col++) {
											
							//Ver en casos en que puede que haya menu activo y hay que hacer overlay
							if (scr_ver_si_refrescar_por_menu_activo(x,y)) {

								//Primera columna usa color en parte parte alta y luego baja
								color=(byte_leido>>4)&15;

								byte_leido=byte_leido << 4;

								
								int subpixel_x,subpixel_y;

								int xfinal=x*8+col*4;
								int yfinal=y*8+row*4;							

								for (subpixel_y=0;subpixel_y<4;subpixel_y++) {
									for (subpixel_x=0;subpixel_x<4;subpixel_x++) {
								
										scr_putpixel_zoom(xfinal+subpixel_x,  yfinal+subpixel_y,  VDP_9918_INDEX_FIRST_COLOR+color);
									}

								}
								
							}
						
						}

					}

				}

			}

		break;	        


		//Screen 2. high-res mode, 256x192
		//video_mode: 1
		case 1:
        default:

			chars_in_line=32;
			char_width=8;

            //printf ("pattern base address before mask: %d\n",pattern_base_address);

            //printf ("pattern color table before mask:  %d\n",pattern_color_table);            


			pattern_base_address &=8192; //Cae en offset 0 o 8192
          
			pattern_color_table &=8192; //Cae en offset 0 o 8192


            //printf ("pattern base address after mask: %d\n",pattern_base_address);

            //printf ("pattern color table after mask:  %d\n",pattern_color_table);

			direccion_name_table=pattern_name_table;  

			for (y=0;y<24;y++) {

				int tercio=y/8;

				for (x=0;x<chars_in_line;x++) {  
					
					
					z80_byte caracter=vdp_9918a_read_vram_byte(vram,direccion_name_table);
					

					int scanline;

					z80_int pattern_address=(caracter*8+2048*tercio) ;
					pattern_address +=pattern_base_address;
					
					


					z80_int color_address=(caracter*8+2048*tercio) ;
					color_address +=pattern_color_table;

	
			

					for (scanline=0;scanline<8;scanline++) {

						byte_leido=vdp_9918a_read_vram_byte(vram,pattern_address++);

						byte_color=vdp_9918a_read_vram_byte(vram,color_address++);


						ink=(byte_color>>4) &15;
						paper=byte_color &15;

							
						for (bit=0;bit<char_width;bit++) {

							int fila=(x*char_width+bit)/8;

													
							//Ver en casos en que puede que haya menu activo y hay que hacer overlay
							if (scr_ver_si_refrescar_por_menu_activo(fila,y)) {
								color= ( byte_leido & 128 ? ink : paper );
								scr_putpixel_zoom(x*char_width+bit,y*8+scanline,VDP_9918_INDEX_FIRST_COLOR+color);
							}

							byte_leido=byte_leido<<1;
						}
					}

						
					direccion_name_table++;

				}
		   }

		break;


	




	}    
}