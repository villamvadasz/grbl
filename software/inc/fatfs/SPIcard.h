#ifndef _SPICARD_H_
#define _SPICARD_H_

#include "k_stdtype.h"
#include "SD-SPI.h"

#define BYTE uint8
#define UINT16 uint16
#define UINT32 uint32

extern BYTE sd_spiMediaDetect(void);
extern MEDIA_INFORMATION * sd_spiMediaInitialize(void);
extern DWORD sd_spiReadCapacity(void);
extern WORD sd_spiReadSectorSize(void);
extern BYTE sd_spiSectorRead(DWORD sector_addr, BYTE* buffer);
extern BYTE sd_spiSectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero);
extern BYTE sd_spiWriteProtectState(void);
extern BYTE sd_spiShutdownMedia(void);

extern void init_sd_spi(void);
extern void deinit_sd_spi(void);
extern void do_sd_spi(void);
extern void isr_sd_spi_1ms(void);

#endif
