#ifndef _C_FATFS_H_
#define _C_FATFS_H_

#error You are using an example of c_fatfs.h

    // Define the SPI frequency
	#define SPI_FREQUENCY_SLOW		   400000
    #define SPI_FREQUENCY			(20000000)

    #define FATFS_USE_SPI
    //#define FATFS_USE_SPI2
    //#define FATFS_USE_SPI4

	//Pinguino-MICRO
    #define SD_CS				PORTBbits.RB13
    #define SD_CS_TRIS			TRISBbits.TRISB13

	//Pinguino-OTG
    //#define SD_CS				PORTBbits.RB13
    //#define SD_CS_TRIS			TRISBbits.TRISB13
    
	//VS1053 Shiled
    //#define SD_CS				PORTBbits.RB14
    //#define SD_CS_TRIS			TRISBbits.TRISB14
    
    //#define SD_CD				PORTFbits.RF13
    //#define SD_CD_TRIS			TRISFbits.TRISF13
    
    //#define SD_WE				PORTFbits.RF1
    //#define SD_WE_TRIS			TRISFbits.TRISF1

	//4bit interface
	//#define SD4BIT_CLK_PORT		PORTBbits.RB0
	//#define SD4BIT_CLK_LAT		LATBbits.LATB0
	//#define SD4BIT_CLK_TRIS		TRISBbits.TRISB0
	
	//#define SD4BIT_CMD_PORT		PORTBbits.RB1
	//#define SD4BIT_CMD_LAT		LATBbits.LATB1
	//#define SD4BIT_CMD_TRIS		TRISBbits.TRISB1
	
	//#define SD4BIT_DAT0_PORT	PORTBbits.RB2
	//#define SD4BIT_DAT0_LAT		LATBbits.LATB2
	//#define SD4BIT_DAT0_TRIS	TRISBbits.TRISB2

	//#define SD4BIT_DAT1_PORT	PORTBbits.RB3
	//#define SD4BIT_DAT1_LAT		LATBbits.LATB3
	//#define SD4BIT_DAT1_TRIS	TRISBbits.TRISB3

	//#define SD4BIT_DAT2_PORT	PORTBbits.RB4
	//#define SD4BIT_DAT2_LAT		LATBbits.LATB4
	//#define SD4BIT_DAT2_TRIS	TRISBbits.TRISB4
	
	//#define SD4BIT_DAT3_PORT	PORTBbits.RB5
	//#define SD4BIT_DAT3_LAT		LATBbits.LATB5
	//#define SD4BIT_DAT3_TRIS	TRISBbits.TRISB5
	
// The FS_MAX_FILES_OPEN #define is only applicable when Dynamic
// memeory allocation is not used (FS_DYNAMIC_MEM not defined).
// Defines how many concurent open files can exist at the same time.
// Takes up static memory. If you do not need to open more than one
// file at the same time, then you should set this to 1 to reduce
// memory usage
#define FS_MAX_FILES_OPEN 	3
/************************************************************************/

// The size of a sector
// Must be 512, 1024, 2048, or 4096
// 512 bytes is the value used by most cards
#define MEDIA_SECTOR_SIZE 		512
/************************************************************************/

// If FAT32 support required then uncomment the following
#define SUPPORT_FAT32
/* ******************************************************************************************************* */

    #define MDD_MediaInitialize     MDD_SDSPI_MediaInitialize
    #define MDD_MediaDetect         MDD_SDSPI_MediaDetect
    #define MDD_SectorRead          MDD_SDSPI_SectorRead
    #define MDD_SectorWrite         MDD_SDSPI_SectorWrite
    #define MDD_InitIO              MDD_SDSPI_InitIO
    #define MDD_deInitIO            MDD_SDSPI_deInitIO
    #define MDD_ShutdownMedia       MDD_SDSPI_ShutdownMedia
    #define MDD_WriteProtectState   MDD_SDSPI_WriteProtectState
    #define MDD_ReadSectorSize      MDD_SDSPI_ReadSectorSize
    #define MDD_ReadCapacity        MDD_SDSPI_ReadCapacity
    #define MDD_FINAL_SPI_SPEED     2000000

#define FATFS_USE_SD4BITCARD
#define FATFS_USE_SDCARD
#define FATFS_USE_INTERNALFLASH
#define FATFS_USE_EXTERNALFLASH
#define FATFS_USE_RESERVE

#endif
