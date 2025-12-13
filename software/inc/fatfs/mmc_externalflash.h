#ifndef _MMC_EXTERNALFLASH_H_
#define _MMC_EXTERNALFLASH_H_

	extern DSTATUS ExternalFlash_disk_initialize(void);
	extern DSTATUS ExternalFlash_disk_status(void);
	extern DRESULT ExternalFlash_disk_read(BYTE*, DWORD, BYTE);
	#if	_READONLY == 0
		extern DRESULT ExternalFlash_disk_write(BYTE*, DWORD, BYTE);
	#endif
	extern DRESULT ExternalFlash_disk_ioctl(BYTE, void*);

#endif
