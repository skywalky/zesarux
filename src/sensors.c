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

sensor_item sensors_array[TOTAL_SENSORS]={
    {
    "ay_vol_chip0_chan_A","AY Volume Chip 0 Channel A",
    0,15,
    84,-9999,
    9999,-9999,
    sensor_ay_vol_chip_funcion_get_value,0
    },

    {
    "ay_vol_chip0_chan_B","AY Volume Chip 0 Channel B",
    0,15,
    84,-9999,
    9999,-9999,
    sensor_ay_vol_chip_funcion_get_value,1
    },

    {
    "ay_vol_chip0_chan_C","AY Volume Chip 0 Channel C",
    0,15,
    84,-9999,
    9999,-9999,
    sensor_ay_vol_chip_funcion_get_value,2
    },   
    
    {
    "fps","Frames per second",
    0,50,
    9999,-9999,
    9999,25,
    sensor_fps_funcion_get_value,0
    },

    {
    "total_avg_cpu","Total average cpu use",
    0,100,
    84,-9999,
    9999,-9999,
    sensor_total_average_cpu_get_value,0
    },

   {
    "instant_avg_cpu","Instant average cpu use",
    0,100,
    84,-9999,
    9999,-9999,
    sensor_instant_average_cpu_get_value,0
    }    

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