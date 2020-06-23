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

/*



CV's sound chip
---------------

The sound chip in the CV is a SN76489AN manufactured by Texas Instruments.
It has 4 sound channels- 3 tone and 1 noise.

The volume of each channel can be controlled seperately in 16 steps from
full volume to silence.

A byte written into the sound chip determines which register is used, along
with the frequency/ attenuation information.

The frequency of each channel is represented by 10 bits.  10 bits won't
fit into 1 byte, so the data is written in as 2 bytes.

Here's the control word:

+--+--+--+--+--+--+--+--+
|1 |R2|R1|R0|D3|D2|D1|D0|
+--+--+--+--+--+--+--+--+

1: This denotes that this is a control word
R2-R0 the register number:

000 Tone 1 Frequency
001 Tone 1 Volume
010 Tone 2 Frequency
011 Tone 2 Volume
100 Tone 3 Frequency
101 Tone 3 Volume
110 Noise Control
111 Noise Volume

D3-D0 is the data

Here's the second frequency register:

+--+--+--+--+--+--+--+--+
|0 |xx|D9|D8|D7|D6|D5|D4|
+--+--+--+--+--+--+--+--+

0: This denotes that we are sending the 2nd part of the frequency

D9-D4 is 6 more bits of frequency 


To write a 10-bit word for frequenct into the sound chip you must first
send the control word, then the second frequency register.  Note that the
second frequency register doesn't have a register number.  When you write
to it, it uses which ever register you used in the control word.

So, if we want to output 11 0011 1010b to tone channel 1:

First, we write the control word:

LD A,1000 1010b
OUT (F0h),A

Then, the second half of the frequency:

LD A,0011 0011b
OUT (F0h),A

To tell the frequency of the wave generated, use this formula:


   3579545
f= -------
     32n

Where f= frequency out,
and n= your 10-bit binary number in


To control the Volume:


+--+--+--+--+--+--+--+--+
|1 |R2|R1|R0|V3|V2|V1|V0|
+--+--+--+--+--+--+--+--+

R2-R0 tell the register

V3-V0 tell the volume:

0000=Full volume
.
.
.
1111=Silence


The noise source is quite intresting.  It has several modes of operation.
Here's a control word:

+--+--+--+--+--+--+--+--+
|1 |1 |1 |0 |xx|FB|M1|M0|
+--+--+--+--+--+--+--+--+

FB= Feedback:

0= 'Periodic' noise
1= 'white' noise 

The white noise sounds, well, like white noise.
The periodic noise is intresting.  Depending on the frequency, it can
sound very tonal and smooth.

M1-M0= mode bits:

00= Fosc/512  Very 'hissy'; like grease frying
01= Fosc/1024 Slightly lower
10= Fosc/2048 More of a high rumble
11= output of tone generator #3

You can use the output of gen. #3 for intresting effects.  If you sweep
the frequency of gen. #3, it'll cause a cool sweeping effect of the noise.
The usual way of using this mode is to attenuate gen. #3, and use the
output of the noise source only.

The attenuator for noise works in the same way as it does for the other
channels.



*/

#include <stdio.h>
//#include <math.h>


#include "sn76489an.h"
#include "cpu.h"
#include "audio.h"
#include "debug.h"
#include "joystick.h"


//Indica si esta presente el chip o no
z80_bit sn_chip_present;


//Por defecto la frecuencia del spectrum
int sn_chip_frequency=FRECUENCIA_SPECTRUM_SN;

//#define FRECUENCIA_SPECTRUM_SN 1773400
//#define FRECUENCIA_CPC_SN      1000000
//#define FRECUENCIA_ZX81_SN     1625000



//Valores de volumen
//Al reves:
/*
0000=Full volume
.
.
.
1111=Silence
*/
char sn_volume_table[16]={24,20,16,15,
			   10,8,6,4,
			   3,2,2,1,
			   1,1,0,0};			   


//Este es bit enviado cuando tanto tono como ruido son 0.... Esto permite speech, como en chase hq
//si lo ponemos a 0, no se oye nada
z80_bit sn_speech_enabled;



//una onda oscila 2 veces de signo en su frecuencia
#define TEMP_MULTIPLICADOR 1


z80_byte sn_volumen_canal_ruido=0;

//valores de 16 bits con signo
//tempp  short sn_sine_table[FRECUENCIA_CONSTANTE_NORMAL_SONIDO];

short sn_sine_table[FRECUENCIA_CONSTANTE_NORMAL_SONIDO];

//tabla suficientemente grande como para que al aumentar la frecuencia del sonido no se salga de rango
//soportar hasta cpu 1000% (10 veces msnor)
//short sn_sine_table[15600*10];



//z80_bit turbosound_enabled={0};

//int total_sn_chips=1;

//Chip de sonido activo 
int sn_chip_selected=0;

//
//Variables que dependen del chip activo
//
//16 BYTES Contenido de los registros del chip de sonido
z80_byte sn_3_8912_registros[MAX_SN_CHIPS][16];

//Ultimo registro seleccionado por el puerto 65533
z80_byte sn_3_8912_registro_sel[MAX_SN_CHIPS];




//frecuencia de cada canal
int sn_freq_tono_A[MAX_SN_CHIPS],sn_freq_tono_B[MAX_SN_CHIPS],sn_freq_tono_C[MAX_SN_CHIPS];

//contador de cada canal... (FRECUENCIA_CONSTANTE_NORMAL_SONIDO/freq_tono)
int sn_contador_tono_A[MAX_SN_CHIPS],sn_contador_tono_B[MAX_SN_CHIPS],sn_contador_tono_C[MAX_SN_CHIPS];

//ultimo valor enviado para cada canal, valores con signo:
short sn_ultimo_valor_tono_A[MAX_SN_CHIPS];
short sn_ultimo_valor_tono_B[MAX_SN_CHIPS];
short sn_ultimo_valor_tono_C[MAX_SN_CHIPS];



//frecuencia de canal de ruido
int sn_freq_ruido[MAX_SN_CHIPS];

//contador de canal de ruido .... (FRECUENCIA_CONSTANTE_NORMAL_SONIDO/sn_freq_ruido)
int sn_contador_ruido[MAX_SN_CHIPS];

//ultimo valor enviado para canal de ruido. valor con signo:
short sn_ultimo_valor_ruido[MAX_SN_CHIPS];

//valor randomize
z80_int sn_randomize_noise[MAX_SN_CHIPS];

//
//Fin variables que dependen del chip activo
//






//Si hsn que autoactivar chip SN en el caso que alguien lo use (lectura o escritura en el puerto SN)
z80_bit autoenable_sn_chip={1};

void init_chip_sn(void)
{


	//inicializamos el chip aunque este desactivado
	//if (sn_chip_present.v==0) return;

	debug_printf (VERBOSE_INFO,"Initializing SN Chip");

	sn_chip_selected=0;


	//resetear valores de cada chip
	int chip;

	for (chip=0;chip<MAX_SN_CHIPS;chip++) {

		//resetear valores de puertos de sonido
		int r;
		for (r=0;r<16;r++) sn_3_8912_registros[chip][r]=255;



		//ultimo valor enviado para cada canal, valores con signo:
		sn_ultimo_valor_tono_A[chip]=+32767;
		sn_ultimo_valor_tono_B[chip]=+32767;
		sn_ultimo_valor_tono_C[chip]=+32767;

		

		sn_ultimo_valor_ruido[chip]=+32767;
	}

int i;



// Onda cuadrada
	for (i=0;i<FRECUENCIA_CONSTANTE_NORMAL_SONIDO/2;i++) {
		sn_sine_table[i]=+32767;
	}
	for (;i<FRECUENCIA_CONSTANTE_NORMAL_SONIDO;i++) {
		sn_sine_table[i]=-32767;
	}

	//Establecemos frecuencia
	if (MACHINE_IS_CPC) sn_chip_frequency=FRECUENCIA_CPC_SN;
	else if (MACHINE_IS_ZX8081) sn_chip_frequency=FRECUENCIA_ZX81_SN;
	else if (MACHINE_IS_MSX) sn_chip_frequency=FRECUENCIA_MSX_SN;
	else sn_chip_frequency=FRECUENCIA_SPECTRUM_SN;

	debug_printf (VERBOSE_INFO,"Setting SN chip frequency to %d HZ",sn_chip_frequency);




//Onda senoidal. activar -lm en proceso de compilacion
/*
float sineval,radians;
	for (i=0;i<FRECUENCIA_CONSTANTE_NORMAL_SONIDO;i++) {
		radians=i;
		radians=radians*6.28318530718;

		radians=radians/((float)(FRECUENCIA_CONSTANTE_NORMAL_SONIDO));
		sineval=sin(radians);
		sn_sine_table[i]=32767*sineval;

		debug_printf (VERBOSE_DEBUG,"i=%d radians=%f sine=%f value=%d",i,radians,sineval,sn_sine_table[i]);
	}

*/


}





void sn_randomize(int chip)
{
/*
;Seguimos la misma formula RND del spectrum:
;0..1 -> n=(75*(n+1)-1)/65536
;0..65535 -> n=65536/(75*(n+1)-1)
generar_random_noise:
*/

        int resultado;
        int r;

        r=sn_randomize_noise[chip];

        resultado=(75*(r+1)-1);

        sn_randomize_noise[chip]=resultado & 0xFFFF;

        //printf ("sn_randomize_noise: %d\n",sn_randomize_noise);


}



//Generar salida aleatoria
void sn_chip_valor_aleatorio(int chip)
{

	sn_randomize(chip);

/*
          ;Generar +1 o -1
          ;0..32767 -> +1
          ;32768..65535 -> -1
  */

	if (sn_randomize_noise[chip]<32768) sn_ultimo_valor_ruido[chip]=+32767;
	else sn_ultimo_valor_ruido[chip]=-32767;

	//printf ("Cambio ruido a : %d\n",sn_ultimo_valor_ruido);

}



//Devuelve la salida del canal indicado, devuelve valor con signo
char sn_da_output_canal(z80_byte mascara,short ultimo_valor_tono,z80_byte volumen,int chip)
{

	//valor con signo
	char valor8;
	int valor;

/*
COMMENT !
    The noise and tone output of a channel is combined in the mixer in the
    following wsn:

        Output_A = (Tone_A OR Ta) AND (Noise OR Na)

    Here Tone_A is the binary output of tone generator A, and Noise is the
    binary output of the noise generator.  Note that setting both Ta and Na
    to 1 produces a constant 1 as output.  Also note that setting both Ta
    and Na to 0 produces bursts of noise and half-periods of constant
    output 0.
( Tone_a OR 0 ) AND ( Noise_a OR 0 )= Tone_a AND Noise_a
( Tone_a OR 0 ) AND ( Noise_a OR 1 )= Tone_a
( Tone_a OR 1 ) AND ( Noise_a OR 0 )= Noise_a
( Tone_a OR 1 ) AND ( Noise_a OR 1 )= 1
!
*/

	z80_bit tone,noise;

//	printf ("ultimo_valor_tono: %d\n",ultimo_valor_tono);

	tone.v=!(sn_retorna_mixer_register(chip) & mascara & 7);
	noise.v=!(sn_retorna_mixer_register(chip) & mascara & (8+16+32));

	if (tone.v==1 && noise.v==0)  {
		valor=ultimo_valor_tono;
		silence_detection_counter=0;
	}

	else if (tone.v==0 && noise.v==1)  {
                valor=sn_ultimo_valor_ruido[chip];
                silence_detection_counter=0;
        }

	else if (tone.v==1 && noise.v==1)  {
		/*
		 Also note that setting both Ta
    and Na to 0 produces bursts of noise and half-periods of constant
    output 0.
*/
		//Valor combinado ruido y tono
		//en version 1.0 este /2 no estaba, era un error, por tanto se generaba al final un volumen msnor de lo normal,
		//cosa que podia hacer que el valor final del sonido cambiase de signo,
		//provocando ruido mas alto de lo normal
		valor=(sn_ultimo_valor_ruido[chip]+ultimo_valor_tono)/2;

                silence_detection_counter=0;
		//printf ("tone y noise. valor:%d\n",valor);

        }

	else {
		//Canales desactivados
		//Parece que deberiamos devolver 0, pero no, devuelve 1
		//esto permite generar sintetizacion de voz/sonido, etc en juegos como Chase HQ por ejemplo, Dizzy III
		//valor=1;
		valor=sn_speech_enabled.v*32767;
	}




	volumen=volumen & 15; //Evitar valores de volumen fuera de rango que vengan de los registros de volumen

	//if (volumen>15) printf ("  Error volumen >15 : %d\n",volumen);
        valor=valor*sn_volume_table[volumen];
	valor=valor/32767;
	valor8=valor;
	//printf ("valor final tono: %d\n",valor8);

	//if (valor8>24) printf ("valor final tono: %d\n",valor8);
	//if (valor8<-24) printf ("valor final tono: %d\n",valor8);

	return valor8;


}



//Devuelve la salida del canal de ruido, devuelve valor con signo
char sn_da_output_canal_ruido(void)
{

	//valor con signo
	char valor8;
	int valor;

	z80_byte volumen=sn_volumen_canal_ruido;

/*
COMMENT !
    The noise and tone output of a channel is combined in the mixer in the
    following wsn:

        Output_A = (Tone_A OR Ta) AND (Noise OR Na)

    Here Tone_A is the binary output of tone generator A, and Noise is the
    binary output of the noise generator.  Note that setting both Ta and Na
    to 1 produces a constant 1 as output.  Also note that setting both Ta
    and Na to 0 produces bursts of noise and half-periods of constant
    output 0.
( Tone_a OR 0 ) AND ( Noise_a OR 0 )= Tone_a AND Noise_a
( Tone_a OR 0 ) AND ( Noise_a OR 1 )= Tone_a
( Tone_a OR 1 ) AND ( Noise_a OR 0 )= Noise_a
( Tone_a OR 1 ) AND ( Noise_a OR 1 )= 1
!
*/

	z80_bit tone,noise;

//	printf ("ultimo_valor_tono: %d\n",ultimo_valor_tono);





	
                valor=sn_ultimo_valor_ruido[0];
                silence_detection_counter=0;
        








	volumen=volumen & 15; //Evitar valores de volumen fuera de rango que vengan de los registros de volumen

	//if (volumen>15) printf ("  Error volumen >15 : %d\n",volumen);
    valor=valor*sn_volume_table[volumen];
	valor=valor/32767;
	valor8=valor;
	//printf ("valor final tono: %d\n",valor8);

	//if (valor8>24) printf ("valor final tono: %d\n",valor8);
	//if (valor8<-24) printf ("valor final tono: %d\n",valor8);

	return valor8;


}

//Devuelve el sonido de salida del chip de los 3 canales
char da_output_sn(void)
{


        //char valor_enviar_sn=0;
        int valor_enviar_sn=0;
	if (sn_chip_present.v==1) {

		//Hacerlo para cada chip
		int chips=sn_retorna_numero_chips();

		int i;

		for (i=0;i<chips;i++) {

			valor_enviar_sn +=sn_da_output_canal(1+8,sn_ultimo_valor_tono_A[i],sn_3_8912_registros[i][8],i);
			valor_enviar_sn +=sn_da_output_canal(2+16,sn_ultimo_valor_tono_B[i],sn_3_8912_registros[i][9],i);
			valor_enviar_sn +=sn_da_output_canal(4+32,sn_ultimo_valor_tono_C[i],sn_3_8912_registros[i][10],i);


			valor_enviar_sn +=sn_da_output_canal_ruido();


		}

		//Dividir valor restante entre numero de chips
		valor_enviar_sn /=chips;
	}


	/*
	if (valor_enviar_sn==0x10) {
		printf ("A %d B %d C %d\n",sn_da_output_canal(1+8,sn_ultimo_valor_tono_A,sn_3_8912_registros[8]),sn_da_output_canal(2+16,sn_ultimo_valor_tono_B,sn_3_8912_registros[9]),sn_da_output_canal(4+32,sn_ultimo_valor_tono_C,sn_3_8912_registros[10]) );
	}
	*/

	return valor_enviar_sn;

}

int sn3_stereo_mode=0;

int sn3_custom_stereo_A=0;
int sn3_custom_stereo_B=1;
int sn3_custom_stereo_C=2;
/*
          0=Mono
          1=ACB Stereo (Canal A=Izq,Canal C=Centro,Canal B=Der)
          2=ABC Stereo (Canal A=Izq,Canal B=Centro,Canal C=Der)
		  3=BAC Stereo (Canal A=Centro,Canal B=Izquierdo,Canal C=Der)
		  4=Custom. Depende de variables 
		  	sn3_custom_stereo_A 
			sn3_custom_stereo_B
			sn3_custom_stereo_C
			En cada una de esas 3, si vale 0=Left. Si 1=Center, Si 2=Right		  
*/



//Calcular e invertir , si conviene, salida de cada canal
void sn_chip_siguiente_ciclo_siguiente(int chip)
{

	if (sn_chip_present.v==0) return;



	//actualizamos contadores de frecuencias
	sn_ultimo_valor_tono_A[chip]=sn_sine_table[sn_contador_tono_A[chip]];
	sn_contador_tono_A[chip] +=sn_freq_tono_A[chip];
	if (sn_contador_tono_A[chip]>=FRECUENCIA_CONSTANTE_NORMAL_SONIDO) {
			sn_contador_tono_A[chip] -=FRECUENCIA_CONSTANTE_NORMAL_SONIDO;
	}

	sn_ultimo_valor_tono_B[chip]=sn_sine_table[sn_contador_tono_B[chip]];
	sn_contador_tono_B[chip] +=sn_freq_tono_B[chip];
	if (sn_contador_tono_B[chip]>=FRECUENCIA_CONSTANTE_NORMAL_SONIDO) {
			sn_contador_tono_B[chip] -=FRECUENCIA_CONSTANTE_NORMAL_SONIDO;
	}

	sn_ultimo_valor_tono_C[chip]=sn_sine_table[sn_contador_tono_C[chip]];
	sn_contador_tono_C[chip] +=sn_freq_tono_C[chip];
	if (sn_contador_tono_C[chip]>=FRECUENCIA_CONSTANTE_NORMAL_SONIDO) {
			sn_contador_tono_C[chip] -=FRECUENCIA_CONSTANTE_NORMAL_SONIDO;
	} 


	sn_contador_ruido[chip] +=sn_freq_ruido[chip];
	if (sn_contador_ruido[chip]>=FRECUENCIA_CONSTANTE_NORMAL_SONIDO) {
			sn_contador_ruido[chip] -=FRECUENCIA_CONSTANTE_NORMAL_SONIDO;
			sn_chip_valor_aleatorio(chip);
			//printf ("Conmutar ruido\n");
	}

	

}




int sn_retorna_numero_chips(void)
{

	return 1;

}

void sn_chip_siguiente_ciclo(void)
{

	int chips=sn_retorna_numero_chips();
	int j;
	for (j=0;j<chips;j++) {
		sn_chip_siguiente_ciclo_siguiente(j);
	}

}




//Calcular contadores de incremento
//void sn_establece_frecuencia_tono(z80_byte indice, int *freq_tono, int *contador_tono)
void sn_establece_frecuencia_tono(z80_byte indice, int *freq_tono)
{

	int freq_temp;
	freq_temp=sn_3_8912_registros[sn_chip_selected][indice]+256*(sn_3_8912_registros[sn_chip_selected][indice+1] & 0x0F);
	//printf ("Valor freq_temp : %d\n",freq_temp);
	freq_temp=freq_temp*16;


	//controlamos divisiones por cero
	if (!freq_temp) freq_temp++;

	*freq_tono=FRECUENCIA_SN/freq_temp;

	//printf ("Valor freq_tono : %d\n",*freq_tono);

	/* Pruebas notas
	Octava 4. Nota C Valor registros: 424. Freq_tono=261 hz ok
	          Nota D Valor registros: 377. Freq_tono=293 hz ok

	Maximo valor en registro=12 bit=4095. 4095*16=65520
	FRECUENCIA_SN=1773400
	1773400/65520=27 Hz

	Minimo valor en registro=0
	1773400/(0*16) infinito

	Si es 1 por ejemplo, 1*16=16
	1773400/16= 110.837 KHz

	*/


	//freq_tono realmente tiene frecuencia*2... dice cada cuando se conmuta de signo
	//esto ya no hace falta con la tabla... multiplicador=1
	*freq_tono=(*freq_tono)*TEMP_MULTIPLICADOR;

        if (*freq_tono>FRECUENCIA_CONSTANTE_NORMAL_SONIDO) {
		//debug_printf (VERBOSE_DEBUG,"Frequency tone %d out of range",(*freq_tono)/TEMP_MULTIPLICADOR);
		*freq_tono=FRECUENCIA_CONSTANTE_NORMAL_SONIDO;
	}

	//si la frecuencia del tono es exactamente igual a la del sonido, pasara que siempre el valor de retorno
	//sera el primer valor en el arrsn de sn_sine_table.. seguramente +1 (32767)
	//alteramos un poco el valor
	if ( (*freq_tono)==FRECUENCIA_CONSTANTE_NORMAL_SONIDO) (*freq_tono)=FRECUENCIA_CONSTANTE_NORMAL_SONIDO-10;

	//de logica deberia resetear esto a 0.... pero si lo hago distorsiona el sonido
	//mejor dejar el contador con el valor actual
	//temp
	//*contador_tono=0;




}





//Enviar valor a puerto
void out_port_sn(z80_int puerto,z80_byte value)
{

	//Resetear detector de silencio
	silence_detection_counter=0;


	//printf ("Out port sn chip. Puerto: %d Valor: %d\n",puerto,value);


	//if (puerto==65533 && value>=14) printf("Out seleccion registro valor: %d\n",value);


	if (puerto==65533) {
		
		//seleccion de registro
		sn_3_8912_registro_sel[sn_chip_selected]=value & 15; //evitamos valores fuera de rango

		
	}
	else if (puerto==49149) {
		//valor a registro
		sn_3_8912_registros[sn_chip_selected][sn_3_8912_registro_sel[sn_chip_selected]&15]=value;



		//Nota sobre registro 7 mixer:
		//Bit 6 controla la direccion del registro de I/O - registro R14 - de puerto paralelo
		//como no emulamos puerto paralelo, no nos debe preocupar esto
		//registro R15 en este chip sn3-8912 no se usa para nada


		if (sn_3_8912_registro_sel[sn_chip_selected] ==0 || sn_3_8912_registro_sel[sn_chip_selected] == 1) {
			//Canal A
			//sn_establece_frecuencia_tono(0,&sn_freq_tono_A[sn_chip_selected],&sn_contador_tono_A[sn_chip_selected]);
			sn_establece_frecuencia_tono(0,&sn_freq_tono_A[sn_chip_selected]);

		}

		if (sn_3_8912_registro_sel[sn_chip_selected] ==2 || sn_3_8912_registro_sel[sn_chip_selected] == 3) {
			//Canal B
			//sn_establece_frecuencia_tono(2,&sn_freq_tono_B[sn_chip_selected],&sn_contador_tono_B[sn_chip_selected]);
			sn_establece_frecuencia_tono(2,&sn_freq_tono_B[sn_chip_selected]);

		}


		if (sn_3_8912_registro_sel[sn_chip_selected] ==4 || sn_3_8912_registro_sel[sn_chip_selected] == 5) {
			//Canal C
			//sn_establece_frecuencia_tono(4,&sn_freq_tono_C[sn_chip_selected],&sn_contador_tono_C[sn_chip_selected]);
			sn_establece_frecuencia_tono(4,&sn_freq_tono_C[sn_chip_selected]);
		}

		if (sn_3_8912_registro_sel[sn_chip_selected] ==6) {
			//Frecuencia ruido
			int freq_temp=sn_3_8912_registros[sn_chip_selected][6] & 31;
	       		//printf ("Valor registros ruido : %d Hz\n",freq_temp);
			freq_temp=freq_temp*16;

			//controlamos divisiones por cero
			if (!freq_temp) freq_temp++;

			sn_freq_ruido[sn_chip_selected]=SN_FRECUENCIA_NOISE/freq_temp;
			//printf ("Frecuencia ruido: %d Hz\n",sn_freq_ruido);

			//sn_freq_ruido realmente tiene frecuencia*2... dice cada cuando se conmuta de signo
			//sn_freq_ruido=sn_freq_ruido*TEMP_MULTIPLICADOR;
			sn_freq_ruido[sn_chip_selected]=sn_freq_ruido[sn_chip_selected]*2;



			if (sn_freq_ruido[sn_chip_selected]>FRECUENCIA_CONSTANTE_NORMAL_SONIDO) {
	                  //debug_printf (VERBOSE_DEBUG,"Frequency noise %d out of range",sn_freq_ruido[sn_chip_selected]/2);
        	          sn_freq_ruido[sn_chip_selected]=FRECUENCIA_CONSTANTE_NORMAL_SONIDO;
			}


			//si la frecuencia del ruido es exactamente igual a la del sonido
			//alteramos un poco el valor
			if ( sn_freq_ruido[sn_chip_selected]==FRECUENCIA_CONSTANTE_NORMAL_SONIDO) sn_freq_ruido[sn_chip_selected]=FRECUENCIA_CONSTANTE_NORMAL_SONIDO-10;



			//printf ("Frecuencia ruido final: %d Hz\n",sn_freq_ruido);


		}

	

	}
}




//Retorna la frecuencia de un registro concreto del chip SN de sonido
int sn_retorna_frecuencia(int registro,int chip)
{
	int freq_temp;
	int freq_tono;	
	freq_temp=sn_3_8912_registros[chip][registro*2]+256*(sn_3_8912_registros[chip][registro*2+1] & 0x0F);
	//printf ("Valor freq_temp : %d\n",freq_temp);
	freq_temp=freq_temp*16;


	//controlamos divisiones por cero
	if (!freq_temp) freq_temp++;

	freq_tono=FRECUENCIA_SN/freq_temp;

	return freq_tono;
}

/*

Filtros de salida sn chip
por chip(0...2) y canal (0..2)

tono si/no
ruido si/no
desactivado del todo si/no


R7 � Control del mezclador y de E/S
D7 No utilizado
D6 1=puerta de entrada, 0=puerta de salida
D5 Ruido en el canal C
D4 Ruido en el canal B
D3 Ruido en el canal A
D2 Tono en el canal C
D1 Tono en el canal B
DO Tono en el canal A

a 1 para desactivar eso
Canal A todo activado: mascara xxxx0xx0 -> mascara or
Canal A sin ruido: xxxx1xx0
Canal A sin tono: xxxx0xx1
Canal A sin nada: xxxx1xx1


Canal B todo activado: mascara xxx0xx0x -> mascara or
Canal B sin ruido: xxx1xx0x
Canal B sin tono: xxx0xx1x
Canal B sin nada: xxx1xx1x

Canal C todo activado: mascara xx0xx0xx -> mascara or
Canal C sin ruido: xx1xx0xx
Canal C sin tono: xx0xx1xx
Canal C sin nada: xx1xx1xx

 */

//A 0 todos para normal
z80_byte sn_filtros[MAX_SN_CHIPS];

void sn_init_filters(void)
{
	int i;
	for (i=0;i<MAX_SN_CHIPS;i++) {
		sn_filtros[i]=0;
	}
}

//Retorna el registro del mezclador, pero aplicando filtro de canal activado/no, ruido si/no, tono si/no
//Usado en mid export, direct midi
z80_byte sn_retorna_mixer_register(int chip)
{
	z80_byte valor=sn_3_8912_registros[chip][7];

	//Aplicar filtro
	valor |=sn_filtros[chip];

	return valor;
}



void sn_set_volume_noise(z80_byte volume)
{

                sn_volumen_canal_ruido=volume;

}
