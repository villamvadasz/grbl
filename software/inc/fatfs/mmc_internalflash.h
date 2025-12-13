#ifndef _MMC_INTERNALFLASH_H_
#define _MMC_INTERNALFLASH_H_

	extern DSTATUS InternalFlash_disk_initialize(void);
	extern void InternalFlash_disk_timerproc (void);
	extern DSTATUS InternalFlash_disk_status(void);
	extern DRESULT InternalFlash_disk_read(BYTE*, DWORD, BYTE);
	#if	_READONLY == 0
		extern DRESULT InternalFlash_disk_write(const BYTE*, DWORD, BYTE);
	#endif
	extern DRESULT InternalFlash_disk_ioctl(BYTE, void*);

#endif
