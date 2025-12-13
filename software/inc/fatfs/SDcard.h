#ifndef _SDCARD_H_
#define _SDCARD_H_

#include "k_stdtype.h"
#include "SD-SPI.h"

#define BYTE uint8
#define UINT16 uint16
#define UINT32 uint32

extern void init_sd_4bit(void);
extern void do_sd_4bit(void);
extern void isr_sd_4bit_1ms(void);

extern void MDD_SD4BIT_InitIO(void);	//Called at System init

extern BYTE MDD_SD4BIT_MediaDetect(void);
extern MEDIA_INFORMATION * MDD_SD4BIT_MediaInitialize(void);
extern DWORD MDD_SD4BIT_ReadCapacity(void);
extern WORD MDD_SD4BIT_ReadSectorSize(void);
extern BYTE MDD_SDSPI_SectorRead(DWORD sector_addr, BYTE* buffer);
extern BYTE MDD_SDSPI_SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero);
extern BYTE MDD_SDSPI_WriteProtectState(void);
extern BYTE MDD_SDSPI_ShutdownMedia(void);

#endif //SDCARD_H
