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
#include "audio.h"

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

    //int slot,segment;

    switch (puerto_l) {
        case 0xA8:
            msx_ppi_register_a=value;
            //printf ("Out port ppi. Port %02XH value %02XH\n",puerto_l,value);

    //temporal mostrar mapeos
    
/*
    for (slot=0;slot<4;slot++) {
        for (segment=0;segment<4;segment++) {
            printf ("%d %d : %d\n",slot,segment,msx_memory_slots[slot][segment]);
        }
    }
*/

        break;

        case 0xA9:
            msx_ppi_register_b=value;
        break;


        case 0xAA:
            msx_ppi_register_c=value;

            
                //printf ("Posible beep: %d\n",value&128);
            
			set_value_beeper_on_array(da_amplitud_speaker_msx() );


        break;
    }
}

z80_byte msx_in_port_ppi(z80_byte puerto_l)
{
    //printf ("In port ppi. Port %02XH\n",puerto_l);

    //z80_byte valor;

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

            //Antes, si es un bloque de 8kb, copiar 8kb bajos en parte alta
            if (tamanyo_archivo==8192) {
                memcpy(&memoria_spectrum[65536+8192],&memoria_spectrum[65536],8192);
            }

            memcpy(&memoria_spectrum[65536],&memoria_spectrum[65536+16384],16384);
            memcpy(&memoria_spectrum[65536+32768],&memoria_spectrum[65536+16384],16384);
            memcpy(&memoria_spectrum[65536+49152],&memoria_spectrum[65536+16384],16384);

            msx_memory_slots[1][0]=MSX_SLOT_MEMORY_TYPE_ROM;
            msx_memory_slots[1][2]=MSX_SLOT_MEMORY_TYPE_ROM;
            msx_memory_slots[1][3]=MSX_SLOT_MEMORY_TYPE_ROM;
    }


    
    //int i;


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

    unsigned int color=vdp_9918a_get_border_color();




        int x,y;




	//Top border cambia en spectrum y zx8081 y ace
	int topborder=MSX_TOP_BORDER;
	

	//color +=spectrum_palette_offset;


        //parte superior
        for (y=0;y<topborder;y++) {
                for (x=0;x<MSX_ANCHO_PANTALLA*zoom_x+MSX_LEFT_BORDER*2;x++) {
                                scr_putpixel(x,y,VDP_9918_INDEX_FIRST_COLOR+color);
                }
        }

        //parte inferior
        for (y=0;y<MSX_BOTTOM_BORDER;y++) {
                for (x=0;x<MSX_ANCHO_PANTALLA*zoom_x+MSX_LEFT_BORDER*2;x++) {
                                scr_putpixel(x,topborder+y+MSX_ALTO_PANTALLA*zoom_y,VDP_9918_INDEX_FIRST_COLOR+color);


                }
        }






        for (y=0;y<MSX_ALTO_PANTALLA*zoom_y;y++) {
                for (x=0;x<MSX_LEFT_BORDER;x++) {
                        scr_putpixel(x,topborder+y,VDP_9918_INDEX_FIRST_COLOR+color);
                }

        

        }

        int ancho_pantalla=MSX_ANCHO_PANTALLA;
        int ancho_border_derecho=MSX_LEFT_BORDER;

        //laterales. En modo 0, 40x24, border derecho es 16 pixeles mas ancho
        z80_byte video_mode=vdp_9918a_get_video_mode();        

        if (video_mode==4) {
            ancho_pantalla -=16;
            ancho_border_derecho +=16*zoom_x;
        }

        for (y=0;y<MSX_ALTO_PANTALLA*zoom_y;y++) {

                for (x=0;x<ancho_border_derecho;x++) {
                        scr_putpixel(MSX_LEFT_BORDER+ancho_pantalla*zoom_x+x,topborder+y,VDP_9918_INDEX_FIRST_COLOR+color);
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

        //int offset_sprite=sprite_attribute_table;

        //int i;
        for (primer_sprite_final=0;primer_sprite_final<32 && !salir;primer_sprite_final++) {
            int offset_sprite=sprite_attribute_table+primer_sprite_final*4;

            z80_byte vert_pos=msx_read_vram_byte(offset_sprite);
            if (vert_pos==208) salir=1;

        }

        //Siempre estara al siguiente
        primer_sprite_final--;

        sprite_attribute_table +=(primer_sprite_final*4);

        //Empezar desde final hacia principio

        for (sprite=primer_sprite_final;sprite>=0;sprite--) {
            int vert_pos=msx_read_vram_byte(sprite_attribute_table);
            int horiz_pos=msx_read_vram_byte(sprite_attribute_table+1);
            z80_byte sprite_name=msx_read_vram_byte(sprite_attribute_table+2);
            z80_byte attr_color_etc=msx_read_vram_byte(sprite_attribute_table+3);

/*
  0: Y-pos, Vertical position (FFh is topmost, 00h is second line, etc.)
  1: X-pos, Horizontal position (00h is leftmost)
  2: Pattern number
  3: Attributes. b0-3:Color, b4-6:unused, b7:EC (Early Clock)            
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
            sprite_attribute_table -=4;

            //Si early clock, x-=32

            if (attr_color_etc & 128) {
                //printf ("sprite number: %d X: %d Y: %d Name: %d color_etc: %d\n",sprite,horiz_pos,vert_pos,sprite_name,attr_color_etc);                
                horiz_pos -=32;
            }

            //printf ("sprite number: %d X: %d Y: %d Name: %d color_etc: %d\n",sprite,horiz_pos,vert_pos,sprite_name,attr_color_etc);

       
                

                //Si coord Y no esta en el borde inferior
                if (vert_pos<192) {
                    //int offset_pattern_table=sprite_name*bytes_per_sprite+sprite_pattern_table;
                      int offset_pattern_table=sprite_name*8+sprite_pattern_table;
                    z80_byte color=attr_color_etc & 15;

                    int x,y;

                    //Sprites de 16x16
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
                                        if (pos_x_final>=0 && pos_x_final<=255 && pos_y_final>=0 && pos_y_final<=191) {

                                            //Si bit a 1
                                            if (byte_leido & 128) {
                                                //Y si ese color no es transparente 
                                                if (color!=0) {
                                                    //printf ("putpixel sprite x %d y %d\n",pos_x_final,pos_y_final);

                                                    z80_byte color_sprite=color;

                                                    if (msx_reveal_layer_sprites.v) {
                                                        int posx=pos_x_final&1;
                                                        int posy=pos_y_final&1;

                                                        //0,0: 0
                                                        //0,1: 1
                                                        //1,0: 1
                                                        //1,0: 0
                                                        //Es un xor

                                                        int si_blanco_negro=posx ^ posy;
                                                        //printf ("si_blanco_negro: %d\n",si_blanco_negro);
                                                        color_sprite=si_blanco_negro*15;
                                                        //printf ("color: %d\n",color);
                                                    }


                                                    scr_putpixel_zoom(pos_x_final,  pos_y_final,  VDP_9918_INDEX_FIRST_COLOR+color_sprite);
                                                }
                                            }

                                            byte_leido = byte_leido << 1;
                                        }
                                    }
                                }
                            }
                        }                        
                    }

                    //Sprites de 8x8
                    else {

                        for (y=0;y<8;y++) {

                                byte_leido=msx_read_vram_byte(offset_pattern_table++);
                                for (x=0;x<8;x++) {

                                    int pos_x_final;
                                    int pos_y_final;

                                    pos_x_final=horiz_pos+x;
                                    pos_y_final=vert_pos+y;
                                    
                                    if (pos_x_final>=0 && pos_x_final<=255 && pos_y_final>=0 && pos_y_final<=191) {

                                        //Si bit a 1
                                        if (byte_leido & 128) {
                                            //Y si ese color no es transparente
                                            if (color!=0) {
                                                //printf ("putpixel sprite x %d y %d\n",pos_x_final,pos_y_final);

                                                z80_byte color_sprite=color;

                                                if (msx_reveal_layer_sprites.v) {
                                                    int posx=pos_x_final&1;
                                                    int posy=pos_y_final&1;

                                                    //0,0: 0
                                                    //0,1: 1
                                                    //1,0: 1
                                                    //1,0: 0
                                                    //Es un xor

                                                    int si_blanco_negro=posx ^ posy;
                                                    color_sprite=si_blanco_negro*15;
                                                }                                            
                                                scr_putpixel_zoom(pos_x_final,  pos_y_final,  VDP_9918_INDEX_FIRST_COLOR+color_sprite);
                                            }
                                        }
                                    }

                                    byte_leido = byte_leido << 1;
                                }
                            
                        }
                    }

                }
            

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

        //Capa activada. Pero tiene reveal?

        if (msx_reveal_layer_ula.v) {
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
            vdp_9918a_render_ula_no_rainbow(msx_vram_memory);
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



    if (msx_force_disable_layer_sprites.v==0) {
        msx_render_sprites_no_rainbow();
    }
        
        


}


int da_amplitud_speaker_msx(void)
{
                                if (msx_ppi_register_c & 128) return amplitud_speaker_actual_msx;
                                else return -amplitud_speaker_actual_msx;
}