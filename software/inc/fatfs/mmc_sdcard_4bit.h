#ifndef _MMC_SDCARD_4BIT_H_
#define _MMC_SDCARD_4BIT_H_

	extern void do_mmc_sdcard_sdio(void);
	extern void SD4Bitcard_disk_timerproc(void);
	extern DSTATUS SD4Bitcard_disk_initialize(void);
	extern DSTATUS SD4Bitcard_disk_status(void);
	extern DRESULT SD4Bitcard_disk_read(BYTE*, DWORD, BYTE);
	#if	_READONLY == 0
		extern DRESULT SD4Bitcard_disk_write(const BYTE*, DWORD, BYTE);
	#endif
	extern DRESULT SD4Bitcard_disk_ioctl(BYTE, void*);

#endif
