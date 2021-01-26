/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include <stdio.h>

/* Definitions of physical drive number for each drive */
#define DEV_MMC		0	/* Example: Map MMC/SD card to physical drive 0 */
#define DEV_RAM		1	/* Example: Map Ramdisk to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


FILE *ptr_fatfs_disk_zero_file;


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	//DSTATUS stat;
	//int result;

    printf("FatFs llamado disk status mmc para physical drive: %d\n",pdrv);

	switch (pdrv) {
    /*
	case DEV_RAM :
		//result = RAM_disk_status();

		// translate the reslut code here

		return stat;
    */
	case DEV_MMC :
		//result = MMC_disk_status();

		// translate the result code here

        //TODO: de momento ok
        printf("FatFs llamado disk status mmc\n");
        return 0;
    break;


    /*
	case DEV_USB :
		//result = USB_disk_status();

		// translate the reslut code here

		return stat;
    */
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	//DSTATUS stat;
	//int result;
    
    printf("FatFs llamado disk_initialize para drive %d\n",pdrv);

	switch (pdrv) {
    /*
	case DEV_RAM :
		//result = RAM_disk_initialize();

		// translate the reslut code here

		return stat;
    */
	case DEV_MMC :
		//result = MMC_disk_initialize();

		// translate the reslut code here

        //Abrimos el archivo

        ptr_fatfs_disk_zero_file=fopen(fatfs_disk_zero_path,"rb");

        if (ptr_fatfs_disk_zero_file==NULL) {
            printf("FatFs error abriendo archivo %s\n",fatfs_disk_zero_path);
            return STA_NOINIT;
        }
            
		return 0;
    break;

    /*
    case DEV_USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
    */
	}
    
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	//DRESULT res;
	//int result;

    printf("FatFs llamado disk_read para drive %d\n",pdrv);

	switch (pdrv) {
        /*
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
        */

	case DEV_MMC :
		// translate the arguments here

		//result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

        //TODO: no se si realmente seria leer FF_MIN_SS o FF_MAX_SS
        fseek(ptr_fatfs_disk_zero_file,sector*FF_MIN_SS,SEEK_SET);

        fread(buff,1,count*FF_MIN_SS,ptr_fatfs_disk_zero_file);

		return RES_OK;
    break;

    /*

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
    */

	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	//DRESULT res;
	//int result;

    printf("FatFs llamado disk_write para drive %d\n",pdrv);

	switch (pdrv) {
        /*
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
        */

	case DEV_MMC :
		// translate the arguments here

		//result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

        //TODO

		return RES_OK;
    break;

    /*
	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
    */
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	//DRESULT res;
	//int result;

    printf("FatFs llamado disk_ioctl para drive %d\n",pdrv);

	switch (pdrv) {
        /*
	case DEV_RAM :

		// Process of the command for the RAM drive

		return res;
        */

	case DEV_MMC :

		// Process of the command for the MMC/SD card

        //TODO
        return RES_OK;

	break;

    /*

	case DEV_USB :

		// Process of the command the USB drive

		return res;
    */
	}

	return RES_PARERR;
}



DWORD get_fattime (void)
{
    //TODO

    return 0;
}


