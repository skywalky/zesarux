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

#ifndef SENSORS_H
#define SENSORS_H

struct s_sensor_item {
    //Nombre corto del sensor
    char short_name[32];
    //Nombre largo del sensor
    char long_name[100];
    //Funcion que retorna el valor del sensor
    //...
    int (*f_funcion_get_value)(int id);

    //Id para la funcion usado sobretodo en sensors del mismo tipo: por ejemplo ay_vol_chip0_chan_A, ay_vol_chip0_chan_B, el id sera 0,1, etc
    int id_parameter;

    int min_value;
    int max_value;
};

typedef struct s_sensor_item sensor_item;

#endif
