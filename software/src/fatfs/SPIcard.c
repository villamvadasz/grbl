#include <stdio.h>
#include <string.h>
#include "c_fatfs.h"
#include "SPIcard.h"

#include "mal.h"
#include "diskio.h"

typedef enum _sd_spi_MDStates {
	MDStates_First = 0,
	MDStates_Slow_1,
	MDStates_Slow_2,
	MDStates_Slow_3,
	MDStates_Init,
	MDStates_Finish,
} sd_spi_MDStates;

unsigned int sd_spi_BRG_LOW = 20;
unsigned int sd_spi_BRG_HIGH = 4;
MEDIA_INFORMATION mediaInformation;
uint8 do_sd_spi_1ms = 0;
BYTE sd_spi_MediaDetected = 0;
WORD gMediaSectorSize = 0;
DWORD sd_spi_finalLBA = 0;
sd_spi_MDStates do_sd_spi_MediaDetectState = MDStates_First;

static void do_sd_spi_MediaDetect(void);

void init_sd_spi(void) {
	// Turn off the card
	SD_CS = 1;                     //Initialize Chip Select line
	_sync();
	SD_CS_TRIS = 0;           //Card Select - output
	_sync();
	
	sd_spi_BRG_LOW = SPICalculateBRG(SPI_FREQUENCY_SLOW);
	sd_spi_BRG_HIGH = SPICalculateBRG(SPI_FREQUENCY);
	
	mediaInformation.errorCode = MEDIA_DEVICE_NOT_PRESENT;
	gMediaSectorSize = 0;
	sd_spi_finalLBA = 0;
}

void deinit_sd_spi(void) {
	// Turn off the card
	SD_CS_TRIS = 1;           //Card Select - output
	_sync();
}

void do_sd_spi(void) {
	if (do_sd_spi_1ms) {
		do_sd_spi_1ms = 0;
		do_sd_spi_MediaDetect();
	}
}

void isr_sd_spi_1ms(void) {
	do_sd_spi_1ms = 1;
}

BYTE sd_spi_MediaDetect(void) {
	return sd_spi_MediaDetected;
}

MEDIA_INFORMATION * sd_spi_MediaInitialize(void) {
	MEDIA_INFORMATION * result = NULL;
	
	if (mediaInformation.errorCode == MEDIA_NO_ERROR) {
		result = &mediaInformation;
	}
	return result;
}

DWORD sd_spi_ReadCapacity(void) {
    return sd_spi_finalLBA;
}

WORD sd_spi_ReadSectorSize(void) {
    return gMediaSectorSize;
}

BYTE sd_spi_SectorRead(DWORD sector_addr, BYTE* buffer) {
	BYTE result = 0;
	return result;
}

BYTE sd_spi_SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero) {
	BYTE result = 0;
	return result;
}

BYTE sd_spi_WriteProtectState(void) {
	return(0);
}

BYTE sd_spi_ShutdownMedia(void) {
	mediaInformation.errorCode = MEDIA_DEVICE_NOT_PRESENT;
	gMediaSectorSize = 0;
	sd_spi_finalLBA = 0;
	SD_CS = 1;
	return 0;
}

static void do_sd_spi_MediaDetect(void) {
	switch (do_sd_spi_MediaDetectState) {
		case MDStates_First : {
			//Check if card is already detected or not
			if (sd_spi_MediaDetect() == 0) {
				do_sd_spi_MediaDetectState = MDStates_Slow_1;
			} else {
				do_sd_spi_MediaDetectState = MDStates_Finish;
			}
			break;
		}
		case MDStates_Slow_1 : {
			//start with initialization
			do_sd_spi_MediaDetectState = MDStates_Slow_2;
			break;
		}
		case MDStates_Slow_2 : {
			break;
		}
		case MDStates_Slow_3 : {
			break;
		}
		case MDStates_Init : {
			break;
		}
		case MDStates_Finish : {
			//issue some spi command to detect the card agin.
			do_sd_spi_MediaDetectState = MDStates_First;
			break;
		}
		default : {
			do_sd_spi_MediaDetectState = MDStates_First;
			break;
		}
	}
}
