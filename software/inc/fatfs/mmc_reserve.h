#ifndef _MMC_RESERVE_H_
#define _MMC_RESERVE_H_

	extern DSTATUS Reserve_disk_initialize(void);
	extern DSTATUS Reserve_disk_status(void);
	extern DRESULT Reserve_disk_read(BYTE*, DWORD, BYTE);
	#if	_READONLY == 0
		extern DRESULT Reserve_disk_write(BYTE*, DWORD, BYTE);
	#endif
	extern DRESULT Reserve_disk_ioctl(BYTE, void*);

#endif
