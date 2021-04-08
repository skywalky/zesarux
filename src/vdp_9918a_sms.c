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


#include "vdp_9918a_sms.h"
#include "vdp_9918a.h"
#include "cpu.h"
#include "debug.h"
#include "screen.h"
#include "settings.h"


//Rutinas del VDP de Sega Master System que extiende el vdp 9918a
//La mayoría de cosas de aquí solo se aplican cuando se activa el modo 4 de la SMS


//TODO mejorar nombre de esto
int sms_writing_cram=0;

int index_sms_escritura_cram=0;

z80_byte vdp_9918a_sms_cram[VDP_9918A_SMS_MODE4_MAPPED_PALETTE_COLOURS];


void vdp_9918a_sms_reset(void)
{
    int i;
    //Y resetear tabla de colores de sms
    //16+16 colores (tiles + sprites)
    
    for (i=0;i<2;i++) {

        vdp_9918a_sms_cram[0+i*16]=0x00;	
        vdp_9918a_sms_cram[1+i*16]=0x00;	
        vdp_9918a_sms_cram[2+i*16]=0x08;	
        vdp_9918a_sms_cram[3+i*16]=0x0C;	
        vdp_9918a_sms_cram[4+i*16]=0x10;	
        vdp_9918a_sms_cram[5+i*16]=0x30;	
        vdp_9918a_sms_cram[6+i*16]=0x01;	
        vdp_9918a_sms_cram[7+i*16]=0x3C;	
        vdp_9918a_sms_cram[8+i*16]=0x02;	
        vdp_9918a_sms_cram[9+i*16]=0x03;	
        vdp_9918a_sms_cram[10+i*16]=0x05;	
        vdp_9918a_sms_cram[11+i*16]=0x0f;	
        vdp_9918a_sms_cram[12+i*16]=0x04;	
        vdp_9918a_sms_cram[13+i*16]=0x33;	
        vdp_9918a_sms_cram[14+i*16]=0x15;	
        vdp_9918a_sms_cram[15+i*16]=0x3f;	

    }
}

int vdp_9918a_si_sms_video_mode4(void)
{
    if (MACHINE_IS_SMS && (vdp_9918a_registers[0] & 4)) return 1;

    else return 0;    
}

z80_int vdp_9918a_get_sprite_pattern_table_sms_mode4(void)
{
    z80_int sprite_pattern_table=(vdp_9918a_registers[6] & 4) * 0x800;

    return sprite_pattern_table;
}

z80_int vdp_9918a_sms_get_pattern_name_table(void)
{

    return (vdp_9918a_registers[2]&14) * 0x400; 

}

z80_byte vdp_9918a_sms_get_scroll_horizontal(void)
{
    return vdp_9918a_registers[8];
}

z80_byte vdp_9918a_sms_get_scroll_vertical(void)
{
    return vdp_9918a_registers[9];
}

void vdp_9918a_render_ula_no_rainbow_sms(z80_byte *vram)
{


	z80_byte video_mode=vdp_9918a_get_video_mode();



	//printf ("video_mode: %d\n",video_mode);


	int x,y,bit; 
	z80_int direccion_name_table;
	z80_byte byte_leido;
    z80_byte byte_color;
	int color=0;
	


	z80_byte ink,paper;


    z80_int pattern_color_table;
	z80_int pattern_base_address; 
	z80_int pattern_name_table; 

	pattern_name_table=vdp_9918a_get_pattern_name_table(); 


	pattern_color_table=vdp_9918a_get_pattern_color_table();


    pattern_base_address=vdp_9918a_get_pattern_base_address();



	int chars_in_line;
	int char_width;

	
		//Modo 4 SMS. high-res mode, 256x192
		//video_mode: 1
		

            //printf("Mode 4 sms\n");

			chars_in_line=32;
			char_width=8;

            z80_byte byte_leido1,byte_leido2,byte_leido3,byte_leido4;


            //TODO. esto siempre asi??
            pattern_base_address=0;


            pattern_name_table=vdp_9918a_get_pattern_name_table();


			direccion_name_table=pattern_name_table;  

            //scroll y
            z80_byte scroll_y=vdp_9918a_sms_get_scroll_vertical();

        
            //fila
            /*
            Register $09 can be divided into two parts, the upper five bits are the
starting row, and the lower three bits are the fine scroll value.
.
            */

            z80_byte fila_scroll_y=((scroll_y>>3)&31);

            z80_byte scroll_y_sublinea=scroll_y&7;

            //TODO scroll a pixel en horiz y vertical

            //TODO: la gestion de scroll fino en vertical y horizontal en modo rainbow, sera
            //algo distinto de este no rainbow (o deberia ser)
      
            //1 fila mas si hay scroll vertical
            int total_filas=24;

            if (scroll_y_sublinea) total_filas++;

			for (y=0;y<total_filas;y++) {

                //Maximo 28 en Y
                z80_byte final_y=(y+fila_scroll_y) % 28;     

                    //scroll x
                    z80_byte scroll_x=vdp_9918a_sms_get_scroll_horizontal();

                    /*
                     If bit #6 of VDP register $00 is set, horizontal scrolling will be fixed
 at zero for scanlines zero through 15. This is commonly used to create
 a fixed status bar at the top of the screen for horizontally scrolling
 games.

 Register $00 - Mode Control No. 1

 D7 - 1= Disable vertical scrolling for columns 24-31
 D6 - 1= Disable horizontal scrolling for rows 0-1  
 D5 - 1= Mask column 0 with overscan color from register #7
 D4 - (IE1) 1= Line interrupt enable
 D3 - (EC) 1= Shift sprites left by 8 pixels
 D2 - (M4) 1= Use Mode 4, 0= Use TMS9918 modes (selected with M1, M2, M3)
 D1 - (M2) Must be 1 for M1/M3 to change screen height in Mode 4.
      Otherwise has no effect.
 D0 - 1= No sync, display is monochrome, 0= Normal display

                    */

                    //Las 2 primeras lineas no tendran scroll si bit D6
                    //Esto lo usa juego Astro Flash
                   if (vdp_9918a_registers[0] & 64 && y<2) scroll_x=0; 

                    z80_byte columna_scroll_x;
                   z80_byte scroll_x_fino;
                   
                   if (scroll_x==0) {
                       scroll_x_fino=0;
                       columna_scroll_x=0;
                   }
                   
                   else {
                       scroll_x_fino=(255-scroll_x) & 7;
                       columna_scroll_x=32-((scroll_x>>3)&31);
                   }

                
                    //columna
                    /*
                     The starting column value gives the first column in the name table to use,
 calculated by subtracting it from the value 32. So if the starting column
 value was $1D, the difference of it from 32 would be $02, hence the first
 column drawn is number 2 from the name table.
                    */

                    

                

            //TODO: la gestion de scroll fino en vertical y horizontal en modo rainbow, sera
            //algo distinto de este no rainbow (o deberia ser)
      
            //1 columna mas si hay scroll horizontal


            //TODO algo falla en la columna final del sonic cuando hay scroll

    
            int total_columnas=32;

            if (scroll_x_fino) total_columnas++;

                //printf("%d\n",total_columnas);

				for (x=0;x<total_columnas;x++) {  
					
                    

                    z80_byte final_x;

                    //prueba para corregir columna final en sonic
                    //if (x==32) final_x=0;
                    //else final_x=(x+columna_scroll_x) % 32;        

                    final_x=(x+columna_scroll_x) % 32;

                    direccion_name_table=pattern_name_table+final_x*2+final_y*64;
					
					z80_int pattern_word=vdp_9918a_read_vram_byte(vram,direccion_name_table)+256*vdp_9918a_read_vram_byte(vram,direccion_name_table+1);

                    
                    /*
                    MSB          LSB
 ---pcvhn nnnnnnnn

 - = Unused. Some games use these bits as flags for collision and damage
     zones. (such as Wonderboy in Monster Land, Zillion 2)
 p = Priority flag. When set, sprites will be displayed underneath the
     background pattern in question.
 c = Palette select.
 v = Vertical flip flag.
 h = Horizontal flip flag.
 n = Pattern index, any one of 512 patterns in VRAM can be selected.
                    */

                    int mirror_x=(pattern_word & 0x0200);

                    int mirror_y=(pattern_word & 0x0400);

                    z80_int caracter=pattern_word & 511;

                    int palette_offset=(pattern_word & 0x0800 ? 16 : 0);
					

					int scanline;

					//z80_int pattern_address=(caracter*32+2048*tercio) ;
                    z80_int pattern_address=(caracter*32) ;
					pattern_address +=pattern_base_address;
					
					
                    //Si tiene mirror vertical, empezamos con la ultima linea del pattern
                    if (mirror_y) {
                        pattern_address +=(8*4)-4;
                    }
	
			

					for (scanline=0;scanline<8;scanline++) {

                        //TODO: no se si esta es la mejor manera de gestionar el scroll
                        int ydestino=y*8+scanline-scroll_y_sublinea;
                        //printf("%d\n",ydestino);

						byte_leido1=vdp_9918a_read_vram_byte(vram,pattern_address);
                        byte_leido2=vdp_9918a_read_vram_byte(vram,pattern_address+1);
                        byte_leido3=vdp_9918a_read_vram_byte(vram,pattern_address+2);
                        byte_leido4=vdp_9918a_read_vram_byte(vram,pattern_address+3);

                        if (mirror_y) {
                            pattern_address -=4;
                        }
                        else {
						    pattern_address +=4;
                        }

                        //No dibujar si y < 0. Esto sucede cuando se aplica scroll vertical

                        //Similar para mayor de 192 cuando hay scroll y hacemos 25 filas (parte de la ultima 25)
                        if (ydestino>=0 && ydestino<=191) {
							
						for (bit=0;bit<char_width;bit++) {

							//int fila=(x*char_width+bit)/8;

                            //TODO: no se si esta es la mejor manera de gestionar el scroll
                            int xdestino=x*char_width+bit-scroll_x_fino;

                            if (mirror_x) {
                                byte_color=((byte_leido1)&1) | ((byte_leido2<<1)&2) | ((byte_leido3<<2)&4) | ((byte_leido4<<3)&8);
                            }
                            else {

                                byte_color=((byte_leido1>>7)&1) | ((byte_leido2>>6)&2) | ((byte_leido3>>5)&4) | ((byte_leido4>>4)&8);
                        
                            }


                            color= byte_color;
                            int color_paleta=vdp_9918a_sms_cram[palette_offset+ color & 15];

                            //maximo 64 colores de paleta
                            color_paleta &=63;



                            
                            

                            //No dibujar si x < 0. Esto sucede cuando se aplica scroll horizontal
                            //Similar para mayor de 255 cuando hay scroll x  y  hacemos 33 filas (parte de la ultima 33)
                            if (xdestino>=0 && xdestino<=255) {

                                //Register $00 - Mode Control No. 1
                                //D5 - 1= Mask column 0 with overscan color from register #7
                                //Esto lo usa sonic. La primera columna es para usar para el scroll
                                if (xdestino<=7 && (vdp_9918a_registers[0] & 32)) {
                                    color_paleta=0; //TODO: que color? debe ser el del border
                                }  

                                scr_putpixel_zoom(xdestino,ydestino,SMS_INDEX_FIRST_COLOR+color_paleta);
                            }

                            if (mirror_x) {
                                byte_leido1=byte_leido1>>1;
                                byte_leido2=byte_leido2>>1;
                                byte_leido3=byte_leido3>>1;
                                byte_leido4=byte_leido4>>1;                                
                            }

                            else {
                                byte_leido1=byte_leido1<<1;
                                byte_leido2=byte_leido2<<1;
                                byte_leido3=byte_leido3<<1;
                                byte_leido4=byte_leido4<<1;
                            }

							
						}

                        }
                        
					}


				}

 
		   }

 
}

int vdp_9918a_sms_get_sprite_height(void)
{
    /*
     Register $01 - Mode Control No. 2

 D7 - No effect
 D6 - (BLK) 1= Display visible, 0= display blanked.
 D5 - (IE0) 1= Frame interrupt enable.
 D4 - (M1) Selects 224-line screen for Mode 4 if M2=1, else has no effect.
 D3 - (M3) Selects 240-line screen for Mode 4 if M2=1, else has no effect.
 D2 - No effect
 D1 - Sprites are 1=16x16,0=8x8 (TMS9918), Sprites are 1=8x16,0=8x8 (Mode 4)
 D0 - Sprite pixels are doubled in size.
    */


    return (vdp_9918a_registers[1] & 2 ? 16 : 8);



}


//Render sprites en modo 4 Sega Master System
void vdp_9918a_render_sprites_sms_video_mode4_no_rainbow(z80_byte *vram)
{

    z80_int sprite_pattern_table=vdp_9918a_get_sprite_pattern_table_sms_mode4();
    
    z80_byte byte_leido;

    z80_byte byte_leido1,byte_leido2,byte_leido3,byte_leido4;

        
    //int sprite_size=vdp_9918a_get_sprite_size();
    int sprite_double=vdp_9918a_get_sprite_double();



    //TODO temp
    //sprite_size=8;
    sprite_double=1;

/*
TODO
 Register $01 - Mode Control No. 2

 D7 - No effect
 D6 - (BLK) 1= Display visible, 0= display blanked.
 D5 - (IE0) 1= Frame interrupt enable.
 D4 - (M1) Selects 224-line screen for Mode 4 if M2=1, else has no effect.
 D3 - (M3) Selects 240-line screen for Mode 4 if M2=1, else has no effect.
 D2 - No effect
 D1 - Sprites are 1=16x16,0=8x8 (TMS9918), Sprites are 1=8x16,0=8x8 (Mode 4)
 D0 - Sprite pixels are doubled in size.
 */        

    //printf ("Sprite size: %d double: %d\n",sprite_size,sprite_double);
    //printf("Sprite size: 8x%d\n",(vdp_9918a_registers[1] & 2 ? 16 : 8));


    int sprite_height=vdp_9918a_sms_get_sprite_height();

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

    int primer_sprite_final; //=VDP_9918A_SMS_MODE4_MAX_SPRITES-1;


    // buscar ultimo sprite
    /*
        If the Y coordinate is set to $D0, then the sprite in question and all
remaining sprites of the 64 available will not be drawn. This only works
in the 192-line display mode, in the 224 and 240-line modes a Y coordinate
of $D0 has no special meaning.
    */
    
    for (primer_sprite_final=0;primer_sprite_final<VDP_9918A_SMS_MODE4_MAX_SPRITES && !salir;primer_sprite_final++) {
        int offset_sprite=sprite_attribute_table+primer_sprite_final;

        z80_byte vert_pos=vdp_9918a_read_vram_byte(vram,offset_sprite);
        if (vert_pos==208) salir=1;

    }
        

    //Siempre estara al siguiente
    primer_sprite_final--;



        //Empezar desde final hacia principio
/*
 Each sprite is defined in the sprite attribute table (SAT), a 256-byte
 table located in VRAM. The SAT has the following layout:

    00: yyyyyyyyyyyyyyyy
    10: yyyyyyyyyyyyyyyy
    20: yyyyyyyyyyyyyyyy
    30: yyyyyyyyyyyyyyyy
    40: ????????????????
    50: ????????????????
    60: ????????????????
    70: ????????????????
    80: xnxnxnxnxnxnxnxn
    90: xnxnxnxnxnxnxnxn
    A0: xnxnxnxnxnxnxnxn
    B0: xnxnxnxnxnxnxnxn
    C0: xnxnxnxnxnxnxnxn
    D0: xnxnxnxnxnxnxnxn
    E0: xnxnxnxnxnxnxnxn
    F0: xnxnxnxnxnxnxnxn

 y = Y coordinate + 1
 x = X coordinate
 n = Pattern index
 ? = Unused
       
*/        

    for (sprite=primer_sprite_final;sprite>=0;sprite--) {
        int vert_pos=vdp_9918a_read_vram_byte(vram,sprite_attribute_table+sprite);
        //printf("sprite %d pos %d\n",sprite,sprite_attribute_table+sprite);

        int horiz_pos=vdp_9918a_read_vram_byte(vram,sprite_attribute_table+0x80+sprite*2);
        z80_byte sprite_name=vdp_9918a_read_vram_byte(vram,sprite_attribute_table+0x80+sprite*2+1);


        //printf("Sprite %d Pattern %d X %d Y %d\n",sprite,sprite_name,horiz_pos,vert_pos);

        /*
        TODO
            The pattern index selects one of 256 patterns to use. Bit 2 of register #6
acts like a global bit 8 in addition to this value, allowing sprite patterns
to be taken from the first 256 or last 256 of the 512 available patterns.
        */



        vert_pos++; //255->coordenada 0
        if (vert_pos==256) vert_pos=0;

        //Entre 255 y 256-32-> son coordenadas negativas
        if (vert_pos>=256-32) {
            //printf ("sprite number: %d X: %d Y: %d Name: %d color_etc: %d\n",sprite,horiz_pos,vert_pos,sprite_name,attr_color_etc);                
            //printf ("Sprite Y negative: %d\n",vert_pos-256);
            vert_pos=vert_pos-256;

        }

        //Siguiente sprite. El precedente
        //sprite_attribute_table -=4;

        //Si early clock, x-=32

        /*if (attr_color_etc & 128) {
            //printf ("sprite number: %d X: %d Y: %d Name: %d color_etc: %d\n",sprite,horiz_pos,vert_pos,sprite_name,attr_color_etc);                
            horiz_pos -=32;
        }*/

        //printf ("sprite number: %d X: %d Y: %d Name: %d color_etc: %d\n",sprite,horiz_pos,vert_pos,sprite_name,attr_color_etc);

    
            

        //Si coord Y no esta en el borde inferior
        if (vert_pos<192) {
            //int offset_pattern_table=sprite_name*bytes_per_sprite+sprite_pattern_table;
                int offset_pattern_table=sprite_name*32+sprite_pattern_table;
            //z80_byte color=attr_color_etc & 15;

            int x,y;

            

            //Sprites de 8x16 y 8x8
            

                for (y=0;y<sprite_height;y++) {

                    byte_leido1=vdp_9918a_read_vram_byte(vram,offset_pattern_table++);
                    byte_leido2=vdp_9918a_read_vram_byte(vram,offset_pattern_table++);
                    byte_leido3=vdp_9918a_read_vram_byte(vram,offset_pattern_table++);
                    byte_leido4=vdp_9918a_read_vram_byte(vram,offset_pattern_table++);


                    for (x=0;x<8;x++) {

                        int pos_x_final;
                        int pos_y_final;

                        pos_x_final=horiz_pos+x*sprite_double;
                        pos_y_final=vert_pos+y*sprite_double;
                        
                        if (pos_x_final>=0 && pos_x_final<=255 && pos_y_final>=0 && pos_y_final<=191) {

                            //Si bit a 1
                            if (1) {
                                //Y si ese color no es transparente

                                //TODO 
                                //if (color!=0) {
                                if (1) {
                                    //printf ("putpixel sprite x %d y %d\n",pos_x_final,pos_y_final);

                                    z80_byte byte_color=((byte_leido1>>7)&1) | ((byte_leido2>>6)&2) | ((byte_leido3>>5)&4) | ((byte_leido4>>4)&8);



                                    //TODO: segunda paleta??  
                                    z80_byte color_sprite=vdp_9918a_sms_cram[16 + (byte_color & 15)];

                                    //maximo 64 colores de paleta
                                    color_sprite &=63;

                                    if (vdp_9918a_reveal_layer_sprites.v) {
                                        int posx=pos_x_final&1;
                                        int posy=pos_y_final&1;

                                        //0,0: 0
                                        //0,1: 1
                                        //1,0: 1
                                        //1,0: 0
                                        //Es un xor

                                        int si_blanco_negro=posx ^ posy;

                                        //Color 0 o 63 (negro / blanco)
                                        color_sprite=si_blanco_negro*(SMS_TOTAL_PALETTE_COLOURS-1);
                                    }             

                                    //TODO transparencia
                                    if (byte_color!=0) {          

                                        //if (x==0 && y==0) printf("Dibujando sprite %d\n",sprite);            
                                            
                                        scr_putpixel_zoom(pos_x_final,  pos_y_final,  SMS_INDEX_FIRST_COLOR+color_sprite);
                                        if (sprite_double==2) {
                                            scr_putpixel_zoom(pos_x_final+1,  pos_y_final,    SMS_INDEX_FIRST_COLOR+color_sprite);
                                            scr_putpixel_zoom(pos_x_final,    pos_y_final+1,  SMS_INDEX_FIRST_COLOR+color_sprite);
                                            scr_putpixel_zoom(pos_x_final+1,  pos_y_final+1,  SMS_INDEX_FIRST_COLOR+color_sprite);
                                        }                   

                                    }                             
                                }
                            }
                        }

                    
                        
                        byte_leido1=byte_leido1<<1;
                        byte_leido2=byte_leido2<<1;
                        byte_leido3=byte_leido3<<1;
                        byte_leido4=byte_leido4<<1;                                    
                    }


                }
            

        }
        

    }   


}
