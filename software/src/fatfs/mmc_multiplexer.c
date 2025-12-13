#include "mal.h"
#include "diskio.h"
#include "c_fatfs.h"

#include "mmc_sdcard.h"
#include "mmc_sdcard_4bit.h"
#include "mmc_internalflash.h"
#include "mmc_externalflash.h"
#include "mmc_reserve.h"

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	DSTATUS Stat = STA_NOINIT;
	switch (drv) {
		case 0 : {//SDcard
			#ifdef FATFS_USE_SDCARD
				Stat = SDcard_disk_initialize();
			#endif
			#ifdef FATFS_USE_SD4BITCARD
				Stat = SD4Bitcard_disk_initialize();
			#endif
			break;
		}
		case 1 : {//InternalFlash
			#ifdef FATFS_USE_INTERNALFLASH
				Stat = InternalFlash_disk_initialize();
			#endif
			break;
		}
		case 2 : {//ExternalFlash
			#ifdef FATFS_USE_EXTERNALFLASH
				Stat = ExternalFlash_disk_initialize();
			#endif
			break;
		}
		case 3 : {//Reserve
			#ifdef FATFS_USE_RESERVE
				Stat = Reserve_disk_initialize();
			#endif
			break;
		}
		default : {
			break;
		}
	}
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	DSTATUS Stat = STA_NOINIT;
	switch (drv) {
		case 0 : {//SDcard
			#ifdef FATFS_USE_SDCARD
				Stat = SDcard_disk_status();
			#endif
			#ifdef FATFS_USE_SD4BITCARD
				Stat = SD4Bitcard_disk_status();
			#endif
			break;
		}
		case 1 : {//InternalFlash
			#ifdef FATFS_USE_INTERNALFLASH
				Stat = InternalFlash_disk_status();
			#endif
			break;
		}
		case 2 : {//ExternalFlash
			#ifdef FATFS_USE_EXTERNALFLASH
				Stat = ExternalFlash_disk_status();
			#endif
			break;
		}
		case 3 : {//Reserve
			#ifdef FATFS_USE_RESERVE
				Stat = Reserve_disk_status();
			#endif
			break;
		}
		default : {
			break;
		}
	}
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0) */
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	DWORD sector,	/* Start sector number (LBA) */
	UINT count		/* Sector count (1..255) */
)
{
	DRESULT Result = RES_NOTRDY;
	switch (drv) {
		case 0 : {//SDcard
			#ifdef FATFS_USE_SDCARD
				Result = SDcard_disk_read(buff, sector, count);
			#endif
			#ifdef FATFS_USE_SD4BITCARD
				Result = SD4Bitcard_disk_read(buff, sector, count);
			#endif
			break;
		}
		case 1 : {//InternalFlash
			#ifdef FATFS_USE_INTERNALFLASH
				Result = InternalFlash_disk_read(buff, sector, count);
			#endif
			break;
		}
		case 2 : {//ExternalFlash
			#ifdef FATFS_USE_EXTERNALFLASH
				Result = ExternalFlash_disk_read(buff, sector, count);
			#endif
			break;
		}
		case 3 : {//Reserve
			#ifdef FATFS_USE_RESERVE
				Result = Reserve_disk_read(buff, sector, count);
			#endif
			break;
		}
		default : {
			break;
		}
	}
	return Result;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE drv,				/* Physical drive nmuber (0) */
	const BYTE *buff,		/* Pointer to the data to be written */
	DWORD sector,			/* Start sector number (LBA) */
	UINT count				/* Sector count (1..255) */
)
{
	DRESULT Result = RES_NOTRDY;
	switch (drv) {
		case 0 : {//SDcard
			#ifdef FATFS_USE_SDCARD
				Result = SDcard_disk_write(buff, sector, count);
			#endif
			#ifdef FATFS_USE_SD4BITCARD
				Result = SD4Bitcard_disk_write(buff, sector, count);
			#endif
			break;
		}
		case 1 : {//InternalFlash
			#ifdef FATFS_USE_INTERNALFLASH
				Result = InternalFlash_disk_write(buff, sector, count);
			#endif
			break;
		}
		case 2 : {//ExternalFlash
			#ifdef FATFS_USE_EXTERNALFLASH
				Result = ExternalFlash_disk_write(buff, sector, count);
			#endif
			break;
		}
		case 3 : {//Reserve
			#ifdef FATFS_USE_RESERVE
				Result = Reserve_disk_write(buff, sector, count);
			#endif
			break;
		}
		default : {
			break;
		}
	}
	return Result;
}
#endif



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
	DRESULT Result = RES_NOTRDY;
	switch (drv) {
		case 0 : {//SDcard
			#ifdef FATFS_USE_SDCARD
				Result = SDcard_disk_ioctl(ctrl, buff);
			#endif
			#ifdef FATFS_USE_SD4BITCARD
				Result = SD4Bitcard_disk_ioctl(ctrl, buff);
			#endif
			break;
		}
		case 1 : {//InternalFlash
			#ifdef FATFS_USE_INTERNALFLASH
				Result = InternalFlash_disk_ioctl(ctrl, buff);
			#endif
			break;
		}
		case 2 : {//ExternalFlash
			#ifdef FATFS_USE_EXTERNALFLASH
				Result = ExternalFlash_disk_ioctl(ctrl, buff);
			#endif
			break;
		}
		case 3 : {//Reserve
			#ifdef FATFS_USE_RESERVE
				Result = Reserve_disk_ioctl(ctrl, buff);
			#endif
			break;
		}
		default : {
			break;
		}
	}
	return Result;
}
#endif


/*-----------------------------------------------------------------------*/
/* Device Timer Driven Procedure                                         */
/*-----------------------------------------------------------------------*/
/* This function must be called by timer interrupt in period of 1ms      */

void disk_timerproc (void)
{
	#ifdef FATFS_USE_SDCARD
		SDcard_disk_timerproc();
	#endif
	#ifdef FATFS_USE_SD4BITCARD
		SD4Bitcard_disk_timerproc();
	#endif
	#ifdef FATFS_USE_INTERNALFLASH
		InternalFlash_disk_timerproc();
	#endif
	#ifdef FATFS_USE_EXTERNALFLASH
		ExternalFlash_disk_timerproc();
	#endif
	#ifdef FATFS_USE_RESERVE
		Reserve_disk_timerproc();
	#endif
}

