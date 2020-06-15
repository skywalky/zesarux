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

#include "msx.h"
#include "vdp_9918a.h"
#include "cpu.h"
#include "debug.h"
#include "ay38912.h"
#include "tape.h"
#include "screen.h"

z80_byte *msx_vram_memory=NULL;

z80_byte msx_ppi_register_a;
//bit 0-1:slot segmento 0 (0000h-3fffh)
//bit 2-3:slot segmento 1 (4000h-7fffh)
//bit 4-5:slot segmento 2 (8000h-bfffh)
//bit 6-7:slot segmento 3 (C000h-ffffh)


z80_byte msx_ppi_register_b;
z80_byte msx_ppi_register_c;


//Aunque solo son 10 filas, metemos array de 16 pues es el maximo valor de indice seleccionable por el PPI
z80_byte msx_keyboard_table[16]={
255,255,255,255,255,255,255,255,
255,255,255,255,255,255,255,255
};


//slots asignados, y sus 4 segmentos
//tipos: rom, ram, vacio
int msx_memory_slots[4][4];



//Forzar desde menu a desactivar capas 
z80_bit msx_force_disable_layer_ula={0};
z80_bit msx_force_disable_layer_sprites={0};
z80_bit msx_force_disable_layer_border={0};


//Forzar a dibujar capa con color fijo, para debug
z80_bit msx_reveal_layer_ula={0};
z80_bit msx_reveal_layer_sprites={0};


//Retorna direccion de memoria donde esta mapeada la ram y su tipo
z80_byte *msx_return_segment_address(z80_int direccion,int *tipo)
{
    int segmento=direccion/16384;

    
    z80_byte slot=msx_ppi_register_a;

    int rotar=segmento*2;

    if (segmento>0) {
        slot=slot>>rotar;
    }

    slot &=3;

     

    *tipo=msx_memory_slots[slot][segmento];

    int offset=((slot*4)+segmento)*16384;

    //if (slot>0) printf ("direccion %6d segmento %d slot %d offset %d\n",direccion,segmento,slot,offset);

    return &memoria_spectrum[offset+(direccion&16383)];



}


void msx_init_memory_tables(void)
{

    //inicio con todos los slots vacios
    int slot,segment;
    for (slot=0;slot<4;slot++) {
        for (segment=0;segment<4;segment++) {
            msx_memory_slots[slot][segment]=MSX_SLOT_MEMORY_TYPE_EMPTY;
        }

    }


    //De momento meter 32 kb rom, 32 kb ram
    msx_memory_slots[0][0]=MSX_SLOT_MEMORY_TYPE_ROM;
    msx_memory_slots[0][1]=MSX_SLOT_MEMORY_TYPE_ROM;


    msx_memory_slots[2][0]=MSX_SLOT_MEMORY_TYPE_RAM;
    msx_memory_slots[2][1]=MSX_SLOT_MEMORY_TYPE_RAM;
    msx_memory_slots[2][2]=MSX_SLOT_MEMORY_TYPE_RAM;
    msx_memory_slots[2][3]=MSX_SLOT_MEMORY_TYPE_RAM;



 


}


void msx_reset(void)
{
    //Mapear inicialmente todo a slot 0
    msx_ppi_register_a=0;



}

void msx_out_port_vdp_data(z80_byte value)
{
    vdp_9918a_out_vram_data(msx_vram_memory,value);
}


z80_byte msx_in_port_vdp_data(void)
{
    return vdp_9918a_in_vram_data(msx_vram_memory);
}



z80_byte msx_in_port_vdp_status(void)
{
    return vdp_9918a_in_vdp_status();
}

void msx_out_port_vdp_command_status(z80_byte value)
{
    vdp_9918a_out_command_status(msx_vram_memory,value);
}


void msx_out_port_ppi(z80_byte puerto_l,z80_byte value)
{
    //printf ("Out port ppi. Port %02XH value %02XH\n",puerto_l,value);

    int slot,segment;

    switch (puerto_l) {
        case 0xA8:
            msx_ppi_register_a=value;
            printf ("Out port ppi. Port %02XH value %02XH\n",puerto_l,value);

    //temporal mostrar mapeos
    

    for (slot=0;slot<4;slot++) {
        for (segment=0;segment<4;segment++) {
            printf ("%d %d : %d\n",slot,segment,msx_memory_slots[slot][segment]);
        }
    }

        break;

        case 0xA9:
            msx_ppi_register_b=value;
        break;


        case 0xAA:
            msx_ppi_register_c=value;
        break;
    }
}

z80_byte msx_in_port_ppi(z80_byte puerto_l)
{
    //printf ("In port ppi. Port %02XH\n",puerto_l);

    z80_byte valor;

    switch (puerto_l) {

        case 0xA8:
            return msx_ppi_register_a;
        break;        
 
        case 0xA9:
            //Leer registro B (filas teclado)
            //que fila? msx_ppi_register_c

            //si estamos en el menu, no devolver tecla
            if (zxvision_key_not_sent_emulated_mach() ) return 255;
            
            return msx_keyboard_table[msx_ppi_register_c & 0x0F];

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
            return msx_ppi_register_c;
        break;

    }

    return 255; //temp
}


void msx_out_port_psg(z80_byte puerto_l,z80_byte value)
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


void msx_alloc_vram_memory(void)
{
    if (msx_vram_memory==NULL) {
        msx_vram_memory=malloc(16384);
        if (msx_vram_memory==NULL) cpu_panic("Cannot allocate memory for msx vram");
    }
}


z80_byte msx_read_vram_byte(z80_int address)
{
    //Siempre leer limitando a 16 kb
    return msx_vram_memory[address & 16383];
}



void msx_insert_rom_cartridge(char *filename)
{

	debug_printf(VERBOSE_INFO,"Inserting msx rom cartridge %s",filename);

    long tamanyo_archivo=get_file_size(filename);

    if (tamanyo_archivo!=16384 && tamanyo_archivo!=32768) {
        debug_printf(VERBOSE_ERR,"Only 16k and 32k rom cartridge are allowed");
        return;
    }

        FILE *ptr_cartridge;
        ptr_cartridge=fopen(filename,"rb");

        if (!ptr_cartridge) {
		debug_printf (VERBOSE_ERR,"Unable to open cartridge file %s",filename);
                return;
        }



	//Leer cada bloque de 16 kb si conviene

	int bloque;

    int salir=0;

    int bloques_totales=0;

	for (bloque=0;bloque<2 && !salir;bloque++) {
        /*
        The ROM Header

A ROM needs a header to be auto-executed by the system when the MSX is initialized.

After finding the RAM and initializing the system variables, the MSX looks for the ROM headers in all the slots 
on the memory pages 4000h-7FFFh and 8000h-FFFh. The search is done in ascending order. 
When a primary Slot is expanded, the search is done in the corresponding secondary Slots before going to the next Primary Slot.
When the system finds a header, it selects the ROM slot only on the memory page corresponding to the address specified in INIT then, runs the program in ROM at the same address. (In short, it makes an inter-slot call.)

        */
        int offset=65536+bloque*16384;
		int leidos=fread(&memoria_spectrum[offset+16384],1,16384,ptr_cartridge);
        if (leidos==16384) {
            msx_memory_slots[1][1+bloque]=MSX_SLOT_MEMORY_TYPE_ROM;
            printf ("loaded 16kb bytes of rom at slot 1 block %d\n",bloque);

            bloques_totales++;


        }
        else {
            salir=1;
        }

	}

    if (bloques_totales==1) {
            //Copiar en los otros 3 segmentos

            memcpy(&memoria_spectrum[65536],&memoria_spectrum[65536+16384],16384);
            memcpy(&memoria_spectrum[65536+32768],&memoria_spectrum[65536+16384],16384);
            memcpy(&memoria_spectrum[65536+49152],&memoria_spectrum[65536+16384],16384);

            msx_memory_slots[1][0]=MSX_SLOT_MEMORY_TYPE_ROM;
            msx_memory_slots[1][2]=MSX_SLOT_MEMORY_TYPE_ROM;
            msx_memory_slots[1][3]=MSX_SLOT_MEMORY_TYPE_ROM;
    }


    
    int i;


        fclose(ptr_cartridge);


        if (noautoload.v==0) {
                debug_printf (VERBOSE_INFO,"Reset cpu due to autoload");
                reset_cpu();
        }


}


void msx_empty_romcartridge_space(void)
{

    int i;
    for (i=0;i<4;i++) {
        msx_memory_slots[1][i]=MSX_SLOT_MEMORY_TYPE_EMPTY;
    }

}





void msx_refresca_border(void)
{

    unsigned int color=vdp_9918a_registers[7] & 15;




        int x,y;




	//Top border cambia en spectrum y zx8081 y ace
	int topborder=TOP_BORDER;
	

	//color +=spectrum_palette_offset;


        //parte superior
        for (y=0;y<topborder;y++) {
                for (x=0;x<ANCHO_PANTALLA*zoom_x+LEFT_BORDER*2;x++) {
                                scr_putpixel(x,y,VDP_9918_INDEX_FIRST_COLOR+color);
                }
        }

        //parte inferior
        for (y=0;y<BOTTOM_BORDER;y++) {
                for (x=0;x<ANCHO_PANTALLA*zoom_x+LEFT_BORDER*2;x++) {
                                scr_putpixel(x,topborder+y+ALTO_PANTALLA*zoom_y,VDP_9918_INDEX_FIRST_COLOR+color);


                }
        }


        //laterales
        for (y=0;y<ALTO_PANTALLA*zoom_y;y++) {
                for (x=0;x<LEFT_BORDER;x++) {
                        scr_putpixel(x,topborder+y,VDP_9918_INDEX_FIRST_COLOR+color);
                        scr_putpixel(LEFT_BORDER+ANCHO_PANTALLA*zoom_x+x,topborder+y,VDP_9918_INDEX_FIRST_COLOR+color);
                }

        }




}




void msx_render_sprites_no_rainbow(void)
{
 


    z80_byte video_mode=vdp_9918a_get_video_mode();




    //En modos 1 y 2 permitimos sprites
    if (video_mode!=1 && video_mode!=2) return;



    z80_int sprite_pattern_table=(vdp_9918a_registers[6]) * 0x800;
    
    z80_byte byte_leido;

        
        int sprite_size=(vdp_9918a_registers[1] & 64 ? 16 : 8);
        int sprite_double=(vdp_9918a_registers[1] & 128 ? 1 : 0);

        //printf ("Sprite size: %d double: %d\n",sprite_size,sprite_double);

        int bytes_per_sprite;
        int bytes_per_line;

        if (sprite_size==8) {
            bytes_per_sprite=8;
            bytes_per_line=1;
        }

        else {
            bytes_per_sprite=32;
            bytes_per_line=2;
        }


        //TODO: si coordenada Y=208, fin tabla sprites
        //    z80_int sprite_attribute_table=(vdp_9918a_registers[5]) * 0x80;

        //z80_int sprite_pattern_table=(vdp_9918a_registers[6]) * 0x800;

        int sprite;
        int salir=0;

        //En boundary de 128
        //sprite_attribute_table &=(65535-128);

        z80_int sprite_attribute_table=vdp_9918a_get_sprite_attribute_table();

        //Empezar por la del final
        //Ver si hay alguno con coordenada 208 que indica final

        int primer_sprite_final=VDP_9918A_MAX_SPRITES-1;

        int offset_sprite=sprite_attribute_table;

        int i;
        for (primer_sprite_final=0;primer_sprite_final<32 && !salir;primer_sprite_final++) {
            int offset_sprite=sprite_attribute_table+primer_sprite_final*4;

            z80_byte vert_pos=msx_read_vram_byte(sprite_attribute_table);
            if (vert_pos==208) salir=1;

        }

        //Siempre estara al siguiente
        primer_sprite_final--;

        sprite_attribute_table +=(primer_sprite_final*4);

        //Empezar desde final hacia principio

        for (sprite=primer_sprite_final;sprite>=0;sprite--) {
            z80_byte vert_pos=msx_read_vram_byte(sprite_attribute_table);
            z80_byte horiz_pos=msx_read_vram_byte(sprite_attribute_table+1);
            z80_byte sprite_name=msx_read_vram_byte(sprite_attribute_table+2);
            z80_byte attr_color_etc=msx_read_vram_byte(sprite_attribute_table+3);

/*
  0: Y-pos, Vertical position (FFh is topmost, 00h is second line, etc.)
  1: X-pos, Horizontal position (00h is leftmost)
  2: Pattern number
  3: Attributes. b0-3:Color, b4-6:unused, b7:EC (Early Clock)            
*/

            vert_pos++;

            //Siguiente sprite
            sprite_attribute_table -=4;

            printf ("sprite number: %d X: %d Y: %d Name: %d color_etc: %d\n",sprite,horiz_pos,vert_pos,sprite_name,attr_color_etc);

       
                

                //Si coord valida
                if (vert_pos<192) {
                    //int offset_pattern_table=sprite_name*bytes_per_sprite+sprite_pattern_table;
                      int offset_pattern_table=sprite_name*8+sprite_pattern_table;
                    z80_byte color=attr_color_etc & 15;

                    int x,y,byte_linea;

                    if (sprite_size==16) {
                        int quad_x,quad_y;

                        for (quad_x=0;quad_x<2;quad_x++) {
                            for (quad_y=0;quad_y<2;quad_y++) {
                                for (y=0;y<8;y++) {
                                
                                    byte_leido=msx_read_vram_byte(offset_pattern_table++);
                                    for (x=0;x<8;x++) {

                                            int pos_x_final;
                                            int pos_y_final;

                                        pos_x_final=horiz_pos+(quad_x*8)+x;
                                        pos_y_final=vert_pos+(quad_y*8)+y;
                                        
                                        //Si dentro de limites
                                        if (pos_x_final<255 && pos_y_final<192) {

                                            //Si bit a 1
                                            if (byte_leido & 128) {
                                                //Y si ese color no es transparente
                                                if (color!=0) {
                                                    //printf ("putpixel sprite x %d y %d\n",pos_x_final,pos_y_final);
                                                    scr_putpixel_zoom(pos_x_final,  pos_y_final,  VDP_9918_INDEX_FIRST_COLOR+color);
                                                }
                                            }

                                            byte_leido = byte_leido << 1;
                                        }
                                    }
                                }
                            }
                        }                        
                    }

                    else {

                        for (y=0;y<8;y++) {

                                byte_leido=msx_read_vram_byte(offset_pattern_table++);
                                for (x=0;x<8;x++) {

                                        int pos_x_final;
                                        int pos_y_final;

                                    pos_x_final=horiz_pos+x;
                                    pos_y_final=vert_pos+y;
                                    

                                    if (byte_leido & 128) {
                                        //printf ("putpixel sprite x %d y %d\n",pos_x_final,pos_y_final);
                                        scr_putpixel_zoom(pos_x_final,  pos_y_final,  VDP_9918_INDEX_FIRST_COLOR+color);
                                    }

                                    byte_leido = byte_leido << 1;
                                }
                            
                        }
                    }

                }
            

        }   
}

void msx_render_ula_no_rainbow(void)
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

	pattern_name_table=(vdp_9918a_registers[2]&15) * 0x400; 



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
       
            		
				z80_byte caracter=msx_read_vram_byte(direccion_name_table);
                
				if (video_mode==0) {
					int posicion_color=caracter/8;

					z80_byte byte_color=msx_read_vram_byte(pattern_color_table+posicion_color);

					ink=(byte_color >> 4) & 15;
					paper=(byte_color ) & 15;
				}


				int scanline;

				z80_int pattern_address=pattern_base_address+caracter*8;

				for (scanline=0;scanline<8;scanline++) {

					byte_leido=msx_read_vram_byte(pattern_address++);
	                       

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


		//Screen 2. high-res mode, 256x192
		//video_mode: 1
		case 1:

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
					
					
					z80_byte caracter=msx_read_vram_byte(direccion_name_table);
					

					int scanline;

					z80_int pattern_address=(caracter*8+2048*tercio) ;
					pattern_address +=pattern_base_address;
					
					


					z80_int color_address=(caracter*8+2048*tercio) ;
					color_address +=pattern_color_table;

	
			

					for (scanline=0;scanline<8;scanline++) {

						byte_leido=msx_read_vram_byte(pattern_address++);

						byte_color=msx_read_vram_byte(color_address++);


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


		case 2:
			//Screen 3. multicolor mode. 64x48
			//video_mode: 2


			direccion_name_table=pattern_name_table;  

			for (y=0;y<24;y++) {
				for (x=0;x<32;x++) {  
		
							
					z80_byte caracter=msx_read_vram_byte(direccion_name_table++);
					
								
					int incremento_byte=(y&3)*2;


					pattern_base_address &=(65536-1023); //Cae offsets de 1kb

					//printf ("pattern_address: %d\n",pattern_base_address);

					z80_int pattern_address=pattern_base_address+caracter*8+incremento_byte;

					int row;
					for (row=0;row<2;row++) {

						byte_leido=msx_read_vram_byte(pattern_address++);
						
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




	}    
}

//Refresco pantalla sin rainbow
void scr_refresca_pantalla_y_border_msx(void)
{

    if (border_enabled.v && msx_force_disable_layer_border.v==0) {
            //ver si hay que refrescar border
            if (modificado_border.v)
            {
                    msx_refresca_border();
                    modificado_border.v=0;
            }

    }


    if (msx_force_disable_layer_ula.v==0) {
        msx_render_ula_no_rainbow();
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



    if (msx_force_disable_layer_sprites.v==0) {
        msx_render_sprites_no_rainbow();
    }
        
        


}