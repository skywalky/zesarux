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
    bitmap_button_ext_desktop_smartload,
    bitmap_button_ext_desktop_snapshot,
    zesarux_ascii_logo,
    bitmap_button_ext_desktop_audio,  
    bitmap_button_ext_desktop_display, //5
    bitmap_button_ext_desktop_storage,
    bitmap_button_ext_desktop_debug,
    bitmap_button_ext_desktop_network,
    bitmap_button_ext_desktop_windows,
    bitmap_button_ext_desktop_settings, //10
    bitmap_button_ext_desktop_help,
    bitmap_button_ext_desktop_exit //12
    
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


//Boton ayuda
char *bitmap_button_ext_desktop_help[EXT_DESKTOP_BUTTONS_ANCHO]={
   //01234567890123456789012345
    "        bbbbbbbb          ",     //0
  	"       bbbbbbbbbb         ",      
	"      bbb      bbb        ",		
	"     bbb        bbb       ",		
	"    bbb          bbb      ",	
	"    bbb          bbb      ",			
	"    bbb          bbb      ",			
	"    bbb          bbb      ", 		
	"                 bbb      ",		
	"                 bbb      ",		
	"                bbbb      ",	//10	
	"               bbbb       ",		
	"              bbbb        ",		
	"             bbbb         ",		
	"            bbbb          ",		
	"            bbb           ",		
	"            bbb           ",		
	"            bbb           ",		
	"            bbb           ",		
	"            bbb           ",		
	"                          ",    //20
	"             b            ",		
	"            bbb           ",		
	"           bbbbb          ",		
	"            bbb           ",
	"             b            " 	 //25
};


char *bitmap_button_ext_desktop_debug[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "                      rr  ",     //0
  	"                     rr   ",      
	"                    rr    ",		
	"                   rr     ",		
	"                  rrr     ",	
	"           rrrrrrrrr      ",			
	"        rrrrrxxxxxxr      ",			
	"        rrrxxxxxxxxxr     ", 		
	"       rrxxxxxxxrrrxxr    ",		
	"      rrxxxxxxxxrrrxxr    ",		
	"      rxxxxxxxxxxxxxxr    ",	//10	
	"      rrxxxxxxxxxxxxxr    ",		
	"   rrrrrxxxxxrrrxxxxrr    ",		
	"  rxxxxrrxxxxxrrrrxxrrr   ",		
	"  rxxxxr rrxxxxxxrrr rrr  ",		
	" rxxxxxrr  rrrrrrrr    rr ",		
	" rxxxxxxr              rr ",		
	" rxxxxxxr      rr       rr",		
	" rxxxxxrr   rrrrr       rr",		
	"  rrxxrrrrrr   rr         ",		
	"  rrrrr                   ",    //20
	"  rr  rr                  ",		
	" rr   rr                  ",		
	" rr    rr                 ",		
	"rr     rrr                ",
	"rr      rr                " 	 //25
};


char *bitmap_button_ext_desktop_display[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "                          ",     //0
  	"                          ",      
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",		
	"RrrrrrrrrrrrrrrrrrrrrrrrrR",		
	"Rr           g          rR",	
	"Rr         gggg         rR",			
	"Rr         ggggg        rR",			
	"Rr         gggg         rR", 		
	"Rr         gg           rR",		
	"Rr         gg           rR",		
	"Rr  xxxxxxxxxxxxxxxxx   rR",	//10	
	"Rr  xxxxxxxxxxxxxxxxx   rR",		
	"Rr   xxxxxxxxxxxxxxx    rR",		
	"Rr    xxxxxxxxxxxxx     rR",		
	"RrbbbbbbbbbbbbbbbbbbbbbbrR",		
	"RrBBccccccbbbcccccccccccrR",		
	"RrBBBBbbbbbbccBBBBBbbbbbrR",		
	"RrBBBBbbbBBBBcBBBBBBBBBBrR",		
	"RrBBBBBBBBBBBBBBBBBBBBBBrR",		
	"RrBBBBBBBBBBBBBBBBBBBBBBrR",		
	"RrrrrrrrrrrrrrrrrrrrrrrrrR",    //20
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",		
	"        RRRRRRRRRR        ",		
	"       RRRRRRRRRRRR       ",		
	"      RRRRRRRRRRRRRR      ",
	"     rrrrrrrrrrrrrrrr     " 	 //25
};



char *bitmap_button_ext_desktop_network[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "    x                x    ",     //0
  	"   xx   x        x   xx   ",      
	"  xx   xx        xx   xx  ",		
	"  xx   x    xx    x   xx  ",		
	" xx   xx   xxxx   xx   xx ",	
	" xx   xx   xxxx   xx   xx ",			
	" xx   xx    xx    xx   xx ",			
	" xx    x          x    xx ",			
  	"   xx  xx        xx  xx   ",		
	"   xx   x   xx   x   xx   ",		
	"    xx      xx      xx    ",	//10	
	"     x     xxxx     x     ",		
	"           xxxx           ",		
	"           x  x           ",		
	"           x  x           ",		
	"          xx  xx          ",		
	"          x    x          ",		
	"         xx    xx         ",		
	"         xxx  xxx         ",		
	"         xx xx xx         ",		
	"        xx  xx  xx        ",    //20
	"        xx x  x xx        ",		
	"       xx x    x xx       ",		
	"       xxx      xxx       ",		
	"      xxx        xxx      ",
	"      xxxxxxxxxxxxxx      " 	 //25
};



char *bitmap_button_ext_desktop_audio[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "        xxx               ",     //0
  	"        xxxxx             ",      
	"        xxxxxxx           ",		
	"        xx  xxxx          ",		
	"        xx    xxxx        ",	
	"        xxxx    xxxx      ",			
	"        xxxxxx    xxx     ",			
	"        xx  xxxx   xxxx   ", 		
	"        xx    xxxx  xxx   ",		
	"        xx      xxx  xx   ",		
	"        xx        xx xx   ",	//10	
	"        xx         xxxx   ",		
	"        xx          xxx   ",		
	"        xx           xx   ",		
	"        xx           xx   ",		
	"        xx           xx   ",		
	"        xx           xx   ",		
	"      xxxx           xx   ",		
	"     xxxxx           xx   ",		
	"     xxxxx           xx   ",		
	"     xxxxx           xx   ",    //20
	"      xxx          xxxx   ",		
	"                  xxxxx   ",		
	"                  xxxxx   ",		
	"                  xxxxx   ",
	"                   xxx    " 	 //25
};




char *bitmap_button_ext_desktop_snapshot[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "                          ",     //0
  	"                          ",      
	"         bbbbbbbb         ",		
	"        bbbbbbbbbb        ",		
	"       bbbbbbbbbbbb       ",	
	"      bbbbbbbbbbbbbb      ",			
	"     bbbbbbbbbbbbbbbb     ",			
	" bbbbbbbbbbbbbbbbbbbbbbbb ", 		
	"bbbbbbbbbbb    bbbbbbbbbbb",		
	"bbbbbbbbbb      bbbbbbbbbb",		
	"bbbWWbbbb        bbbbbbbbb",	//10	
	"bbbWWbbb   xx     bbbbbbbb",		
	"bbbbbbbb  x       bbbbbbbb",		
	"bbbbbbb   x        bbbbbbb",		
	"bbbbbbb            bbbbbbb",		
	"bbbbbbb            bbbbbbb",		
	"bbbbbbbb          bbbbbbbb",		
	"bbbbbbbb          bbbbbbbb",		
	"bbbbbbbbb        bbbbbbbbb",		
	"bbbbbbbbbb      bbbbbbbbbb",		
	"bbbbbbbbbbb    bbbbbbbbbbb",    //20
	"bbbbbbbbbbbbbbbbbbbbbbbbbb",		
	"bbbbbbbbbbbbbbbbbbbbbbbbbb",		
	"bbbbbbbbbbbbbbbbbbbbbbbbbb",		
	"                          ",
	"                          " 	 //25
};



char *bitmap_button_ext_desktop_windows[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "          xxxxxx          ",     //0
  	"        xxxxxxxxxx        ",      
	"       xxYYYxxYYYxx       ",		
	"       xxYYYxxYYYxx       ",		
	"      xxYYYYxxYYYYxx      ",	
	"      xxYYYYxxYYYYxx      ",			
	"      xxxxxxxxxxxxxx      ",			
	"      xxxxxxxxxxxxxx      ", 		
	"      xxYYYYxxYYYYxx      ",		
	"      xxYYYYxxYYYYxx      ",		
	"      xxYYYYxxYYYYxx      ",	//10	
	"      xxYYYYxxYYYYxx      ",		
	"      xxYYYYxxYYYYxx      ",		
	"      xxxxxxxxxxxxxx      ",		
	"      xxxxxxxxxxxxxx      ",		
	"      xxYYYYxxYYYYxx      ",		
	"      xxYYYYxxYYYYxx      ",		
	"      xxYYYYxxYYYYxx      ",		
	"      xxYYYYxxYYYYxx      ",		
	"      xxYYYYxxYYYYxx      ",		
	"      xxxxxxxxxxxxxx      ",    //20
	"      xxxxxxxxxxxxxx      ",		
	"                          ",		
	"                          ",		
	"     xxxxxxxxxxxxxxxx     ",
	"     xxxxxxxxxxxxxxxx     " 	 //25
};

char *bitmap_button_ext_desktop_exit[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "RRRRRRRRRRRRRRRRRRRRRRRRRR",     //0
  	"RRRRRRRRRRRRRRRRRRRRRRRRRR",      
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",		
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",		
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",	
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",			
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",			
	"RRRRRRRRRRRRRRRRRRRRRRRRRR", 		
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",		
	"xxxxxRRxxRRxxRRxxRRxxxxxxR",		
	"xxxxxRRxxRRxxRRxxRRxxxxxxR",	//10	
	"xxRRRRRRxxxxRRRxxRRRRxxRRR",		
	"xxxxxRRRRxxRRRRxxRRRRxxRRR",		
	"xxxxxRRRRxxRRRRxxRRRRxxRRR",		
	"xxRRRRRRxxxxRRRxxRRRRxxRRR",		
	"xxxxxRRxxRRxxRRxxRRRRxxRRR",		
	"xxxxxRRxxRRxxRRxxRRRRxxRRR",		
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",		
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",		
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",		
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",    //20
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",		
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",		
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",		
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",
	"RRRRRRRRRRRRRRRRRRRRRRRRRR" 	 //25
};





char *bitmap_button_ext_desktop_storage[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "                          ",     //0
  	"                          ",      
	" xxxxxxxxxxx              ",		
	"xxgggggggggxx             ",		
	"xgggxxxgggggx             ",	
	"xggxx xxggggx             ",			
	"xggx   xgggxx             ",			
	"xggxx xxggxgx     bbbbbbbb", 		
	"xgggxxxggxggx    bbbbbbbbb",		
	"xggggggggxggx   bbybybybbb",		
	"xggg   ggxggx  bbbybybybbb",	//10	
	"xggg   ggxggx bbybybybybbb",		
	"xggg   ggxggx bbybybybybbb",		
	"xggg   ggxggx bbybybybybbb",		
	"xggg   ggxggx bbybybybybbb",		
	"xggg   ggxggx bbybbbbbbbbb",		
	"xggggggggxggx bbbbbbbbbbbb",		
	"xgggxxxggxggx bbwwwbwwbbb",		
	"xggxx xxggxgx bbwbbbwbwbbb",		
	"xggx   xgggxx bbwwwbwbwbbb",		
	"xggxx xxggggx bbbbwbwbwbbb",    //20
	"xgggxxxgggggx bbwwwbwwbbbb",		
	"xxgggggggggxx bbbbbbbbbbbb",		
	" xxxxxxxxxxx  bbbbbbbbbbbb",		
	"                          ",
	"                          " 	 //25
};




char *bitmap_button_ext_desktop_smartload[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "                x         ",     //0
  	"           x    x     x   ",      
	"            x   x    x    ",		
	"             x      x     ",		
	"               xxxx       ",	
	"             xxyyyyxx     ",			
	"            xyyyyyyyyx    ",			
	"            xyyyyxyyyx    ", 		
	"           xyyyyxyyyyyx   ",		
	"        xx xyyyxyyyyyyx xx",		
	"           xyyxxxxxyyyx   ",	//10	
	"           xyyyyyxyyyyx   ",		
	"            xyyyxyyyyx    ",		
	"            xyyxyyyyyx    ",		
	"             xyyyyyyx     ",		
	" xxxxx       xyyyyyyx     ",		
	"x     x      xxyyyyxx     ",		
	"x xxx x       xyyyyx      ",		
	"x     x  xxxx xyyyyx      ",		
	"x     x    xx xxxxxx      ",		
	"x xxx x   x x             ",    //20
	"x     x  x  x xxxxxx      ",		
	"x     x x     xxxxxx      ",		
	"x xxx x                   ",		
	"x     x        xxxx       ",
	" xxxxx         xxxx       " 	 //25
};


char *bitmap_button_ext_desktop_settings[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "   xxx      xxx      xxx  ",     //0
  	"   xxx      xxx      xxx  ",      
	"   xxx      xxx      xxx  ",		
	"   xxx      xxx      xxx  ",		
	"   xxx      xxx      xxx  ",	
	"   xxx      xxx      xxx  ",			
	"   xxx      xxx      xxx  ",			
	"            xxx      xxx  ", 		
	" xxxxxxx    xxx      xxx  ",		
	" xxxxxxx    xxx      xxx  ",		
	" xxxxxxx    xxx      xxx  ",	//10	
	"            xxx      xxx  ",		
	"   xxx      xxx      xxx  ",		
	"   xxx      xxx      xxx  ",		
	"   xxx      xxx           ",		
	"   xxx      xxx    xxxxxxx",		
	"   xxx      xxx    xxxxxxx",		
	"   xxx      xxx    xxxxxxx",		
	"   xxx                    ",		
	"   xxx    xxxxxxx    xxx  ",		
	"   xxx    xxxxxxx    xxx  ",    //20
	"   xxx    xxxxxxx    xxx  ",		
	"   xxx               xxx  ",		
	"   xxx      xxx      xxx  ",		
	"   xxx      xxx      xxx  ",
	"   xxx      xxx      xxx  " 	 //25
};

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
