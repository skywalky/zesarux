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

#include "menu_bitmaps.h"

char **zxdesktop_buttons_bitmaps[EXT_DESKTOP_TOTAL_BUTTONS]={
    zesarux_ascii_logo,
    bitmap_button_ext_desktop_help,
    zesarux_ascii_logo,
    zesarux_ascii_logo,
    zesarux_ascii_logo,  
    zesarux_ascii_logo, //5
    zesarux_ascii_logo,
    zesarux_ascii_logo,
    zesarux_ascii_logo,
    zesarux_ascii_logo,
    zesarux_ascii_logo, //10
    zesarux_ascii_logo,
    zesarux_ascii_logo //12
    
};


char *zesarux_ascii_logo[ZESARUX_ASCII_LOGO_ALTO]={
    //01234567890123456789012345
    "wwwwwwwwwwwwwwwwwwwwwwwwww",     //0
  	"wxxxxxxxxxxxxxxxxxxxxxxxxw",      
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",	
	"wwwwwwwwwwwwwwwwwxxxxwwwww",			
	"                wxxxxw   w",			
	"                wxxxxw  rw", 		
	"             wwwwxxxxw rrw",		
	"            wxxxxwwww rrrw",		
	"            wxxxxw   rrrrw",	//10	
	"            wxxxxw  rrrryw",		
	"         wwwwxxxxw rrrryyw",		
	"        wxxxxwwww rrrryyyw",		
	"        wxxxxw   rrrryyyyw",		
	"        wxxxxw  rrrryyyygw",		
	"     wwwwxxxxw rrrryyyyggw",		
	"    wxxxxwwww rrrryyyygggw",		
	"    wxxxxw   rrrryyyyggggw",		
	"    wxxxxw  rrrryyyyggggcw",		
	"wwwwwxxxxw rrrryyyyggggccw",    //20
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",
	"wwwwwwwwwwwwwwwwwwwwwwwwww" 		//25
};


//Prueba boton ayuda
char *bitmap_button_ext_desktop_help[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "          GGGGGGG         ",     //0
  	"        GGGGGGGGGG        ",      
	"     GGGGGGGGGGGGGG       ",		
	"     GGGGGGGGGGGGGGGG     ",		
	"   GGGGGGGGGGGGGGGGGG     ",	
	"   GGGGG       GGGGGGG    ",			
	"                GGGGGG    ",			
	"                GGGGGG    ", 		
	"             GGGGGGGGG    ",		
	"            GGGGGGGGG     ",		
	"            GGGGGG        ",	//10	
	"            GGGGGG        ",		
	"         GGGGGGGGG        ",		
	"        GGGGGGGGG         ",		
	"        GGGGGG            ",		
	"        GGGGGG            ",		
	"     GGGGGGGGG            ",		
	"    GGGGGGGGG             ",		
	"    GGGGGG                ",		
	"    GGGGGG                ",		
	"    GGGGGG                ",    //20
	"                          ",		
	"                          ",		
	"    GGGGGG                ",		
	"    GGGGGG                ",
	"    GGGGGG                " 		//25
};