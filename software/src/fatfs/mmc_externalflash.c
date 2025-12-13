/*------------------------------------------------------------------------/
/  MMCv3/SDv1/SDv2 (in SPI mode) control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2012, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/
#include "mal.h"
#include "diskio.h"
#include "c_fatfs.h"
#include "ffconf.h"

#include "SST25VF016.h"
#include "w_SST25VF016.h"

static volatile DSTATUS ExternalFlash_Stat = STA_NOINIT;	/* Disk status */
uint8 externalFlashTempBuffer[4096];

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS ExternalFlash_disk_initialize (
	void
)
{
	init_SST25();
	SST25ResetWriteProtection();
	ExternalFlash_Stat &= ~STA_NOINIT;
	return ExternalFlash_Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS ExternalFlash_disk_status (
	void
)
{
	return ExternalFlash_Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT ExternalFlash_disk_read (
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	DWORD sector,	/* Start sector number (LBA) */
	BYTE count		/* Sector count (1..255) */
)
{
	SST25ReadArray(sector * _MAX_SS, buff, count * _MAX_SS);
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT ExternalFlash_disk_write (
	BYTE *buff,		/* Pointer to the data to be written */
	DWORD sector,			/* Start sector number (LBA) */
	BYTE count				/* Sector count (1..255) */
)
{
	uint32 i = 0;
	for (i = 0; i < count; i++) {
		uint32 addressTemp = (sector * _MAX_SS) & 0xFFFFF000;
		uint32 offsetTemp = (sector * _MAX_SS) & 0xFFF;
		SST25ReadArray(addressTemp, externalFlashTempBuffer, 4096);
		SST25SectorErase(addressTemp);
		memcpy(externalFlashTempBuffer + offsetTemp, buff, 512);
		SST25WriteArray(addressTemp, externalFlashTempBuffer, 4096);
		sector++;
	}


	//SST25ReadArray(sector * _MAX_SS, buff, count * _MAX_SS);
	//SST25ReadArray(sector * _MAX_SS, buff, count * _MAX_SS);
	return RES_OK;
}
#endif



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT ExternalFlash_disk_ioctl (
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	DWORD csz;


	if (ExternalFlash_Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;
	switch (ctrl) {
		case CTRL_SYNC :	/* Flush write-back cache, Wait for end of internal process */
			res = RES_OK;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (WORD) */
			*(DWORD*)buff = 4096;//16 mbit flash, 2 Mbyte, 512byte sector size 
			res = RES_OK;
			break;

		case GET_SECTOR_SIZE :	/* Get sectors on the disk (WORD) */
			*(WORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sectors (DWORD) */
			*(WORD*)buff = 1; //4Kb (bit size?) sector size
			res = RES_OK;
			break;

		case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
			res = RES_PARERR;
			break;

		case MMC_GET_CSD :	/* Receive CSD as a data block (16 bytes) */
			res = RES_PARERR;
			break;

		case MMC_GET_CID :	/* Receive CID as a data block (16 bytes) */
			res = RES_PARERR;
			break;

		case MMC_GET_OCR :	/* Receive OCR as an R3 resp (4 bytes) */
			res = RES_PARERR;
			break;

		case MMC_GET_SDSTAT :	/* Receive SD statsu as a data block (64 bytes) */
			res = RES_PARERR;
			break;

		default:
			res = RES_PARERR;
	}
	return res;
}
#endif


/*-----------------------------------------------------------------------*/
/* Device Timer Driven Procedure                                         */
/*-----------------------------------------------------------------------*/
/* This function must be called by timer interrupt in period of 1ms      */

void ExternalFlash_disk_timerproc (void)
{
}

