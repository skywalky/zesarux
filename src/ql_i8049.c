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


#include "ql_i8049.h"
#include "ql.h"
#include "debug.h"
#include "utils.h"


/*

Intel 8049 "Intelligent Peripheral Controller (IPC)" emulation

Actually it is not a real emulation, I am not emulating the cpu, but simulating its behaviour

Info:

The 8049, IC24, is a totally self-sufficient 8-bit single chip microcomputer containing 2 k bytes of program memory and 128 bytes of RAM. It is clocked internally at 11 MHz from crystal X4

In this application the function of the 8049 is to:

receive RS232 interface signals,
monitor the keyboard,
control the loudspeaker,
control the joystick.
The IPC utilises a data bus, two 8-bit I/O ports and some control lines to control these functions.

*/

#define QL_STATUS_IPC_IDLE 0
#define QL_STATUS_IPC_WRITING 1

//Valores que me invento para gestionar pulsaciones de teclas no ascii
#define QL_KEYCODE_F1 256
#define QL_KEYCODE_F2 257
#define QL_KEYCODE_UP 258
#define QL_KEYCODE_DOWN 259
#define QL_KEYCODE_LEFT 260
#define QL_KEYCODE_RIGHT 261


unsigned char ql_ipc_last_write_value=0;

//Ultimo comando recibido
unsigned char ql_ipc_last_command=0;

//Alterna bit ready o no leyendo el serial bit de ipc
int ql_ipc_reading_bit_ready=0;

//Ultimo parametro de comando recibido
unsigned char ql_ipc_last_command_parameter;

int ql_ipc_last_write_bits_enviados=0;
int ql_estado_ipc=QL_STATUS_IPC_IDLE;
int ql_ipc_bytes_received=0;

unsigned char ql_ipc_last_nibble_to_read[32];
int ql_ipc_last_nibble_to_read_mascara=8;
int ql_ipc_last_nibble_to_read_index=0;
int ql_ipc_last_nibble_to_read_length=1;


moto_int ql_current_sound_duration=0;

//unsigned char ql_audio_pitch=0;
//unsigned char ql_audio_pitch_counter=0;

moto_int ql_audio_pitch=0;
moto_int ql_audio_pitch_counter=0;


int ql_audio_output_bit=0;
int ql_audio_playing=0;



//Para gestionar repeticiones
int ql_mantenido_pulsada_tecla=0;
int ql_mantenido_pulsada_tecla_timer=0;
//adicionales
int ql_pressed_backspace=0;

//ql_keyboard_table[0] identifica a fila 7 F4     F1      5     F2     F3     F5      4      7
//...
//ql_keyboard_table[7] identifica a fila 0 Shift   Ctrl    Alt      x      v      /      n      ,

//Bits se cuentan desde la izquierda:

// 1      2      4     8      16     32      64    128
//F4     F1      5     F2     F3     F5      4      7

unsigned char ql_keyboard_table[8]={
	255,
	255,
	255,
	255,
	255,
	255,
	255,
	255
};



//Nota: esta matrix de teclas y su numeracion de cada fila está documentada erroneamente en la info del QL de manera ascendente (de 0 a 7),
//mientras que lo correcto, cuando se habla de filas en resultado de comandos ipc, es descendente, tal y como está a continuación:

// ================================== matrix ============================
//        0      1      2      3      4      5      6      7
//  +-------------------------------------------------------
// 7|    F4     F1      5     F2     F3     F5      4      7     ql_keyboard_table[0]
// 6|   Ret   Left     Up    Esc  Right      \  Space   Down     ql_keyboard_table[1]
// 5|     ]      z      .      c      b  Pound      m      '     ql_keyboard_table[2]
// 4|     [   Caps      k      s      f      =      g      ;     ql_keyboard_table[3]
// 3|     l      3      h      1      a      p      d      j     ql_keyboard_table[4]
// 2|     9      w      i    Tab      r      -      y      o     ql_keyboard_table[5]
// 1|     8      2      6      q      e      0      t      u     ql_keyboard_table[6]
// 0| Shift   Ctrl    Alt      x      v      /      n      ,     ql_keyboard_table[7]


//Retorna fila y columna para una tecla pulsada mirando ql_keyboard_table. No se puede retornar por ahora mas de una tecla a la vez
//Se excluye shift, ctrl y alt de la respuesta
//Retorna fila -1 y columna -1 si ninguna tecla pulsada

void ql_return_columna_fila_puertos(int *columna,int *fila)
{

	int c,f; 
	c=-1;
	f=-1;

	int i;
	int rotacion;

	unsigned char valor_puerto;
	int salir=0;
	for (i=0;i<8 && salir==0;i++){
		valor_puerto=ql_keyboard_table[i];
		//Si shift ctrl y alt quitarlos
		if (i==7) valor_puerto |=1+2+4;

		//Ver si hay alguna tecla pulsada

		for (rotacion=0;rotacion<8 && salir==0;rotacion++) {
			if ((valor_puerto&1)==0) {
				c=rotacion;
				f=7-i;
				salir=1;
				//printf ("c: %d f: %d\n",c,f);
			}
			else {
				valor_puerto=valor_puerto>>1;
			}
		}
	}

	*columna=c;
	*fila=f;
}


//Mete en valores ipc de vuelta segun teclas pulsadas
/*Desde la rom, cuando se genera una PC_INTR, se pone a leer de ipc lo siguiente y luego ya llama a write_ipc comando 8:


Read PC_INTR pulsado tecla
Returning ipc: 0H. Index nibble: 0 mask: 1
Returning ipc: 0H. Index nibble: 0 mask: 8
Returning ipc: 80H. Index nibble: 0 mask: 4
Write ipc command 1 pressed key
Returning ipc: 80H. Index nibble: 0 mask: 8
Returning ipc: 80H. Index nibble: 0 mask: 4
Returning ipc: 80H. Index nibble: 0 mask: 2
Returning ipc: 80H. Index nibble: 0 mask: 1

Returning ipc: 80H. Index nibble: 0 mask: 8
Returning ipc: 80H. Index nibble: 0 mask: 4
Returning ipc: 80H. Index nibble: 0 mask: 2
Returning ipc: 80H. Index nibble: 0 mask: 1

Returning ipc: 80H. Index nibble: 0 mask: 8
Returning ipc: 80H. Index nibble: 0 mask: 4
Returning ipc: 80H. Index nibble: 0 mask: 2
Returning ipc: 80H. Index nibble: 0 mask: 1

Returning ipc: 80H. Index nibble: 0 mask: 8
Returning ipc: 80H. Index nibble: 0 mask: 4
Returning ipc: 80H. Index nibble: 0 mask: 2
Returning ipc: 80H. Index nibble: 0 mask: 1

Returning ipc: 80H. Index nibble: 0 mask: 8
QL Trap ROM: Tell one key pressed
Returning ipc: 80H. Index nibble: 0 mask: 4
Returning ipc: 80H. Index nibble: 0 mask: 2
Returning ipc: 80H. Index nibble: 0 mask: 1


--Aparentemente estas lecturas anteriores no afectan al valor de la tecla

Lectura de teclado comando. PC=00002F8EH
letra: a
no repeticion


*/


//int temp_flag_reves=0;
void ql_ipc_write_ipc_teclado(void)
{
	/*
	* This returns one nibble, plus up to 7 nibble/byte pairs:
	* first nibble, ms bit: set if final last keydef is still held
	* first nibble, ls 3 bits: count of keydefs to follow.
	* then, for each of the 0..7 keydefs:
	* nibble, bits are 3210=lsca: lost keys (last set only), shift, ctrl and alt.
	* byte, bits are 76543210=00colrow: column and row as keyrow table.
	* There is a version of the IPC used on the thor that will also return keydef
	* values for a keypad. This needs looking up
	*/

	/*Devolveremos una tecla. Esto es:
	* first nibble, ms bit: set if final last keydef is still held
	* first nibble, ls 3 bits: count of keydefs to follow.
	valor 1
	//nibble, bits are 3210=lsca: lost keys (last set only), shift, ctrl and alt.
	valor 0
	//byte, bits are 76543210=00colrow: column and row as keyrow table
// ================================== matrix ============================
//        0      1      2      3      4      5      6      7
//  +-------------------------------------------------------
// 7|    F4     F1      5     F2     F3     F5      4      7
// 6|   Ret   Left     Up    Esc  Right      \  Space   Down
// 5|     ]      z      .      c      b  Pound      m      '
// 4|     [   Caps      k      s      f      =      g      ;
// 3|     l      3      h      1      a      p      d      j
// 2|     9      w      i    Tab      r      -      y      o
// 1|     8      2      6      q      e      0      t      u
// 0| Shift   Ctrl    Alt      x      v      /      n      ,


	//F1 es columna 1, row 0
	valor es 7654=00co=0000=0
	valor es lrow=1000 =8
	col=001 row=000
	00 001 000

	*/


	int columna;
	int fila;


int i;
	//Si tecla no pulsada
	//if ((puerto_49150&1)) {
	if (!ql_pulsado_tecla()) {
		for (i=0;i<ql_ipc_last_nibble_to_read_length;i++) ql_ipc_last_nibble_to_read[i]=0;
	}


ql_return_columna_fila_puertos(&columna,&fila);
int tecla_shift=0;
int tecla_control=0;
int tecla_alt=0;


if ( (columna>=0 && fila>=0) || ql_pressed_backspace) {
	if (ql_mantenido_pulsada_tecla==0 || (ql_mantenido_pulsada_tecla==1 && ql_mantenido_pulsada_tecla_timer>=50) )  {
		if (ql_mantenido_pulsada_tecla==0) {
			ql_mantenido_pulsada_tecla=1;
			ql_mantenido_pulsada_tecla_timer=0;
		}


		if (ql_pressed_backspace) {
			//CTRL + flecha izquierda
			tecla_control=1;
			// 6|   Ret   Left     Up    Esc  Right      \  Space   Down
			fila=6;
			columna=1;

		}

		//printf ("------fila %d columna %d\n",fila,columna);
		unsigned char byte_tecla=((fila&7)<<3) | (columna&7);



		ql_ipc_last_nibble_to_read[2]=(byte_tecla>>4)&15;
		ql_ipc_last_nibble_to_read[3]=(byte_tecla&15);

		if ((ql_keyboard_table[7]&1)==0) tecla_shift=1;
		if ((ql_keyboard_table[7]&2)==0) tecla_control=1;
		if ((ql_keyboard_table[7]&4)==0) tecla_alt=1;


		ql_ipc_last_nibble_to_read[1]=0;  //lsca
		if (tecla_shift) ql_ipc_last_nibble_to_read[1] |=4;
		if (tecla_control) ql_ipc_last_nibble_to_read[1] |=2;
		if (tecla_alt) ql_ipc_last_nibble_to_read[1] |=1;


	}
	else {
		//debug_printf (VERBOSE_PARANOID,"Repeating key");
		ql_ipc_last_nibble_to_read[0]=ql_ipc_last_nibble_to_read[1]=ql_ipc_last_nibble_to_read[2]=ql_ipc_last_nibble_to_read[3]=ql_ipc_last_nibble_to_read[4]=ql_ipc_last_nibble_to_read[5]=0;
	}
}
else {
	//debug_printf (VERBOSE_PARANOID,"Unknown key");
	ql_mantenido_pulsada_tecla=0;
	ql_ipc_last_nibble_to_read[0]=ql_ipc_last_nibble_to_read[1]=ql_ipc_last_nibble_to_read[2]=ql_ipc_last_nibble_to_read[3]=ql_ipc_last_nibble_to_read[4]=0;
}


				ql_ipc_last_nibble_to_read_mascara=8;
				ql_ipc_last_nibble_to_read_index=0;
				ql_ipc_last_nibble_to_read_length=5; //5;

					//printf ("Ultimo pc_intr: %d\n",temp_pcintr);

				for (i=0;i<ql_ipc_last_nibble_to_read_length;i++) {
					//debug_printf (VERBOSE_PARANOID,"Return IPC values:[%d] = %02XH",i,ql_ipc_last_nibble_to_read[i]);
				}

}





void ql_ipc_write_ipc_read_keyrow(int row)
{

/*
kbdr_cmd equ    9       keyboard direct read
* kbdr_cmd requires one nibble which selects the row to be read.
* The top bit of this is ignored (at least on standard IPC's...).
* It responds with a byte whose bits indicate which of the up to eight keys on
* the specified row of the keyrow table are held down. */

//De momento nada

	//unsigned char temp_resultado=0;

	//if (ql_pulsado_tecla()) temp_resultado++;
	unsigned char resultado_row;

	resultado_row=ql_keyboard_table[row&7] ^ 255;
	//Bit a 1 para cada tecla pulsada
	//row numerando de 0 a 7
	/*
	// ================================== matrix ============================
	//        0      1      2      3      4      5      6      7
	//  +-------------------------------------------------------
	// |    F4     F1      5     F2     F3     F5      4      7
	// |   Ret   Left     Up    Esc  Right      \  Space   Down
	// |     ]      z      .      c      b  Pound      m      '
	// |     [   Caps      k      s      f      =      g      ;
	// |     l      3      h      1      a      p      d      j
	// |     9      w      i    Tab      r      -      y      o
	// |     8      2      6      q      e      0      t      u
	// | Shift   Ctrl    Alt      x      v      /      n      ,
	Por ejemplo, para leer si se pulsa Space, tenemos que leer row 1, y ver luego si bit 6 está a 1 (40H)
	*/

	if (zxvision_key_not_sent_emulated_mach() )  resultado_row=255;

	debug_printf (VERBOSE_PARANOID,"Reading ipc command 9: read keyrow. row %d returning %02XH",row,resultado_row);

		ql_ipc_last_nibble_to_read[0]=(resultado_row>>4)&15;
		ql_ipc_last_nibble_to_read[1]=resultado_row&15;
			ql_ipc_last_nibble_to_read_mascara=8;
			ql_ipc_last_nibble_to_read_index=0;
			ql_ipc_last_nibble_to_read_length=2;


}



void ql_ipc_reset(void)
{
	ql_ipc_last_write_value=0;
	ql_ipc_last_write_bits_enviados=0;
	ql_estado_ipc=QL_STATUS_IPC_IDLE;
	ql_ipc_last_command=0;
	ql_ipc_bytes_received=0;
	ql_ipc_last_nibble_to_read_mascara=8;
	ql_ipc_last_nibble_to_read_index=0;
	ql_ipc_last_nibble_to_read_length=1;
	ql_ipc_reading_bit_ready=0;
}




//unsigned char temp_read_ipc;
unsigned char ql_read_ipc(void)
{


//Temporal
//temp_read_ipc ^=64;

	unsigned char valor_retorno=0;

	//printf ("Valor temporal reading ipc: %d\n",ql_ipc_last_nibble_to_read[0]);
/*
        ql_ipc_last_nibble_to_read_index;
        ql_ipc_last_nibble_to_read_length;
*/

	//Ir alternando valor retornado
	if (ql_ipc_reading_bit_ready==0) {
		ql_ipc_reading_bit_ready=1;
		return 0;
	}

	//else ql_ipc_reading_bit_ready=0;



	if (ql_ipc_last_nibble_to_read[ql_ipc_last_nibble_to_read_index]&ql_ipc_last_nibble_to_read_mascara) valor_retorno |=128; //Valor viene en bit 7


	//Solo mostrar este debug si hay tecla pulsada

	//if (ql_pulsado_tecla() )printf ("Returning ipc: %XH. Index nibble: %d mask: %d\n",valor_retorno,ql_ipc_last_nibble_to_read_index,ql_ipc_last_nibble_to_read_mascara);

	if (ql_ipc_last_nibble_to_read_mascara!=1) ql_ipc_last_nibble_to_read_mascara=ql_ipc_last_nibble_to_read_mascara>>1;
	else {

		//Siguiente byte
		ql_ipc_last_nibble_to_read_mascara=8;
		ql_ipc_last_nibble_to_read_index++;
		if (ql_ipc_last_nibble_to_read_index>=ql_ipc_last_nibble_to_read_length) ql_ipc_last_nibble_to_read_index=0; //Si llega al final, dar la vuelta
		//if (ql_ipc_last_nibble_to_read_index>=ql_ipc_last_nibble_to_read_length) ql_ipc_last_nibble_to_read_index=ql_ipc_last_nibble_to_read_length; //dejarlo al final
	}
	//Para no perder nunca el valor. Rotamos mascara


	//if (ql_ipc_last_nibble_to_read_index>1) sleep(2);

	//if (valor_retorno) sleep(1);

	return valor_retorno;
	//return 0;  //De momento eso



	/*
	* Receiving data from the IPC is done by writing %1110 to pc_ipcwr for each bit
* of the data, once again waiting for bit 6 at pc_ipcrd to go to zero, and
* then reading bit 7 there as the data bit. The data is received msb first.
*/
}



int ql_pulsado_tecla(void)
{

	if (zxvision_key_not_sent_emulated_mach() ) return 0;

	//Si backspace
	if (ql_pressed_backspace) return 1;

	unsigned char acumulado;

	acumulado=255;

	int i;
	for (i=0;i<8;i++) acumulado &=ql_keyboard_table[i];

	if (acumulado==255) return 0;
	return 1;
	/*

	acumulado=menu_da_todas_teclas();


					//Hay tecla pulsada
					if ( (acumulado & MENU_PUERTO_TECLADO_NINGUNA) !=MENU_PUERTO_TECLADO_NINGUNA ) {
						return 1;
					}
	return 0;*/
}

//unsigned char temp_stat_cmd;
//unsigned char temp_contador_tecla_pulsada;

//int temp_columna=0;





/*
// ================================== matrix ============================
//        0      1      2      3      4      5      6      7
//  +-------------------------------------------------------
// 7|    F4     F1      5     F2     F3     F5      4      7
// 6|   Ret   Left     Up    Esc  Right      \  Space   Down
// 5|     ]      z      .      c      b  Pound      m      '
// 4|     [   Caps      k      s      f      =      g      ;
// 3|     l      3      h      1      a      p      d      j
// 2|     9      w      i    Tab      r      -      y      o
// 1|     8      2      6      q      e      0      t      u
// 0| Shift   Ctrl    Alt      x      v      /      n      ,
*/

struct x_tabla_columna_fila
{
        int columna;
        int fila;
};

struct x_tabla_columna_fila ql_col_fil_numeros[]={
	{5,1}, //0
	{3,3},
	{1,1}, //2
	{1,3},
	{6,7},  //4
	{2,7},
	{2,1},  //6
	{7,7},
	{0,1}, //8
	{0,2}
};


/*
// ================================== matrix ============================
//        0      1      2      3      4      5      6      7
//  +-------------------------------------------------------
// 7|    F4     F1      5     F2     F3     F5      4      7
// 6|   Ret   Left     Up    Esc  Right      \  Space   Down
// 5|     ]      z      .      c      b  Pound      m      '
// 4|     [   Caps      k      s      f      =      g      ;
// 3|     l      3      h      1      a      p      d      j
// 2|     9      w      i    Tab      r      -      y      o
// 1|     8      2      6      q      e      0      t      u
// 0| Shift   Ctrl    Alt      x      v      /      n      ,
*/
struct x_tabla_columna_fila ql_col_fil_letras[]={
	{4,3}, //A
	{4,5},
	{3,5},
	{6,3},
	{4,1}, //E
	{4,4},
	{6,4},
	{2,3}, //H
	{2,2},
	{7,3},
	{2,4}, //K
	{0,3},
	{6,5},
	{6,0}, //N
	{7,2},
	{5,3},
	{3,1}, //Q
	{4,2},
	{3,4},
	{6,1}, //T
	{7,1},
	{4,0},
	{1,2}, //W
	{3,0},
	{6,2},
	{1,5} //Z
};


//Returna fila y columna para una tecla dada.
void ql_return_fila_columna_tecla(int tecla,int *columna,int *fila)
{
	int c;
	int f;
	int indice;

	//Por defecto
	c=-1;
	f=-1;

	if (tecla>='0' && tecla<='9') {
		indice=tecla-'0';
		c=ql_col_fil_numeros[indice].columna;
		f=ql_col_fil_numeros[indice].fila;
	}

	else if (tecla>='a' && tecla<='z') {
		indice=tecla-'a';
		c=ql_col_fil_letras[indice].columna;
		f=ql_col_fil_letras[indice].fila;
	}

	else if (tecla==32) {
		c=6;
		f=6;
	}

	else if (tecla==10) {
		c=0;
		f=6;
	}

	else if (tecla==QL_KEYCODE_F1) {
		c=1;
		f=7;
	}

	else if (tecla==QL_KEYCODE_F2) {
		c=3;
		f=7;
	}

	else if (tecla=='.') {
		c=2;
		f=5;
	}

	else if (tecla==',') {
		c=7;
		f=0;
	}

	//        0      1      2      3      4      5      6      7
	//  +-------------------------------------------------------
	// 6|   Ret   Left     Up    Esc  Right      \  Space   Down

	else if (tecla==QL_KEYCODE_UP) {
		c=2;
		f=6;
	}

	else if (tecla==QL_KEYCODE_DOWN) {
		c=7;
		f=6;
	}


	else if (tecla==QL_KEYCODE_LEFT) {
		c=1;
		f=6;
	}


	else if (tecla==QL_KEYCODE_RIGHT) {
		c=4;
		f=6;
	}


	*columna=c;
	*fila=f;
}

/*
89l6ihverantyd


*/





//Retorna caracter ascii segun tecla pulsada en ql_keyboard_table
//Miramos segun tabla de tecla a puertos (ql_tabla_teclado_letras)
//NO USADO YA esto
/*
int ql_return_ascii_key_pressed(void)
{
	//temp
	//if ((ql_keyboard_table[4]&1)==0) return 'l';
	//if ((ql_keyboard_table[4]&16)==0) return 'a';

	int letra;
	int indice=0;

	for (letra='a';letra<='z';letra++) {
		if ((*ql_tabla_teclado_letras[indice].puerto & ql_tabla_teclado_letras[indice].mascara)==0) {
			//printf ("letra: %c\n",letra);
			return letra;
		}

		indice++;
	}

	indice=0;
	for (letra='0';letra<='9';letra++) {
		if ((*ql_tabla_teclado_numeros[indice].puerto & ql_tabla_teclado_numeros[indice].mascara)==0) {
			//printf ("numero: %c\n",letra);
			return letra;
		}

		indice++;
	}

	//Otras teclas
	//Enter
	if ((ql_keyboard_table[1]&1)==0) return 10;

	//Punto
	if ((ql_keyboard_table[2]&4)==0) return '.';

	//Coma
	if ((ql_keyboard_table[7]&128)==0) return ',';

	//Espacio
	if ((ql_keyboard_table[1]&64)==0) return 32;


	//F1
	if ((ql_keyboard_table[0]&2)==0) return QL_KEYCODE_F1;

	//F2
	if ((ql_keyboard_table[0]&8)==0) return QL_KEYCODE_F2;

// 1|   Ret   Left     Up    Esc  Right      \  Space   Down
	if ((ql_keyboard_table[1]&4)==0) return QL_KEYCODE_UP;

	if ((ql_keyboard_table[1]&128)==0) return QL_KEYCODE_DOWN;

	if ((ql_keyboard_table[1]&2)==0) return QL_KEYCODE_LEFT;

	if ((ql_keyboard_table[1]&16)==0) return QL_KEYCODE_RIGHT;


	return 0;
}

*/



void ql_stop_sound(void)
{
    			//de momento solo tonos
			//unsigned char valor_mixer=255;


		
			//printf ("set mixer chip ay 0. tonos: %02XH ruidos: %02XH final: %02XH\n",mixer_tonos,mixer_ruido,valor_mixer);

            /*
			ay_chip_selected=0;
			out_port_ay(65533,7);
			out_port_ay(49149,valor_mixer);    
            */
            ql_audio_playing=0;
}


void ql_audio_next_cycle(void)
{

            //Decrementamos contador sonido si conviene
            //Si es 0, dejarlo tal cual
            if (ql_current_sound_duration!=0) {
                ql_current_sound_duration--;
                if (ql_current_sound_duration==0) {
                    //Silenciar
                    printf("stop sound\n");
                    ql_stop_sound();
                }
            }
}


char ql_audio_da_output(void)
{
    ql_audio_pitch_counter--;
    if (ql_audio_pitch_counter==0) {
        ql_audio_pitch_counter=ql_audio_pitch;
        ql_audio_output_bit ^=1;
    }

    if (!ql_audio_playing) return 0;

    return ql_audio_output_bit * 30;
}


void ql_simulate_sound(unsigned char pitch1,moto_int duration)
{
    			//de momento solo tonos
                /*
			unsigned char valor_mixer=255;


			valor_mixer &=(255-1); //Canal 0 tono
			
			//printf ("set mixer chip ay 0. tonos: %02XH ruidos: %02XH final: %02XH\n",mixer_tonos,mixer_ruido,valor_mixer);

			ay_chip_selected=0;
			out_port_ay(65533,7);
			out_port_ay(49149,valor_mixer);

            //volumen

            out_port_ay(65533,8);
			out_port_ay(49149,15);

            //Tono
            //RO � Ajuste fino del tono, canal A
            //R1 � Ajuste aproximado del tono, canal A- (4 bits)
            //En AY: valor mas alto en el chip, frecuencia mas alta
            //igual que ql: pitch       0,255: pitch 1 is high, 255 is low
            */

           /*
    unsigned char frecuencia=pitch1;

	out_port_ay(65533,0);
	out_port_ay(49149,(frecuencia << 4) & 0xF0); //Aqui los 4 bits bajos

	out_port_ay(65533,1);
	out_port_ay(49149,(frecuencia>>4) & 0xF );  //Y aqui los 4 bits altos     
*/

 
}

//8 bloques de 4 bits. MSB first
unsigned char ql_ipc_sound_command_buffer[8*2];


void ql_ipc_set_sound_parameters(void) 
{
							
    /*
    BEEP syntax:

    duration    -32768,32768: duration in units of 72 microseconds. duration of 0 will run the sound until terminated by another beep command
    pitch       0,255: pitch 1 is high, 255 is low
    pitch2      0,255: a second pitch level between which the sound will "bounce"
    grad_x      -32768,32767: time interval between pitch steps
    grad_y      -8,7: size of each step. grad_x and grad_y control the rate at which the pitch bounces between levels
    wrap        0,15: will force the sound to wrap around the specified number of times. if wrap is equal to 15 the sound will grap around forever
    fuzzy       0,15: defined the amount of fuzziness to be added to the sound
    random      0,15: defined the amount of randomness to be added to the sound

    Formato del mensaje ipc:

    8 bits pitch 1
    8 bits pitch 2
    16 bits  interval between steps (grad_x)
    16 bits duration
    4 bits step in pitch (grad_y)
    4 bits wrap
    4 bits randomness of step
    4 bits fuzziness

    no reply

    Para aproximar, cada "duration" es un scanline


    */

    unsigned char pitch1;
    unsigned char pitch2;
    moto_int interval_steps;
    moto_int duration;
    unsigned char step_in_pitch;
    unsigned char wrap;
    unsigned char randomness_of_step;
    unsigned char fuziness;

    pitch1=(ql_ipc_sound_command_buffer[0]<<4)|ql_ipc_sound_command_buffer[1];
    pitch2=(ql_ipc_sound_command_buffer[2]<<4)|ql_ipc_sound_command_buffer[3];

    //OJO a como se ordena esto:
    interval_steps=(ql_ipc_sound_command_buffer[6]<<12)|(ql_ipc_sound_command_buffer[7]<<8)|
                    (ql_ipc_sound_command_buffer[4]<<4)|ql_ipc_sound_command_buffer[5];


    //OJO a como se ordena esto:
    duration=(ql_ipc_sound_command_buffer[10]<<12)|(ql_ipc_sound_command_buffer[11]<<8)|
            (ql_ipc_sound_command_buffer[8]<<4)|ql_ipc_sound_command_buffer[9];

    step_in_pitch=ql_ipc_sound_command_buffer[12];
    wrap=ql_ipc_sound_command_buffer[13];
    randomness_of_step=ql_ipc_sound_command_buffer[14];
    fuziness=ql_ipc_sound_command_buffer[15];
             

    printf("pitch1 %d pitch2 %d interval_steps %d duration %d step_in_pitch %d wrap %d randomness_of_step %d fuziness %d\n",
    pitch1,pitch2,interval_steps,duration,step_in_pitch,wrap,randomness_of_step,fuziness);

    ql_simulate_sound(pitch1,duration);


    ql_current_sound_duration=duration;    

    ql_audio_pitch=pitch1;

    //Ajuste a ojo dividir entre 1.5. Solo si es mayor que 2
    if (ql_audio_pitch>2) {
        ql_audio_pitch=(ql_audio_pitch*2)/3;
    }

    ql_audio_pitch_counter=ql_audio_pitch;

    ql_audio_playing=1;

    //sleep (5);
}



void ql_write_ipc(unsigned char Data)
{
	/*
	* Commands and data are sent msb first, by writing a byte containg %11x0 to
	* location pc_ipcwr ($18023), where the "x" is one data bit. Bit 6 at location
	* pc_ipcrd ($18020) is then examined, waiting for it to go zero to indicate
	* that the bit has been received by the IPC.
	*/
	/*
	* Receiving data from the IPC is done by writing %1110 to pc_ipcwr for each bit
* of the data, once again waiting for bit 6 at pc_ipcrd to go to zero, and
* then reading bit 7 there as the data bit. The data is received msb first.
	*/

	//Si dato tiene formato: 11x0  (8+4+1)
	if ((Data&13)!=12) return;

	int bitdato=(Data>>1)&1;
	//printf ("Escribiendo bit ipc: %d. bits enviados: %d\n",bitdato,ql_ipc_last_write_bits_enviados);
	ql_ipc_last_write_value=ql_ipc_last_write_value<<1;
	ql_ipc_last_write_value |=bitdato;
	ql_ipc_last_write_bits_enviados++;
	if (ql_ipc_last_write_bits_enviados==4) {
			switch (ql_estado_ipc) {
			  case QL_STATUS_IPC_IDLE:
					ql_ipc_last_command=ql_ipc_last_write_value&15;
					//printf ("Resultante ipc command: %d (%XH)\n",ql_ipc_last_command,ql_ipc_last_command); //Se generan 4 bits cada vez
					ql_ipc_last_write_bits_enviados=0;

					//Actuar segun comando
					switch (ql_ipc_last_command)
					{


						case 0:
						//*rset_cmd equ    0       resets the IPC software
							ql_ipc_reset();
						break;

						case 1:
/*
stat_cmd equ    1       report input status
* returns a byte, the bits of which are:
ipc..kb equ     0       set if data available in keyboard buffer, or key held
ipc..so equ     1       set if sound is still being generated
*               2       set if kbd shift setting has changed, with key held
*               3       set if key held down
ipc..s1 equ     4       set if input is pending from RS232 channel 1
ipc..s2 equ     5       set if input is pending from RS232 channel 2
ipc..wp equ     6       return state of p26, currently not connected
*               7       was set if serial transfer was being zapped, now zero
*/

							ql_ipc_last_nibble_to_read[0]=0; //ipc..kb equ     0       set if data available in keyboard buffer, or key held


							//Decir tecla pulsada

							//temp
							//temp_stat_cmd++;
							//ql_ipc_last_nibble_to_read[0]=temp_stat_cmd;
							ql_ipc_last_nibble_to_read[0]=15; //Devolver valor entre 8 y 15 implica que acabara leyendo el teclado
						        ql_ipc_last_nibble_to_read_mascara=8;
						        ql_ipc_last_nibble_to_read_index=0;
						        ql_ipc_last_nibble_to_read_length=1;


							//Si tecla no pulsada
							if (!ql_pulsado_tecla()) ql_ipc_last_nibble_to_read[0]=4;

							else {
								//debug_printf (VERBOSE_DEBUG,"Write ipc command 1: Report input status pressed key");
							}
							//if ((puerto_49150&1)) ql_ipc_last_nibble_to_read[0]=4;

							//printf ("Valor a retornar: %d\n",ql_ipc_last_nibble_to_read[0]&15);

                            //Bit de beeping
                            if (ql_audio_playing) ql_ipc_last_nibble_to_read[0] |=2;
                            else ql_ipc_last_nibble_to_read[0] &=(255-2);


							//sleep(1);

						break;

						case 8:

							//debug_printf (VERBOSE_DEBUG,"Write ipc command 8: Read key. PC=%08XH",get_pc_register() );

							ql_ipc_write_ipc_teclado();


						break;

						case 9:
							//debug_printf (VERBOSE_ERR,"Write ipc command 9: Reading keyrow. Not implemented");
							/*
							kbdr_cmd equ    9       keyboard direct read
* kbdr_cmd requires one nibble which selects the row to be read.
* The top bit of this is ignored (at least on standard IPC's...).
* It responds with a byte whose bits indicate which of the up to eight keys on
* the specified row of the keyrow table are held down. */

							ql_estado_ipc=QL_STATUS_IPC_WRITING;


						break;

						case 10:
						/*
						inso_cmd equ    10      initiate sound process
* This requires no less than 64 bits of data. it starts sound generation.
* Note that the 16 bit values below need to have their ls 8 bits sent first!
* 8 bits pitch 1
* 8 bits pitch 2
* 16 bits interval between steps
* 16 bits duration (0=forever)
* 4 bits signed step in pitch
* 4 bits wrap
* 4 bits randomness (none unless msb is set)
* 4 bits fuziness (none unless msb is set)
*/
							debug_printf (VERBOSE_PARANOID,"ipc command 10 inso_cmd initiate sound process");
                            printf ("ipc command 10 inso_cmd initiate sound process\n");
							//sleep(5);
                            
                            ql_estado_ipc=QL_STATUS_IPC_WRITING; //Mejor lo desactivo porque si no se queda en estado writing y no sale de ahi
						break;

						//baud_cmd
						case 13:
						/*
						baud_cmd equ    13      change baud rate
* This expects one nibble, of which the 3 lsbs select the baud rate for both
* serial channels. The msb is ignored. Values 7 down to zero correspond to baud
* rates 75/300/600/1200/2400/4800/9600/19200.
* The actual clock rate is supplied from the PC to the IPC, but this command is
* also needed in the IPC for timing out transfers!
						*/
						ql_estado_ipc=QL_STATUS_IPC_WRITING;
						break;


						case 15:
							//El que se acaba enviando para leer del puerto ipc. No hacer nada
						break;

						default:
							//debug_printf (VERBOSE_ERR,"Write ipc command %d. Not implemented",ql_ipc_last_command);
						break;
					}
				break;


			case QL_STATUS_IPC_WRITING:
				ql_ipc_last_command_parameter=ql_ipc_last_write_value&15;
				//printf ("Parametro recibido de ultimo comando %d: %d\n",ql_ipc_last_command,ql_ipc_last_command_parameter);
				//Segun ultimo comando
					switch (ql_ipc_last_command) {
						case 9:
						/*
						kbdr_cmd equ    9       keyboard direct read
* kbdr_cmd requires one nibble which selects the row to be read.
* The top bit of this is ignored (at least on standard IPC's...).
* It responds with a byte whose bits indicate which of the up to eight keys on
* the specified row of the keyrow table are held down. */
//ql_ipc_write_ipc_read_keyrow();
							//printf ("Parametro recibido de ultimo comando read keyrow %d: %d\n",ql_ipc_last_command,ql_ipc_last_command_parameter);
							ql_estado_ipc=QL_STATUS_IPC_IDLE;
							ql_ipc_last_write_bits_enviados=0;
							ql_ipc_write_ipc_read_keyrow(ql_ipc_last_command_parameter);


						break;


						case 10:
							debug_printf (VERBOSE_PARANOID,"parameter sound %d: %d",ql_ipc_bytes_received,ql_ipc_last_command_parameter);

                            printf ("parameter sound %d: %d\n",ql_ipc_bytes_received,ql_ipc_last_command_parameter);
                            ql_ipc_sound_command_buffer[ql_ipc_bytes_received]=ql_ipc_last_command_parameter;

							ql_ipc_bytes_received++;
                            ql_ipc_last_write_bits_enviados=0;
                            //sleep(1);

                            //16 bytes (o sea, cada byte me lleva 4 bits efectivos, en total: 8 bytes efectivos)
							if (ql_ipc_bytes_received>=16) {
								debug_printf (VERBOSE_PARANOID,"End receiving ipc parameters");
								ql_estado_ipc=QL_STATUS_IPC_IDLE;
								//ql_ipc_last_write_bits_enviados=0;
								ql_ipc_bytes_received=0;
                                //sleep(10);

                                ql_ipc_set_sound_parameters();
							}
                            /*
                            BEEP syntax:
                            duration    -32768,32768: duration in units of 72 microseconds. duration of 0 will run the sound until terminated by another beep command
                            pitch       0,255: pitch 1 is high, 255 is low
                            pitch2      0,255: a second pitch level between which the sound will "bounce"
                            grad_x      -32768,32767: time interval between pitch steps
                            grad_y      -8,7: size of each step. grad_x and grad_y control the rate at which the pitch bounces between levels
                            wrap        0,15: will force the sound to wrap around the specified number of times. if wrap is equal to 15 the sound will grap around forever
                            fuzzy       0,15: defined the amount of fuzziness to be added to the sound
                            random      0,15: defined the amount of randomness to be added to the sound

                            Formato del mensaje ipc:
                            8 bits pitch 1
                            8 bits pitch2
                            16 bits  interval between steps
                            16 bits duration
                            4 bits step in pitch
                            4 bits wrap
                            4 bits randomness of step
                            4 bits fuzziness

                            no reply


                            */
						break;

						case 13:
							ql_estado_ipc=QL_STATUS_IPC_IDLE;
							//Fin de parametros de comando. Establecer baud rate y dejar status a idle de nuevo para que la siguiente escritura se interprete como comando inicial
						break;
					}
			break;
			}
			//sleep(2);
	}


}