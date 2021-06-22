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
#include <sys/time.h>

#include "cpu.h"
#include "sensors.h"
#include "debug.h"
#include "ay38912.h"
#include "screen.h"
#include "menu.h"
#include "timer.h"
#include "stats.h"
#include "audio.h"




int sensor_fps_funcion_get_value(int id)
{
    return ultimo_fps;
}

int sensor_total_average_cpu_get_value(int id)
{
	int media_cpu=0;

	if (cpu_use_total_acumulado_medidas>0) {
		media_cpu=cpu_use_total_acumulado/cpu_use_total_acumulado_medidas;
	}

    return media_cpu;
}

int sensor_instant_average_cpu_get_value(int id)
{

    return menu_last_cpu_use;
}

//Retorna volumen de un canal AY
//Id es:
//(chip*4) + canal
int sensor_ay_vol_chip_funcion_get_value(int id)
{
    int chip=id/4;

    int canal=id & 3;

    return (ay_3_8912_registros[chip][8+canal]&15);

}

int sensor_time_betw_frames_get_value(int id)
{
    return core_cpu_timer_each_frame_difftime;
}

int sensor_last_core_frame_get_value(int id)
{
    return core_cpu_timer_frame_difftime;
}

int sensor_last_full_render_get_value(int id)
{
    return core_cpu_timer_refresca_pantalla_difftime;
}

int sensor_dropped_frames_get_value(int id)
{

    int perc_dropped;

    //Lo ideal es que el valor maximo definido en el array fuese stats_frames_total en vez de 100,
    //para poder retornar aquí el valor tal cual de stats_frames_total_dropped
    //pero dado que ese stats_frames_total no es un valor constante no puede indicarse en el array,
    //y aqui ya retornamos el tanto por ciento tal cual

    //Evitar división por cero
    if (stats_frames_total==0) perc_dropped=0;

    else perc_dropped=(stats_frames_total_dropped*100)/stats_frames_total;    

    return perc_dropped;
}

int sensor_audio_buffer_get_value(int id)
{

    //Igual que sensor_dropped_frames_get_value, retornamos tanto por ciento tal cual en vez de valor absoluto

    int tamanyo_buffer_audio,posicion_buffer_audio;
    audio_get_buffer_info(&tamanyo_buffer_audio,&posicion_buffer_audio);

    int perc_audio;

    if (tamanyo_buffer_audio==0) {
        perc_audio=0;
    }

    else {
        perc_audio=(posicion_buffer_audio*100)/tamanyo_buffer_audio;
    }

    return perc_audio;
}

sensor_item sensors_array[TOTAL_SENSORS]={
    {
    "ay_vol_chip0_chan_A","AY Volume Chip 0 Channel A","VolA[0]",
    0,15,
    84,-9999,
    9999,-9999,
    sensor_ay_vol_chip_funcion_get_value,0
    },

    {
    "ay_vol_chip0_chan_B","AY Volume Chip 0 Channel B","VolB[0]",
    0,15,
    84,-9999,
    9999,-9999,
    sensor_ay_vol_chip_funcion_get_value,1
    },

    {
    "ay_vol_chip0_chan_C","AY Volume Chip 0 Channel C","VolC[0]",
    0,15,
    84,-9999,
    9999,-9999,
    sensor_ay_vol_chip_funcion_get_value,2
    },   


    {
    "ay_vol_chip1_chan_A","AY Volume Chip 1 Channel A","VolA[1]",
    0,15,
    84,-9999,
    9999,-9999,
    sensor_ay_vol_chip_funcion_get_value,4
    },

    {
    "ay_vol_chip1_chan_B","AY Volume Chip 1 Channel B","VolB[1]",
    0,15,
    84,-9999,
    9999,-9999,
    sensor_ay_vol_chip_funcion_get_value,5
    },

    {
    "ay_vol_chip1_chan_C","AY Volume Chip 1 Channel C","VolC[1]",
    0,15,
    84,-9999,
    9999,-9999,
    sensor_ay_vol_chip_funcion_get_value,6
    },  



    {
    "ay_vol_chip2_chan_A","AY Volume Chip 2 Channel A","VolA[2]",
    0,15,
    84,-9999,
    9999,-9999,
    sensor_ay_vol_chip_funcion_get_value,8
    },

    {
    "ay_vol_chip2_chan_B","AY Volume Chip 2 Channel B","VolB[2]",
    0,15,
    84,-9999,
    9999,-9999,
    sensor_ay_vol_chip_funcion_get_value,9
    },

    {
    "ay_vol_chip2_chan_C","AY Volume Chip 2 Channel C","VolC[2]",
    0,15,
    84,-9999,
    9999,-9999,
    sensor_ay_vol_chip_funcion_get_value,10
    },      


    
    {
    "fps","Frames per second","FPS",
    0,50,
    9999,-9999,
    9999,25,
    sensor_fps_funcion_get_value,0
    },

    {
    "total_avg_cpu","Total average cpu use","TotalCPU",
    0,100,
    84,-9999,
    9999,-9999,
    sensor_total_average_cpu_get_value,0
    },

   {
    "instant_avg_cpu","Instant average cpu use","CPU",
    0,100,
    84,-9999,
    9999,-9999,
    sensor_instant_average_cpu_get_value,0
    },

    //En este el tiempo maximo y los porcentajes no tienen mucho sentido
    //core_cpu_timer_frame_difftime
   {
    "last_core_frame","Last Core Frame","CoreFrame",
    0,20000, 
    9999,-9999,
    10000,-9999,
    sensor_last_core_frame_get_value,0
    },    

    //En este el tiempo maximo y los porcentajes no tienen mucho sentido
    //core_cpu_timer_refresca_pantalla_difftime
   {
    "last_full_render","Last Full Render","FullRender",
    0,20000, 
    9999,-9999,
    10000,-9999,
    sensor_last_full_render_get_value,0
    },            

    //En este el tiempo maximo y los porcentajes no tienen mucho sentido
   {
    "time_betw_frames","Time between frames","TBFrames",
    0,40000, //lo ajusto a 40000 porque el tiempo ideal es 20000 o sea que idealmente flucturara sobre 50% el porcentaje
    9999,-9999,
    22000,-9999,
    sensor_time_betw_frames_get_value,0
    },    

   {
    "perc_dropped_frames","Percent Dropped Video Frames","%DropFrame",
    0,100, 
    50,-9999,
    9999,-9999,
    sensor_dropped_frames_get_value,0
    },     

   {
    "perc_audio_buffer","Percent Audio Buffer","%AudioBuff",
    0,100, 
    85,15,
    9999,-9999,
    sensor_audio_buffer_get_value,0
    },        

};

//Encuentra la posicion en el array de sensores segun su nombre corto
//retorna -1 si no encontrado
int sensor_find(char *short_name)
{
    int i;

    for (i=0;i<TOTAL_SENSORS;i++) {
        if (!strcasecmp(short_name,sensors_array[i].short_name)) return i;
    }

    debug_printf(VERBOSE_DEBUG,"Sensor name %s not found",short_name);
    return -1;
}

//Retorna valor sensor segun id del array
int sensor_get_value_by_id(int indice)
{
    if (indice<0 || indice>=TOTAL_SENSORS) {
        debug_printf(VERBOSE_DEBUG,"Sensor index %d beyond limit",indice);
        return 0;
    }

    int id_parameter=sensors_array[indice].id_parameter;

    return sensors_array[indice].f_funcion_get_value(id_parameter);
}

//Retorna valor sensor. 0 si no encontrado
int sensor_get_value(char *short_name)
{
    int indice=sensor_find(short_name);

    if (indice<0) return 0;

    return sensor_get_value_by_id(indice);
}

//Retorna valor porcentaje sensor. 0 si no encontrado
int sensor_get_percentaje_value_by_id(int indice)
{

    int current_value=sensor_get_value_by_id(indice);

    int min_value=sensors_array[indice].min_value;

    int max_value=sensors_array[indice].max_value;

    //Obtener el total de values desde min a max
    int total_valores=max_value-min_value;

    if (total_valores==0) return 0; //Evitar divisiones por 0

    //Obtener diferencia desde el minimo al valor actual
    int offset_valor=current_value-min_value;

    int porcentaje=(offset_valor*100)/total_valores;

    //Controlar limites
    if (porcentaje<0) return 0;
    if (porcentaje>100) return 100;

    return porcentaje;
}

//Retorna valor porcentaje sensor. 0 si no encontrado
int sensor_get_percentaje_value(char *short_name)
{
    int indice=sensor_find(short_name);

    if (indice<0) return 0;

    return sensor_get_percentaje_value_by_id(indice);

}