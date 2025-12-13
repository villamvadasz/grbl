/*------------------------------------------------------------------------/
/  MMCv3/SDv1/SDv2 (in SDIO mode) control module
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
#include "SDcard.h"

static volatile DSTATUS Stat = (STA_NODISK | STA_NOINIT);	/* Disk status */
uint8 mmc_sdcard_microchip_do1ms = 0;
MEDIA_INFORMATION *media_information = NULL;
/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

void do_mmc_sdcard_sdio(void) {
	if (mmc_sdcard_microchip_do1ms) {
		mmc_sdcard_microchip_do1ms = 0;
	}
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
DSTATUS SD4Bitcard_disk_initialize (void)
{
	if (MDD_SD4BIT_MediaDetect()) {
		Stat &= ~STA_NODISK;
		media_information = MDD_SD4BIT_MediaInitialize();
		if (media_information != NULL) {
			if (media_information->errorCode == MEDIA_NO_ERROR) {
				if (media_information->sectorSize == 512) {
					Stat &= ~STA_NOINIT;
				}
			} else if (media_information->errorCode == MEDIA_CANNOT_INITIALIZE) {
			} else {
			}
		}
	} else {
		Stat |= (STA_NODISK | STA_NOINIT);
	}
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS SD4Bitcard_disk_status (void)
{
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT SD4Bitcard_disk_read (
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	DWORD sector,	/* Start sector number (LBA) */
	BYTE count		/* Sector count (1..255) */
)
{
	DRESULT result = RES_OK;
	if (!count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	
	BYTE x = 0;
	for (x = 0; x < count; x++) {
		if (MDD_SD4BIT_SectorRead(sector + x, buff + (x * MEDIA_BLOCK_SIZE)) == 0) {
			result = RES_ERROR;
		}
	}
	return result;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT SD4Bitcard_disk_write (
	const BYTE *buff,		/* Pointer to the data to be written */
	DWORD sector,			/* Start sector number (LBA) */
	BYTE count				/* Sector count (1..255) */
)
{
	DRESULT result = RES_OK;
	BYTE x = 0;

	if (!count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (Stat & STA_PROTECT) return RES_WRPRT;

	for (x = 0; x < count; x++) {
		BYTE* ptr = (BYTE*)(buff + (x * MEDIA_BLOCK_SIZE));
		if (MDD_SD4BIT_SectorWrite(sector + x, ptr, 1) == 0) {
			result = RES_ERROR;
		}
	}
	return result;
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
#if _USE_IOCTL
DRESULT SD4Bitcard_disk_ioctl (
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	DWORD csz;


	if (Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;
	switch (ctrl) {
		case CTRL_SYNC :	/* Flush write-back cache, Wait for end of internal process */
			res = RES_OK;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (WORD) */
			*(DWORD*)ptr = MDD_SD4BIT_ReadCapacity() / MDD_SD4BIT_ReadSectorSize();
			res = RES_OK;
			break;

		case GET_SECTOR_SIZE :	/* Get sectors on the disk (WORD) */
			*(DWORD*)ptr = MDD_SD4BIT_ReadSectorSize();
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sectors (DWORD) */
			*(DWORD*)ptr = MDD_SD4BIT_ReadSectorSize();
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

void SD4Bitcard_disk_timerproc (void)
{
	mmc_sdcard_microchip_do1ms = 1;
}
