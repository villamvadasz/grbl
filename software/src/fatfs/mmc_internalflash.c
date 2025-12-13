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
#include <string.h>
#include "mal.h"
#include "diskio.h"
#include "c_fatfs.h"
#include "k_stdtype.h"

#ifdef __32MZ2048ECG144__
	#define NUMBER_OF_BYTES_IN_PAGE		  (16384uL)
#else
	#ifdef __32MX440F256H__
		#define NUMBER_OF_BYTES_IN_PAGE		  (4096uL)
	#else
		#ifdef __32MX460F512L__
			#define NUMBER_OF_BYTES_IN_PAGE		  (4096uL)
		#else
			#ifdef __32MX470F512L__
				#define NUMBER_OF_BYTES_IN_PAGE		  (4096uL)
			#else
				#ifdef __32MX470F512H__
					#define NUMBER_OF_BYTES_IN_PAGE		  (4096uL)
				#else
					#ifdef __32MZ2048EFM100__
						#define NUMBER_OF_BYTES_IN_PAGE		  (16384uL)
					#else
						#ifdef __32MX795F512H__
							#define NUMBER_OF_BYTES_IN_PAGE		  (4096uL)
						#else
							#error TODO define how big the flash is in that controller
						#endif
					#endif
				#endif
			#endif
		#endif
	#endif
#endif
	
#ifdef __32MX795F512H__
	//Half of the flash is reserved for disk drive.
	#define MMC_FLASH_ADDR (0x9D040000)
	#define MMC_FLASH_SIZE	((128uL)*(1024uL))
#endif


#define MMC_SECTOR_SIZE (512uL)
#define MMC_SECTOR_COUNT (MMC_FLASH_SIZE / MMC_SECTOR_SIZE)

#define MMC_PAGES_COUNT (MMC_FLASH_SIZE / NUMBER_OF_BYTES_IN_PAGE)
#define MMC_RATIO (NUMBER_OF_BYTES_IN_PAGE / MMC_SECTOR_SIZE)

typedef union _MMC_Page {
	uint32 pages[MMC_PAGES_COUNT][NUMBER_OF_BYTES_IN_PAGE / 4];//HW page sizes
	uint8 sectors[MMC_SECTOR_COUNT][MMC_SECTOR_SIZE];//FatFS Sector size
	uint32 raw[MMC_FLASH_SIZE / 4];
} MMC_Page;

static volatile DSTATUS InternalFlash_Stat = STA_NOINIT;	/* Disk status */
//const unsigned char *mmc_page_ptr = (const unsigned char *)MMC_FLASH_ADDR;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
	const MMC_Page mmc_page __attribute__ (( address(MMC_FLASH_ADDR), aligned(NUMBER_OF_BYTES_IN_PAGE) )) = {0};
#pragma GCC diagnostic pop

extern unsigned int NVMWriteWord(void* address, unsigned int data);
extern unsigned int NVMErasePage(void* address);


/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS InternalFlash_disk_initialize (
	void
)
{
	InternalFlash_Stat &= ~STA_NOINIT;
	return InternalFlash_Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS InternalFlash_disk_status (
	void
)
{
	return InternalFlash_Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT InternalFlash_disk_read (
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	DWORD sector,	/* Start sector number (LBA) */
	BYTE count		/* Sector count (1..255) */
)
{
	DWORD x = 0;
	for (x = sector; x < (sector + count); x++) {
		memcpy((void *)&buff[x * MMC_SECTOR_SIZE], (void *)&mmc_page.sectors[x], MMC_SECTOR_SIZE);
	}

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
uint32 mmc_page_cache[NUMBER_OF_BYTES_IN_PAGE / 4];
DWORD mmc_page_cache_item = -1;

DRESULT InternalFlash_disk_write (
	BYTE *buff,		/* Pointer to the data to be written */
	DWORD sector,			/* Start sector number (LBA) */
	BYTE count				/* Sector count (1..255) */
)
{
	DRESULT res = RES_ERROR;
	DWORD sector_act = 0;
	uint32 result = 0;
	uint32 y = 0;

	for (sector_act = sector; sector_act < (sector + count); sector_act ++) {
		uint32 offset = 0;
		DWORD page = sector_act / MMC_RATIO;
		
		if (mmc_page_cache_item != page) {
			//write old page out.
			if (mmc_page_cache_item != -1) {
				result = NVMErasePage((void*)mmc_page.pages[mmc_page_cache_item]);
				for (y = 0; y < (NUMBER_OF_BYTES_IN_PAGE / 4); y++) {
					result =  NVMWriteWord((void*)&mmc_page.pages[mmc_page_cache_item][y], mmc_page_cache[y]);
				}
			}
			//read new page in
			memcpy(mmc_page_cache, mmc_page.pages[page], NUMBER_OF_BYTES_IN_PAGE / 4);
			mmc_page_cache_item = page;
		}
		offset = (sector_act % MMC_RATIO) * MMC_SECTOR_SIZE / 4;
		memcpy(&mmc_page_cache[offset], buff, MMC_SECTOR_SIZE);
	}

	if (mmc_page_cache_item != -1) {
		result = NVMErasePage((void*)mmc_page.pages[mmc_page_cache_item]);
		for (y = 0; y < (NUMBER_OF_BYTES_IN_PAGE / 4); y++) {
			result =  NVMWriteWord((void*)&mmc_page.pages[mmc_page_cache_item][y], mmc_page_cache[y]);
		}
		mmc_page_cache_item = -1;
	}


	if (result) {
		//failed to write data
	} else {
		res = RES_OK;
	}

	return res;
}
#endif



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT InternalFlash_disk_ioctl (
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
	DRESULT res = 0;
	//BYTE n, csd[16];
	//BYTE *ptr = buff;
	//DWORD csz;


	if (InternalFlash_Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;
	switch (ctrl) {
		case CTRL_SYNC :	/* Flush write-back cache, Wait for end of internal process */
			res = RES_OK;
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (WORD) */
			*(DWORD*)buff = MMC_SECTOR_COUNT;
			res = RES_OK;
			break;

		case GET_SECTOR_SIZE :	/* Get sectors on the disk (WORD) */
			*(WORD*)buff = MMC_SECTOR_SIZE;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sectors (DWORD) */
			*(WORD*)buff = MMC_SECTOR_SIZE;
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

void InternalFlash_disk_timerproc (void)
{
}
