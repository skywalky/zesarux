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

/*
0 ZEsarUX
1 Smartload
2 Snapshot 
3 Machine
4 Audio
5 Display
6 Storage
7 Debug
8 Network
9 Windows

10 Settings
11 Help
12 Exit

*/
char **zxdesktop_buttons_bitmaps[EXT_DESKTOP_TOTAL_BUTTONS]={
    zesarux_ascii_logo,
    zesarux_ascii_logo,
    zesarux_ascii_logo,
    zesarux_ascii_logo,
    zesarux_ascii_logo,  
    zesarux_ascii_logo, //5
    zesarux_ascii_logo,
    bitmap_button_ext_desktop_debug,
    zesarux_ascii_logo,
    zesarux_ascii_logo,
    zesarux_ascii_logo, //10
    bitmap_button_ext_desktop_help,
    zesarux_ascii_logo //12
    
};

//Usado en watermark y en botones
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


//Iconos con contenido 26x26. 
	//Hay que dejar margen de 6 por cada lado (3 izquierdo, 3 derecho, 3 alto, 3 alto)
	//Cada 3 pixeles de margen son: fondo-negro(rectangulo)-gris(de dentro boton)
	//total maximo 32x32 
	//Ejemplo:
	/*

char *zesarux_ascii_logo[ZESARUX_ASCII_LOGO_ALTO]={ 
  ................................
  ################################
  --------------------------------
    "WWWWWWWWWWWWWWWWWWWWWWWWWW",     //0
  	"WXXXXXXXXXXXXXXXXXXXXXXXXW",      
	"WXXXXXXXXXXXXXXXXXXXXXXXXW",		
	"WXXXXXXXXXXXXXXXXXXXXXXXXW",		
	"WXXXXXXXXXXXXXXXXXXXXXXXXW",	
	"WWWWWWWWWWWWWWWWWXXXXWWWWW",			
	"                WXXXXW   W",			
	"                WXXXXW  RW", 		
	"             WWWWXXXXW RRW",		
	"            WXXXXWWWW RRRW",		
	"            WXXXXW   RRRRW",	//10	
	"            WXXXXW  RRRRYW",		
	"         WWWWXXXXW RRRRYYW",		
	"        WXXXXWWWW RRRRYYYW",		
	"        WXXXXW   RRRRYYYYW",		
	"        WXXXXW  RRRRYYYYGW",		
	"     WWWWXXXXW RRRRYYYYGGW",		
	"    WXXXXWWWW RRRRYYYYGGGW",		
	"    WXXXXW   RRRRYYYYGGGGW",		
	"    WXXXXW  RRRRYYYYGGGGCW",		
	"WWWWWXXXXW RRRRYYYYGGGGCCW",    //20
	"WXXXXXXXXXXXXXXXXXXXXXXXXW",		
	"WXXXXXXXXXXXXXXXXXXXXXXXXW",		
	"WXXXXXXXXXXXXXXXXXXXXXXXXW",		
	"WXXXXXXXXXXXXXXXXXXXXXXXXW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWW" 		//25
};
	*/

/*
Template
char *bitmap_button_ext_desktop_xxxxx[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "                          ",     //0
  	"                          ",      
	"                          ",		
	"                          ",		
	"                          ",	
	"                          ",			
	"                          ",			
	"                          ", 		
	"                          ",		
	"                          ",		
	"                          ",	//10	
	"                          ",		
	"                          ",		
	"                          ",		
	"                          ",		
	"                          ",		
	"                          ",		
	"                          ",		
	"                          ",		
	"                          ",		
	"                          ",    //20
	"                          ",		
	"                          ",		
	"                          ",		
	"                          ",
	"                          " 	 //25
};

*/

//Boton ayuda
char *bitmap_button_ext_desktop_help[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "          ggggggg         ",     //0
  	"        gggggggggg        ",      
	"     gggggggggggggg       ",		
	"     gggggggggggggggg     ",		
	"   gggggggggggggggggg     ",	
	"   ggggg       ggggggg    ",			
	"                gggggg    ",			
	"                gggggg    ", 		
	"             ggggggggg    ",		
	"            ggggggggg     ",		
	"            gggggg        ",	//10	
	"            gggggg        ",		
	"         ggggggggg        ",		
	"        ggggggggg         ",		
	"        gggggg            ",		
	"        gggggg            ",		
	"     ggggggggg            ",		
	"    ggggggggg             ",		
	"    gggggg                ",		
	"    gggggg                ",		
	"    gggggg                ",    //20
	"                          ",		
	"                          ",		
	"    gggggg                ",		
	"    gggggg                ",
	"    gggggg                " 		//25
};


char *bitmap_button_ext_desktop_debug[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "                       r  ",     //0
  	"                      r   ",      
	"                     r    ",		
	"                    r     ",		
	"                   rr     ",	
	"           rrrrrrrrr      ",			
	"        rrrrr      r      ",			
	"        rrr         r     ", 		
	"       rr       rr   r    ",		
	"      rr        rr   r    ",		
	"      r              r    ",	//10	
	"      rr             r    ",		
	"   rrrrr     rr     rr    ",		
	"  r    rr     rrr   r r   ",		
	"  r    r rr      rrr  rr  ",		
	" r     rr  rrrrrrrr     r ",		
	" r      r               r ",		
	" r      r      rr        r",		
	" r     rr   rrrrr        r",		
	"  rr  rrrrrr   rr         ",		
	"  rrrrr                   ",    //20
	"  r   r                   ",		
	" r    r                   ",		
	" r     r                  ",		
	"r      rr                 ",
	"r       r                 " 	 //25
};