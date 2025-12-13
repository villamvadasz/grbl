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

static volatile DSTATUS TemplateNameToReplace_Stat = STA_NOINIT;	/* Disk status */

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS TemplateNameToReplace_disk_initialize (
	void
)
{
	return TemplateNameToReplace_Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS TemplateNameToReplace_disk_status (
	void
)
{
	return TemplateNameToReplace_Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT TemplateNameToReplace_disk_read (
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	DWORD sector,	/* Start sector number (LBA) */
	BYTE count		/* Sector count (1..255) */
)
{
	return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT TemplateNameToReplace_disk_write (
	BYTE *buff,		/* Pointer to the data to be written */
	DWORD sector,			/* Start sector number (LBA) */
	BYTE count				/* Sector count (1..255) */
)
{
	return RES_ERROR;
}
#endif



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT TemplateNameToReplace_disk_ioctl (
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	DWORD csz;


	if (TemplateNameToReplace_Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;
	switch (ctrl) {
		case CTRL_SYNC :	/* Flush write-back cache, Wait for end of internal process */
			res = RES_PARERR;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (WORD) */
			res = RES_PARERR;
			break;

		case GET_SECTOR_SIZE :	/* Get sectors on the disk (WORD) */
			res = RES_PARERR;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sectors (DWORD) */
			res = RES_PARERR;
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

void TemplateNameToReplace_disk_timerproc (void)
{
}

