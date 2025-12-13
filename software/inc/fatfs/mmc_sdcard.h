#ifndef _MMC_SDCARD_H_
#define _MMC_SDCARD_H_

	extern void do_mmc_sdcard_microchip(void);
	extern void SDcard_disk_timerproc(void);
	extern DSTATUS SDcard_disk_initialize(void);
	extern DSTATUS SDcard_disk_status(void);
	extern DRESULT SDcard_disk_read(BYTE*, DWORD, BYTE);
	#if	_READONLY == 0
		extern DRESULT SDcard_disk_write(const BYTE*, DWORD, BYTE);
	#endif
	extern DRESULT SDcard_disk_ioctl(BYTE, void*);

#endif
