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

#ifndef MENU_ITEMS_H
#define MENU_ITEMS_H

#include "cpu.h"
#include "sensors.h"


extern void menu_poke(MENU_ITEM_PARAMETERS);
extern void menu_settings_debug(MENU_ITEM_PARAMETERS);
extern void menu_settings_audio(MENU_ITEM_PARAMETERS);
extern void menu_zxvision_test(MENU_ITEM_PARAMETERS);

extern int menu_cond_ay_chip(void);
extern int menu_cond_ay_or_sn_chip(void);
extern int menu_cond_i8049_chip(void);
extern int last_debug_poke_dir;
extern void menu_debug_poke(MENU_ITEM_PARAMETERS);
extern void menu_debug_cpu_resumen_stats(MENU_ITEM_PARAMETERS);
extern void menu_about_core_statistics(MENU_ITEM_PARAMETERS);
extern void menu_ay_registers(MENU_ITEM_PARAMETERS);
extern void menu_debug_tsconf_tbblue_msx_videoregisters(MENU_ITEM_PARAMETERS);
extern void menu_debug_tsconf_tbblue_msx_spritenav(MENU_ITEM_PARAMETERS);
extern void menu_debug_tsconf_tbblue_msx_tilenav(MENU_ITEM_PARAMETERS);
extern void menu_audio_new_waveform(MENU_ITEM_PARAMETERS);
extern void menu_debug_new_visualmem(MENU_ITEM_PARAMETERS);
extern void menu_audio_new_ayplayer(MENU_ITEM_PARAMETERS);
extern int menu_audio_new_ayplayer_si_mostrar(void);
extern void menu_debug_hexdump(MENU_ITEM_PARAMETERS);
extern void menu_osd_adventure_keyboard(MENU_ITEM_PARAMETERS);
extern void menu_osd_adventure_keyboard_next(void);
extern void menu_debug_dma_tsconf_zxuno(MENU_ITEM_PARAMETERS);
extern void menu_display_total_palette(MENU_ITEM_PARAMETERS);

extern void menu_debug_disassemble(MENU_ITEM_PARAMETERS);
extern void menu_debug_assemble(MENU_ITEM_PARAMETERS);

extern void menu_cpu_settings(MENU_ITEM_PARAMETERS);
extern void menu_settings_display(MENU_ITEM_PARAMETERS);

extern void menu_draw_background_windows(MENU_ITEM_PARAMETERS);
extern void menu_debug_cpu_stats(MENU_ITEM_PARAMETERS);
extern void menu_tbblue_machine_id(MENU_ITEM_PARAMETERS);

extern void menu_ext_desktop_settings(MENU_ITEM_PARAMETERS);
extern void menu_cpu_transaction_log(MENU_ITEM_PARAMETERS);

extern void menu_debug_view_sprites(MENU_ITEM_PARAMETERS);


extern void menu_breakpoint_fired(char *s);


extern void menu_ay_partitura(MENU_ITEM_PARAMETERS);
extern void menu_record_mid(MENU_ITEM_PARAMETERS);

extern void menu_direct_midi_output(MENU_ITEM_PARAMETERS);
extern void menu_ay_mixer(MENU_ITEM_PARAMETERS);
extern void menu_i8049_mixer(MENU_ITEM_PARAMETERS);
extern void menu_uartbridge(MENU_ITEM_PARAMETERS);
extern void menu_network(MENU_ITEM_PARAMETERS);
extern void menu_settings_statistics(MENU_ITEM_PARAMETERS);

extern void menu_debug_change_memory_zone_splash(void);


extern void menu_zeng_send_message(MENU_ITEM_PARAMETERS);

extern void menu_mmc_divmmc(MENU_ITEM_PARAMETERS);
extern void menu_storage_diviface_eprom_write_jumper(MENU_ITEM_PARAMETERS);
extern void menu_storage_mmc_autoconfigure_tbblue(MENU_ITEM_PARAMETERS);

extern void menu_ide_divide(MENU_ITEM_PARAMETERS);

extern int menu_zeng_send_message_cond(void);


extern int menu_zsock_http(char *host, char *url,int *http_code,char **mem,int *t_leidos, char **mem_after_headers,
            int skip_headers,char *add_headers,int use_ssl,char *redirect_url,char *ssl_sni_host_name);

extern int menu_download_file(char *host,char *url,char *archivo_temp,int ssl_use,int estimated_maximum_size,char *ssl_sni_host_name);

extern void menu_display_settings(MENU_ITEM_PARAMETERS);

extern void menu_ay_pianokeyboard(MENU_ITEM_PARAMETERS);

extern void menu_beeper_pianokeyboard(MENU_ITEM_PARAMETERS);

extern void menu_debug_tsconf_tbblue_msx(MENU_ITEM_PARAMETERS);

extern void menu_windows(MENU_ITEM_PARAMETERS);

extern void menu_help_show_keyboard(MENU_ITEM_PARAMETERS);

extern void menu_audio_chip_info(MENU_ITEM_PARAMETERS);

extern void menu_zxpand(MENU_ITEM_PARAMETERS);

extern void menu_ql_mdv_flp(MENU_ITEM_PARAMETERS);

extern void menu_debug_unnamed_console(MENU_ITEM_PARAMETERS);

extern void menu_audio_general_sound(MENU_ITEM_PARAMETERS);

extern void menu_debug_ioports(MENU_ITEM_PARAMETERS);

extern void menu_about_new(MENU_ITEM_PARAMETERS);

extern void menu_fileselector_settings(MENU_ITEM_PARAMETERS);

extern void menu_debug_load_source_code(MENU_ITEM_PARAMETERS);

extern void menu_debug_unload_source_code(MENU_ITEM_PARAMETERS);

extern void menu_snapshot_rewind(MENU_ITEM_PARAMETERS);

extern menu_z80_moto_int menu_debug_hexdump_direccion;

extern void menu_debug_hexdump_with_ascii(char *dumpmemoria,menu_z80_moto_int dir_leida,int bytes_por_linea,z80_byte valor_xor);

extern void menu_find(MENU_ITEM_PARAMETERS);

extern void menu_debug_view_basic_variables(MENU_ITEM_PARAMETERS);

extern void menu_debug_view_sensors(MENU_ITEM_PARAMETERS);

extern void menu_zxdesktop_set_userdef_buttons_functions(MENU_ITEM_PARAMETERS);

extern void menu_accessibility_settings(MENU_ITEM_PARAMETERS);

extern void menu_ext_desk_settings_enable(MENU_ITEM_PARAMETERS);

extern void menu_visual_realtape(MENU_ITEM_PARAMETERS);

struct s_menu_debug_view_sensors_list {
    char short_name[SENSORS_MAX_SHORT_NAME];
    int fila;
    int columna;
    //tipo de widget
    int tipo;
    int valor_en_vez_de_perc;
};

typedef struct s_menu_debug_view_sensors_list menu_debug_view_sensors_list;

//5x3
//0=fila 0, columna 0
//1=fila 0, columna 1
//...
//6=fila 1, columna 0
#define MENU_VIEW_SENSORS_TOTAL_COLUMNS 5
#define MENU_VIEW_SENSORS_TOTAL_ROWS 3

#define MENU_VIEW_SENSORS_TOTAL_ELEMENTS (MENU_VIEW_SENSORS_TOTAL_COLUMNS*MENU_VIEW_SENSORS_TOTAL_ROWS)




extern menu_debug_view_sensors_list menu_debug_view_sensors_list_sensors[];

#endif

