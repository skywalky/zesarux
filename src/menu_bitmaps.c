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
    bitmap_button_ext_desktop_machine,
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
    "                      RR  ",     //0
  	"                     RR   ",      
	"                    RR    ",		
	"                   RR     ",		
	"                  RRR     ",	
	"           RRRRRRRRR      ",			
	"         RRxxxxxxxxR      ",			
	"        RRxxxxxxxxxxR     ", 		
	"       RRxxxxxxxRRRxxR    ",		
	"      RRxxxxxxxxRRRxxR    ",		
	"      RxxxxxxxxxxxxxxR    ",	//10	
	"      RRxxxxxRxxxxxxxR    ",		
	"   RRRRRxxxxxRxxxxxxRR    ",		
	"  RxxxxRRxxxxxRRRRxxRRR   ",		
	"  RxxxxR RRxxxxxxxxR RRR  ",		
	" RxxxxxRR  RRRRRRRR    RR ",		
	" RxxxxxxR              RR ",		
	" RxxxxxxR      RR       RR",		
	" RxxxxxRR   RRRRR       RR",		
	"  RRxxRRRRRR   RR         ",		
	"  RRRRR                   ",    //20
	"  RR  RR                  ",		
	" RR   RR                  ",		
	" RR    RR                 ",		
	"RR     RRR                ",
	"RR      RR                " 	 //25
};


char *bitmap_button_ext_desktop_display[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "                          ",     //0
  	"                          ",      
	"RRRRRRRRRRRRRRRRRRRRRRRRRR",		
	"RrrrrrrrrrrrrrrrrrrrrrrrrR",		
	"Rr                      rR",	
	"Rr         mmmm         rR",			
	"Rr         mmmmm        rR",			
	"Rr         mmmm         rR", 		
	"Rr         xx           rR",		
	"Rr         xx           rR",		
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
	"  xx   x          x   xx  ",		
	" xx   xx    xx    xx   xx ",	
	" xx   xx   xxxx   xx   xx ",			
	" xx   xx   xxxx   xx   xx ",			
	" xx    x    xx    x    xx ",			
  	"  xx   xx        xx   xx  ",		
	"  xx    x        x    xx  ",		
	"   xx       xx       xx   ",	//10	
	"    x       xx       x    ",		
	"           x  x           ",		
	"           x  x           ",		
	"           x  x           ",		
	"          x    x          ",		
	"          x    x          ",		
	"          x    x          ",		
	"         x x  x x         ",		
	"         x  xx  x         ",		
	"        x   xx   x        ",    //20
	"        x  x  x  x        ",		
	"       x  x    x  x       ",		
	"       x x      x x       ",		
	"      x x        x x      ",
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
	"                          ",	
	"                          ",      
	"         bbbbbbbb         ",		
	"        bbbbbbbbbb        ",		
	"       bbbbbbbbbbbb       ",	
	"      bbbbbbbbbbbbbb      ",			
	"     bbbbbbbbbbbbbbbb     ",			
	" bbbbbbbbbbbbbbbbbbbbbbbb ", 		
	"bbbbbbbbbbb    bbbbbbbbbbb",	//10	
	"bbbbbbbbbb      bbbbbbbbbb",		
	"bbbWWbbbb        bbbbbbbbb",		
	"bbbWWbbb   xx     bbbbbbbb",		
	"bbbbbbbb  x       bbbbbbbb",		
	"bbbbbbb   x        bbbbbbb",		
	"bbbbbbb            bbbbbbb",		
	"bbbbbbb            bbbbbbb",		
	"bbbbbbbb          bbbbbbbb",		
	"bbbbbbbb          bbbbbbbb",		
	"bbbbbbbbb        bbbbbbbbb",	//20	
	"bbbbbbbbbb      bbbbbbbbbb",		
	"bbbbbbbbbbb    bbbbbbbbbbb",    
	"bbbbbbbbbbbbbbbbbbbbbbbbbb",		
	"bbbbbbbbbbbbbbbbbbbbbbbbbb",		
	"bbbbbbbbbbbbbbbbbbbbbbbbbb"	//25	
 
	 
};



char *bitmap_button_ext_desktop_windows[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "          xxxxxx          ",     //0
  	"        xxxxxxxxxx        ",      
	"       xxcccxxcccxx       ",		
	"       xxcccxxcccxx       ",		
	"      xxccccxxccccxx      ",	
	"      xxccccxxccccxx      ",			
	"      xxxxxxxxxxxxxx      ",			
	"      xxxxxxxxxxxxxx      ", 		
	"      xxccccxxccccxx      ",		
	"      xxccccxxccccxx      ",		
	"      xxccccxxccccxx      ",	//10	
	"      xxccccxxccccxx      ",		
	"      xxccccxxccccxx      ",		
	"      xxxxxxxxxxxxxx      ",		
	"      xxxxxxxxxxxxxx      ",		
	"      xxccccxxccccxx      ",		
	"      xxccccxxccccxx      ",		
	"      xxccccxxccccxx      ",		
	"      xxccccxxccccxx      ",		
	"      xxccccxxccccxx      ",		
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
	"                          ",	   
	"                          ",
	" xxxxxxxxxxx              ",		
	"xxgggggggggxx             ",		
	"xgggxxxgggggx             ",	
	"xggxx xxggggx             ",			
	"xggx   xgggxx             ",			
	"xggxx xxggxgx     bbbbbbbb", 		
	"xgggxxxggxggx    bbbbbbbbb",	//10	
	"xggggggggxggx   bbybybybbb",		
	"xggg   ggxggx  bbbybybybbb",		
	"xggg   ggxggx bbybybybybbb",		
	"xggg   ggxggx bbybybybybbb",		
	"xggg   ggxggx bbybybybybbb",		
	"xggg   ggxggx bbybybybybbb",		
	"xggg   ggxggx bbybbbbbbbbb",		
	"xggggggggxggx bbbbbbbbbbbb",		
	"xgggxxxggxggx bbwwwbwwbbb",		
	"xggxx xxggxgx bbwbbbwbwbbb",	//20	
	"xggx   xgggxx bbwwwbwbwbbb",		
	"xggxx xxggggx bbbbwbwbwbbb",    
	"xgggxxxgggggx bbwwwbwwbbbb",		
	"xxgggggggggxx bbbbbbbbbbbb",		
	" xxxxxxxxxxx  bbbbbbbbbbbb"  //25		

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



char *bitmap_button_ext_desktop_machine[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "          xxx             ",     //0
  	"         xxxxx            ",      
	"        xx   xx           ",		
	"             xx           ",		
	"            xx            ",	
	"           xx             ",			
	"           xx             ",			
	"                          ", 		
	"           xx             ",		
	"           xx             ",		
	"                          ",	//10	
	"xxxxxxxxxxxxxxxxxxxxxxxxxx",		
	"xxxxxxxxxxxxxxxxxxxxxxxxxx",		
	"xxwwxwxwwxwwxxxxxxxxxxxxxx",		
	"xxxxxxxxxxxxxxxxxxxxxxxxxx",		
	"xxxxxxxxxxxxxxxxxxxxxxxxxx",		
	"xxxxxxxxxxxxxxxxxxxxxxxxxx",		
	"xxwwxwwxwwxwwxwwxwwxwwwxxx",		
	"xxxxxxxxxxxxxxxxxxxxxxxxxx",		
	"xxxwwxwwxwwxwwxwwxwwxwwxxx",		
	"xxxxxxxxxxxxxxxxxxxxxxwxxr",    //20
	"xxxxwwxwwxwwxwwxwwxwwxwxry",		
	"xxxxxxxxxxxxxxxxxxxxxxxryg",		
	"xxxxxxwwxwwwwwwwwxwwxxrygb",		
	"xxxxxxxxxxxxxxxxxxxxxrygbx",
	" xxxxxxxxxxxxxxxxxxxrygbx " 	 //25
};



//Para lower icons

char *bitmap_lowericon_ext_desktop_cassette_active[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "                          ",//0
  	"                          ",
	"                          ",	
	"                          ",
	" wwwwwwwwwwwwwwwwwwwwwwww ",		
	"wwggggggggggggggggggggggww",		
	"wggggggggggggggggggggggggw",	
	"wggggggggggggggggggggggggw",			
	"wggggxxxggggggggggxxxggggw",			
	"wgggxx xxg      gxx xxgggw",		
	"wgggx   xg      gx   xgggw",	//10	
	"wgggxx xxg      gxx xxgggw",		
	"wggggxxxggggggggggxxxggggw",		
	"wggggggggggggggggggggggggw",		
	"wggggggggggggggggggggggggw",		
	"wgggggggxxxxxxxxxxgggggggw",		
	"wggggggxggggggggggxggggggw",		
	"wgggggxggggggggggggxgggggw",		
	"wggggxggggggggggggggxggggw",		
	"wwggxggggggggggggggggxggww",		
	" wwwwwwwwwwwwwwwwwwwwwwww ",	//20	
	"                          ",		
	"                          ",
	"                          ",		
	"                          ",		
	"                          "//25		

};


char *bitmap_lowericon_ext_desktop_cassette_inactive[EXT_DESKTOP_BUTTONS_ANCHO]={
   //01234567890123456789012345
    "                          ",     //0
  	"                          ",    
	"                          ",	   
	"                          ",
	" wwwwwwwwwwwwwwwwwwwwwwww ",		
	"wwxxxxxxxxxxxxxxxxxxxxxxww",		
	"wxx                    xxw",	
	"wx                      xw",			
	"wx   xxx          xxx   xw",			
	"wx  xx xx        xx xx  xw", 		
	"wx  x   x        x   x  xw",	//10	
	"wx  xx xx        xx xx  xw",		
	"wx   xxx          xxx   xw",		
	"wx                      xw",		
	"wx                      xw",		
	"wx      xxxxxxxxxx      xw",		
	"wx     x          x     xw",		
	"wx    x            x    xw",		
	"wxx  x              x  xxw",		
	"wwxxxxxxxxxxxxxxxxxxxxxxww",		
	" wwwwwwwwwwwwwwwwwwwwwwww ",	//20	
	"                          ",		
	"                          ",    
	"                          ",		
	"                          ",		
	"                          "  //25		

};



char *bitmap_lowericon_ext_desktop_mmc_active[EXT_DESKTOP_BUTTONS_ANCHO]={
   //01234567890123456789012345
    "           wwwwwwwwwwww   ",     //0
  	"          wbbbbbbbbbbbw   ",    
	"         wbbbbbbbbbbbbw   ",	   
	"        wbbbbbbbbbbbbbw   ",
	"       wbbyybbyybbyybbw   ",		
	"      wbbbyybbyybbyybbw   ",		
	"     wbbbbyybbyybbyybbw   ",	
	"    wbbbbbyybbyybbyybbw   ",			
	"   wbbbbbbyybbyybbyybbw   ",			
	"   wbbyybbyybbyybbyybbw   ", 		
	"   wbbyybbyybbyybbyybbw   ",	//10	
	"   wbbyybbyybbyybbyybbw   ",		
	"   wbbyybbyybbyybbyybbw   ",		
	"   wbbyybbbbbbbbbbbbbbw   ",		
	"   wbbyybbbbbbbbbbbbbbw   ",		
	"   wbbbbbbbbbbbbbbbbbbw   ",		
	"   wbbbbwwwwbbwwwbbbbbw   ",		
	"   wbbbwbbbbbbwbbwbbbbw   ",		
	"   wbbbwbbbbbbwbbbwbbbw   ",		
	"   wbbbbwwwbbbwbbbwbbbw   ",		
	"   wbbbbbbbwbbwbbbwbbbw   ",	//20	
	"   wbbbbbbbwbbwbbwbbbbw   ",		
	"   wbbbwwwwbbbwwwbbbbbw   ",    
	"   wbbbbbbbbbbbbbbbbbbw   ",		
	"   wbbbbbbbbbbbbbbbbbbw   ",		
	"   wwwwwwwwwwwwwwwwwwww   "  //25		

};


char *bitmap_lowericon_ext_desktop_mmc_inactive[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "           wwwwwwwwwwww   ",     //0
  	"          wxxxxxxxxxxxw   ",    
	"         wx          xw   ",	   
	"        wx           xw   ",
	"       wx xx  xx  xx xw   ",		
	"      wx  xx  xx  xx xw   ",		
	"     wx   xx  xx  xx xw   ",	
	"    wx    xx  xx  xx xw   ",			
	"   wx     xx  xx  xx xw   ",			
	"   wx xx  xx  xx  xx xw   ", 		
	"   wx xx  xx  xx  xx xw   ",	//10	
	"   wx xx  xx  xx  xx xw   ",		
	"   wx xx  xx  xx  xx xw   ",		
	"   wx xx             xw   ",		
	"   wx xx             xw   ",		
	"   wx                xw   ",		
	"   wx   xxxx  xxx    xw   ",		
	"   wx  x      x  x   xw   ",		
	"   wx  x      x   x  xw   ",		
	"   wx   xxx   x   x  xw   ",		
	"   wx      x  x   x  xw   ",	//20	
	"   wx      x  x  x   xw   ",		
	"   wx  xxxx   xxx    xw   ",    
	"   wx                xw   ",		
	"   wxxxxxxxxxxxxxxxxxxw   ",		
	"   wwwwwwwwwwwwwwwwwwww   "  //25		

};


char *bitmap_lowericon_ext_desktop_z88_active[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
	"wwwwwwwwwwwwwwwwwwwwwwwwww", //0
    "wxxxxxxxxxxxxxxxxxxxxxxxxw",     
  	"w xwwwwwwwwwwwwwwwwwwwwx w",      
	"wxxwwxxxwxwwwxxxwxxxwwwxxw",		
	"w xwwxwwwxwwwxwxwwxwwwwx w",		
	"wxxwwxxxwxwwwxwxwwxwwwwxxw",	
	"w xwwwwxwxwwwxwxwwxwwwwx w",			
	"wxxwwxxxwxxxwxxxwwxwwwwxxw",			
	"w xwwwwwwwwwwwwwwwwwwwwx w", 		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",  //10		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",   //20 
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxwwwwwwwwwwwwwwwwwwwwxxw",
	"wwwwwwwwwwwwwwwwwwwwwwwwww" 	 //25
};


char *bitmap_lowericon_ext_desktop_z88_inactive[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
	"wwwwwwwwwwwwwwwwwwwwwwwwww", //0
    "wxxxxxxxxxxxxxxxxxxxxxxxxw",     
  	"w x                    x w",      
	"wxx  xxx x   xxx xxx   xxw",		
	"w x  x   x   x x  x    x w",		
	"wxx  xxx x   x x  x    xxw",	
	"w x    x x   x x  x    x w",			
	"wxx  xxx xxx xxx  x    xxw",			
	"w x                    x w", 		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wx                      xw",  //10		
	"wx                      xw",		
	"wx                      xw",		
	"wx                      xw",		
	"wx                      xw",		
	"wx                      xw",		
	"wx                      xw",		
	"wx                      xw",		
	"wx                      xw",		
	"wx                      xw",		
	"wx                      xw",   //20 
	"wx                      xw",		
	"wx                      xw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxwwwwwwwwwwwwwwwwwwwwxxw",
	"wwwwwwwwwwwwwwwwwwwwwwwwww" 	 //25
};



char *bitmap_lowericon_ext_desktop_mdv_active[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    " wwwwwww            wwwww ",     //0
  	" wxxxxxww          wxxxxw ",      
	" wxxxxxxxwwwwwwwwwwxxxxxw ",		
	" wxxxxxxxxxxxxxxxxxxxxxxw ",		
	" wxxxxxxxxxxxxxxxxxxxxxxw ",	
	"  wxxxxxxxxxxxxxxxxxxxxxw ",			
	"   wxxxxxxxxxxxxxxxxxxxxw ",			
	"   wxxxxxxxxxxxxxxxxxxxxw ", 		
	"   wxxxxxxxxxxxxxxxxxxxxw ",		
	"  wxxxxxxxxxxxxxxxxxxxxxw ",		
	" wxxxxxxxxxxxxxxxxxxxxxxw ",	//10	
	" wxxwwwwwwwwwwwwwwwwwwxxw ",		
	" wxxwwwwwwwwwwwwwwwwwwxxw ",		
	" wxxwwxwwwxwxxxwwxwxwwxxw ",		
	" wxxwwxxwxxwxwwxwxwxwwxxw ",		
	" wxxwwxwxwxwxwwxwxwxwwxxw ",		
	" wxxwwxwwwxwxxxwwwxwwwxxw ",		
	" wxxwwwwwwwwwwwwwwwwwwxxw ",		
	" wxxwwwwwwwwwwwwwwwwwwxxw ",		
	" wxxxxxxxxxxxxxxxxxxxxxxw ",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",    //20
	" wxxxxxxxxxxxxxxxxxxxxxxw ",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	" wxxxxxxxxxxxxxxxxxxxxxxw ",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",
	"wwwwwwwwwwwwwwwwwwwwwwwwww" 	 //25
};


char *bitmap_lowericon_ext_desktop_mdv_inactive[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    " wwwwww             wwwww ",     //0
  	" wxxxxxw           wxxxxw ",      
	" wx    xxwwwwwwwwwwx   xw ",		
	" wx      xxxxxxxxxx    xw ",		
	" wx                    xw ",	
	"  wx                   xw ",			
	"   wx                  xw ",			
	"   wx                  xw ", 		
	"   wx                  xw ",		
	"  wx                   xw ",		
	" wx                    xw ",	//10	
	" wx                    xw ",		
	" wx                    xw ",		
	" wx   x   x xxx  x x   xw ",		
	" wx   xx xx x  x x x   xw ",		
	" wx   x x x x  x x x   xw ",		
	" wx   x   x xxx   x    xw ",		
	" wx                    xw ",		
	" wx                    xw ",		
	" wx                    xw ",		
	"wxx                    xxw",    //20
	" wx                    xw ",		
	"wxx                    xxw",		
	" wx                    xw ",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",
	"wwwwwwwwwwwwwwwwwwwwwwwwww" 	 //25
};



char *bitmap_lowericon_ext_desktop_msx_cart_active[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "                          ",     //0
  	"                          ",      
	"                          ",		
	"                          ",		
	"                          ",	
	"wwwwwwwwwwwwwwwwwwwwwwwwww",			
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",			
	"wxxccccccccccccccccccccxxw", 		
	"wxxccccccccccccccccccccxxw",		
	"wxxcxcxxxcxcxccxccxxcxcxxw",		
	"wxxcxxcxxcxcxcxcxcxxcccxxw",	//10	
	"wxxcxccxxcxxxcxxxcxxcxcxxw",		
	"wxxcxccxxcxcxcxcxcxxcxcxxw",		
	"wxxcxxcxxcxcxcxcxcxxcxcxxw",		
	"wxxcxcxxxcxcxcxcxcxxcxcxxw",		
	"wxxccccccccccccccccccccxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",		
	"wxxxxxxxxxwwwwwwwxxxxxxxxw",		
	"wx xxxxxxxxxwwwxxxxxxxxxxw",		
	"wxxxxxxxxxxxxwxxxxxxxxxxxw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",    //20
	"wwwwwwwwwwwwwwwwwwwwwwwwww",		
	"                          ",		
	"                          ",		
	"                          ",
	"                          " 	 //25
};

char *bitmap_lowericon_ext_desktop_msx_cart_inactive[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "                          ",     //0
  	"                          ",      
	"                          ",		
	"                          ",		
	"                          ",	
	"wwwwwwwwwwwwwwwwwwwwwwwwww",			
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",			
	"wx                      xw", 		
	"wx                      xw",		
	"wx  x xxx x x  x  xx x  xw",		
	"wx  xx xx x x x x xx    xw",	//10	
	"wx  x  xx xxx xxx xx x  xw",		
	"wx  x  xx x x x x xx x  xw",		
	"wx  xx xx x x x x xx x  xw",		
	"wx  x xxx x x x x xx x  xw",		
	"wx                      xw",		
	"wx                      xw",		
	"wxx       xxxxxxx       xw",		
	"wx x        xxx         xw",		
	"wxx          x          xw",		
	"wxxxxxxxxxxxxxxxxxxxxxxxxw",    //20
	"wwwwwwwwwwwwwwwwwwwwwwwwww",		
	"                          ",		
	"                          ",		
	"                          ",
	"                          " 	 //25
};


char *bitmap_lowericon_ext_desktop_svi_active[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "     wwwwwwwwwwwwwwwww    ",     //0
  	"    wxxxxxxxxxxxxxxxxxw   ",      
	"   wxxxxxwwwwwwwwxxxxxxw  ",		
	"  wxxxxxxwxxxxxxwxxxxxxw  ",		
	"  wxxxxxxwwwwwwwwxxxxxxw  ",	
	"  wxxxRRRwxxxxxxwRRRxxxw  ",			
	"  wxxRyyywwwwwwwwyyyRxxw  ",			
	"  wxxRyxxxxxxxxxxxxyRxxw  ", 		
	"  wxxRyxxRRRRRRRRxxyRxxw  ",		
	"  wxxRyxxxxxxxxxxxxyRxxw  ",		
	"  wxxRyxRRxRRxRRxRxyRxxw  ",	//10	
	"  wxxRyxRxxRRxRxxRxyRxxw  ",		
	"  wxxRyxRRxRRxxRxRxyRxxw  ",		
	"  wxxRyxxxxxxxxxxxxyRxxw  ",		
	"  wxxRywxxwxxwxxwxwyRxxw  ",		
	"  wxxRyxwxxwxwxwxwxyRxxw  ",		
	"  wxxRyxxwxwxwxwxwxyRxxw  ",		
	"  wxxRyyyyyyyyyyyyyyRxxw  ",		
	"  wxxxRRRRRRRRRRRRRRxxxw  ",		
	"  wxxxxxxxxxxxxxxxxxxxxw  ",		
	"  wxxxxxxxxxxxxxxxxxxxxw  ",    //20
	"  wxxxxxxxxxxxxxxxxxxxxw  ",		
	"  wxxxxxWxWxWxWxWxWxxxxw  ",		
	"  wxwxxxWxWxWxWxWxWxxwxw  ",		
	"  wxwxxxxxxxxxxxxxxxxwxw  ",
	"  wwwwwwwwwwwwwwwwwwwwww  " 	 //25
};

char *bitmap_lowericon_ext_desktop_svi_inactive[EXT_DESKTOP_BUTTONS_ANCHO]={
    //01234567890123456789012345
    "     wwwwwwwwwwwwwwww     ",     //0
  	"    wxxxxxxxxxxxxxxxxw    ",      
	"   wx                xw   ",		
	"  wx                  xw  ",		
	"  wx                  xw  ",	
	"  wx                  xw  ",			
	"  wx                  xw  ",			
	"  wx                  xw  ", 		
	"  wx                  xw  ",		
	"  wx                  xw  ",		
	"  wx                  xw  ",	//10	
	"  wx                  xw  ",		
	"  wx                  xw  ",		
	"  wx                  xw  ",		
	"  wx                  xw  ",		
	"  wx                  xw  ",		
	"  wx                  xw  ",		
	"  wx                  xw  ",		
	"  wx                  xw  ",		
	"  wx                  xw  ",		
	"  wx                  xw  ",    //20
	"  wxxxxxxxxxxxxxxxxxxxxw  ",		
	"  wxxxxx x x x x x xxxxw  ",		
	"  wx xxx x x x x x xx xw  ",		
	"  wx xxxxxxxxxxxxxxxx xw  ",
	"  wwwwwwwwwwwwwwwwwwwwww  " 	 //25
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

