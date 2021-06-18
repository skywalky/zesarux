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


#define TOTAL_SENSORS 3


//Retorna volumen de un canal AY
//Id es:
//(chip*4) + canal
int sensor_ay_vol_chip_funcion_get_value(int id)
{
    int chip=id/4;

    int canal=id & 3;

    return (ay_3_8912_registros[chip][8+canal]&15);

    //TODO otros dos chips

}

sensor_item sensors_array[TOTAL_SENSORS]={
    {
    "ay_vol_chip0_chan_A","AY Volume Chip 0 Channel A",
    0,15,
    sensor_ay_vol_chip_funcion_get_value,0
    
    },

    {
    "ay_vol_chip0_chan_B","AY Volume Chip 0 Channel B",
    0,15,
    sensor_ay_vol_chip_funcion_get_value,1
    },

    {
    "ay_vol_chip0_chan_B","AY Volume Chip 0 Channel C",
    0,15,
    sensor_ay_vol_chip_funcion_get_value,2
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
int sensor_get_percentaje_value(char *short_name)
{
    int indice=sensor_find(short_name);

    if (indice<0) return 0;

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