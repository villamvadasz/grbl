#include "fatfs.h"
#include "c_fatfs.h"

#include "mmc_multiplexer.h"
#include "ff.h"
#ifdef FATFS_USE_SDCARD
	#include "SD-SPI.h"
	#include "mmc_sdcard.h"
	#include "SDcard.h"
#endif

uint8 do_fatfs_1ms = 0;
DSTATUS MDD_SDSPI_diskDetected = STA_NOINIT;
FRESULT MDD_SDSPI_diskMounted = FR_INVALID_PARAMETER;
FATFS MDD_SDSPI_Fatfs;
uint8 MDD_SDSPI_fatfs_drv = 0; //FATFS_USE_SDCARD
uint8 MDD_SDSPI_fatfs_isDiskPresent = 0;
uint8 MDD_SDSPI_fatfs_isDiskMounted = 0;
uint8 MDD_SDSPI_fatfs_diskIsReady = 0;


DSTATUS MDD_InternalFlash_diskDetected = STA_NOINIT;
FRESULT MDD_InternalFlash_diskMounted = FR_INVALID_PARAMETER;
FATFS MDD_InternalFlash_Fatfs;
uint8 MDD_InternalFlash_fatfs_drv = 1; //FATFS_USE_INTERNALFLASH
uint8 MDD_InternalFlash_fatfs_isDiskPresent = 0;
uint8 MDD_InternalFlash_fatfs_isDiskMounted = 0;
uint8 MDD_InternalFlash_fatfs_diskIsReady = 0;

void init_fatfs(void) {
	#ifdef FATFS_USE_SDCARD
		init_sd_spi();
	#endif
	#ifdef FATFS_USE_SD4BITCARD
		MDD_SD4BIT_InitIO();
	#endif
}

void deinit_fatfs(void) {
	#ifdef FATFS_USE_SDCARD
		deinit_sd_spi();
	#endif
	#ifdef FATFS_USE_SD4BITCARD
		MDD_SD4BIT_deInitIO();
	#endif
}

void do_fatfs(void) {
	#ifdef FATFS_USE_SDCARD
		do_sd_spi();
		do_mmc_sdcard_microchip();
	#endif
	#ifdef FATFS_USE_SD4BITCARD
		do_mmc_sdcard_sdio();
	#endif

	if (do_fatfs_1ms) {
		do_fatfs_1ms = 0;
		#if defined(FATFS_USE_SDCARD) || defined(FATFS_USE_SD4BITCARD)
			{
				static BYTE MDD_SDSPI_MediaDetect_prev = 0;
				BYTE MDD_SDSPI_MediaDetect_current = 0;
		
				MDD_SDSPI_MediaDetect_current = MDD_SDSPI_MediaDetect();
				MDD_SDSPI_fatfs_diskIsReady = 0;
		
				if (MDD_SDSPI_MediaDetect_current != MDD_SDSPI_MediaDetect_prev) {
					f_mount(NULL, "", 0);
					MDD_SDSPI_fatfs_isDiskPresent = 0;
					MDD_SDSPI_fatfs_isDiskMounted = 0;
				} else {
					if ((MDD_SDSPI_fatfs_isDiskPresent) && (MDD_SDSPI_fatfs_isDiskMounted)) {
						if (MDD_SDSPI_MediaDetect()) {
							MDD_SDSPI_fatfs_diskIsReady = 1;
						}
					} else {
						if (MDD_SDSPI_fatfs_isDiskPresent == 0) {
							MDD_SDSPI_diskDetected = disk_initialize(MDD_SDSPI_fatfs_drv);
							if ((MDD_SDSPI_diskDetected & STA_NOINIT) == 0) {
								MDD_SDSPI_fatfs_isDiskPresent = 1;
							}
						} else if (MDD_SDSPI_fatfs_isDiskMounted == 0) {
							MDD_SDSPI_diskMounted = f_mount(&MDD_SDSPI_Fatfs, "", 0);
							if (MDD_SDSPI_diskMounted == FR_OK) {
								MDD_SDSPI_fatfs_isDiskMounted = 1;
							}
						} else {
							//can not be, next cycle will be first part of if
						}
					}
				}
				MDD_SDSPI_MediaDetect_prev = MDD_SDSPI_MediaDetect_current;
			}
		#endif
		#ifdef FATFS_USE_INTERNALFLASH
			{
				static unsigned int MDD_InternalFlash_singleshoot = 1;
				if (MDD_InternalFlash_singleshoot) {
					MDD_InternalFlash_singleshoot = 0;
				}
				if (MDD_InternalFlash_fatfs_isDiskPresent == 0) {
					MDD_InternalFlash_diskDetected = disk_initialize(MDD_InternalFlash_fatfs_drv);
					if ((MDD_InternalFlash_diskDetected & STA_NOINIT) == 0) {
						MDD_InternalFlash_fatfs_isDiskPresent = 1;
					}
				} else if (MDD_InternalFlash_fatfs_isDiskMounted == 0) {
					MDD_InternalFlash_diskMounted = f_mount(&MDD_InternalFlash_Fatfs, "1:/", 0);
					if (MDD_InternalFlash_diskMounted == FR_OK) {
						MDD_InternalFlash_fatfs_isDiskMounted = 1;
						{
							FRESULT res;        /* API result code */
							res = f_mkfs("1:/", 0, 512);
						}
					}
				} else {
					//can not be, next cycle will be first part of if
				}
			}
		#endif
	}
}

void isr_fatfs_1ms(void) {
	disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
	#ifdef FATFS_USE_SDCARD
		isr_sd_spi_1ms();
	#endif
	#ifdef FATFS_USE_SD4BITCARD
		isr_sd_4bit_1ms();
	#endif

	do_fatfs_1ms = 1;
}

uint8 fatfs_isDiskReady(void) {
	uint8 result = 0;
	result = MDD_SDSPI_fatfs_diskIsReady;
	return result;
}
