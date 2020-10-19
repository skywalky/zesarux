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

#ifndef QL_QDOS_HANDLER_H
#define QL_QDOS_HANDLER_H

#include "ql.h"

#define QLTRAPS_MAX_OPEN_FILES 3
#define QLTRAPS_START_FILE_NUMBER 32


//operation not complete
#define QDOS_ERROR_CODE_NC -1


//buffer overflow
#define QDOS_ERROR_CODE_BO -5

//channel not open
#define QDOS_ERROR_CODE_NO -6


//file or device not found
#define QDOS_ERROR_CODE_NF -7

//end of file
#define QDOS_ERROR_CODE_EF -10

extern void ql_rom_traps(void);

extern char ql_mdv1_root_dir[];
extern char ql_mdv2_root_dir[];
extern char ql_flp1_root_dir[];

extern int ql_microdrive_floppy_emulation;

extern z80_byte ql_last_trap;

extern int ql_previous_trap_was_4;

#endif
