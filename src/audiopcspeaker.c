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
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


#include <sys/io.h>



#include "audiopcspeaker.h"
#include "cpu.h"
#include "audio.h"
#include "compileoptions.h"
#include "debug.h"
#include "settings.h"

#ifdef USE_PTHREADS
#include <pthread.h>

     pthread_t thread1;

#endif

int ptr_audiopcspeaker;

unsigned int requested, ioctl_format, ioctl_channels, ioctl_rate;

#define BASE_SOUND_FRAG_PWR     6

//static char const * const default_device = "/dev/input/by-path/platform-pcspkr-event-spkr";

int audiopcspeaker_init(void)
{

	//audio_driver_accepts_stereo.v=1;



    debug_printf (VERBOSE_INFO,"Init PC Speaker Audio Driver");


	//Metodo alternativo era usando el device default_device y enviando eventos, pero eso solo gestiona frecuencias
                //ptr_audiopcspeaker=open(default_device,O_WRONLY);


	//Pedir "permiso" para usar puerto pc speaker
	if (ioperm(0x61,1,1)) {
		debug_printf(VERBOSE_ERR,"Error asking permissions on speaker port");
		return 1;
	}




	//Esto debe estar al final, para que funcione correctamente desde menu, cuando se selecciona un driver, y no va, que pueda volver al anterior
	audio_driver_name="pcspeaker";

	return 0;

}



int audiopcspeaker_thread_finish(void)
{

	if (thread1!=0) {
		debug_printf (VERBOSE_DEBUG,"Ending audiopcspeaker thread");
		int s=pthread_cancel(thread1);
		if (s != 0) debug_printf (VERBOSE_DEBUG,"Error cancelling pthread pcspeaker");

		thread1=0;
	}

	//Pausa de 0.1 segundo
	 usleep(100000);
	

	return 0;

}

void audiopcspeaker_end(void)
{
	debug_printf (VERBOSE_INFO,"Ending pcspeaker audio driver");
	audiopcspeaker_thread_finish();
	audio_playing.v=0;

	//stop_beep();

	//close(ptr_audiopcspeaker);
}







/* no usados estos
void beep_freq(int frequency)
{
	struct input_event e = { 0 };
	e.type = EV_SND;
	e.code = SND_TONE;
	e.value = frequency; //freq;
	write(ptr_audiopcspeaker, &e, sizeof(e));
}

void start_beep(void)
{
  beep_freq(10);
}

void stop_beep(void)
{
beep_freq(0);
}
*/

char *buffer_playback_pcspeaker;

char last_audio_sample=0;

int audiopcspeaker_tiempo_espera=8;

void *audiopcspeaker_enviar_audio(void *nada)
{


	while (1) {


		//Establecer el buffer de reproduccion
		buffer_playback_pcspeaker=audio_buffer_playback;

		//enviamos siguiente sonido avisando de interrupcion a cpu
		//interrupt_finish_sound.v=1;



		int len=AUDIO_BUFFER_SIZE;

		int ofs=0;


		//Valor de referencia
		/*
Port 61h controls how the speaker will operate as follows:

Bit 0    Effect
-----------------------------------------------------------------
  0      The state of the speaker will follow bit 1 of port 61h
  1      The speaker will be connected to PIT channel 2, bit 1 is
         used as switch ie 0 = not connected, 1 = connected.		
		*/
		z80_byte valor_puerto_original=inb(0x61);
		valor_puerto_original &=(255-2-1);

		//Ultimo valor. de momento meterlo a un valor que no sea igual
		z80_byte last_valor_puerto=valor_puerto_original ^255;
		z80_byte final_valor_puerto;
		for (;len>=0;len--) {
			
			char current_audio_sample=buffer_playback_pcspeaker[ofs];
			
			//Si valor actual es mayor, enviar 1
			if (current_audio_sample>last_audio_sample) {
				// altavoz a 1

				final_valor_puerto=valor_puerto_original | 2;
			}

			//Si es menor o igual, enviar 0
			else {
				// altavoz a 0

				final_valor_puerto=valor_puerto_original;

			}

			last_audio_sample=current_audio_sample;

			int tiempo_espera=audiopcspeaker_tiempo_espera;

			//Si cambia el altavoz
			if (last_valor_puerto!=final_valor_puerto) {
				outb(final_valor_puerto,0x61);
				tiempo_espera--; //se supone que el out tarda 1 microsegundo
			}

			if (tiempo_espera>0) usleep(tiempo_espera);

			last_valor_puerto=final_valor_puerto;

			ofs++;
			//stereo. Pasamos del otro canal directamente
			ofs++;
		}
         

		while (audio_playing.v==0 || silence_detection_counter==SILENCE_DETECTION_MAX) {
				//1 ms
				usleep(1000);
		}

	}


	//Para evitar warnings al compilar de unused parameter ‘nada’ [-Wunused-parameter]
	nada=0;
	nada++;


	return NULL;
} 

pthread_t thread1=0;

void audiopcspeaker_send_frame(char *buffer)
{


	if (audio_playing.v==0) {
			//Volvemos a activar pthread
			buffer_playback_pcspeaker=buffer;
			audio_playing.v=1;
	}


	if (thread1==0) {
		buffer_playback_pcspeaker=buffer;
     	if (pthread_create( &thread1, NULL, &audiopcspeaker_enviar_audio, NULL) ) {
                cpu_panic("Can not create audiopcspeaker pthread");
        }      
	}


	
}



void audiopcspeaker_get_buffer_info (int *buffer_size,int *current_size)
{
  *buffer_size=AUDIO_BUFFER_SIZE*2; //*2 porque es stereo
  *current_size=0;
}

