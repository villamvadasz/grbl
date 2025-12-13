#ifndef _C_SPI_SW_H_
#define _C_SPI_SW_H_

	#include "mal.h"

	#define	SPI_SW_PORT_SYNC()	_sync()

	#define SPI_SW_USER_VS1053 1
	#define SPI_SW_USER_SDCARD 2
	#define SPI_SW_USER_XTP2046 3
	#define SPI_SW_USER_ILI9341 4
	#define SPI_SW_USER_XC1276 5
	#define SPI_SW_USER_NRF24l01 6
	#define SPI_SW_USER_SX1276 7

	#define SPI_SW_BRG_CONFIG_DEFAULT 4

	//SPI MODE		CPOL/CKP		CPHA	CKE
	//0				0               0       1
	//1				0               1       0
	//2				1               0       1
	//3				1               1       0
	
	//#define SPI_SW_MODE_0
	//#define SPI_SW_MODE_1
	//#define SPI_SW_MODE_2
	//#define SPI_SW_MODE_3
	
	//#ifdef SPI_SW_MODE_0
	//	#define	SPI_SW_CKP_CONFIG_DEFAULT	0
	//	#define SPI_SW_CKE_CONFIG_DEFAULT	1
	//#endif
	//#ifdef SPI_SW_MODE_1
	//	#define	SPI_SW_CKP_CONFIG_DEFAULT	0
	//	#define SPI_SW_CKE_CONFIG_DEFAULT	0
	//#endif
	//#ifdef SPI_SW_MODE_2
	//	#define	SPI_SW_CKP_CONFIG_DEFAULT	1
	//	#define SPI_SW_CKE_CONFIG_DEFAULT	1
	//#endif
	//#ifdef SPI_SW_MODE_3
	//	#define	SPI_SW_CKP_CONFIG_DEFAULT	1
	//	#define SPI_SW_CKE_CONFIG_DEFAULT	0
	//#endif
	
	
	//#define	SPI_SW_SMP_CONFIG_DEFAULT 1		//Data is sample at the end
	#define	SPI_SW_SMP_CONFIG_DEFAULT 0	//Data is sample at the middle
	
	#define	SPI_SW_CKP_CONFIG_DEFAULT 1		//Idle state for clock is a high level; active state is a low level
	//#define	SPI_SW_CKP_CONFIG_DEFAULT 0	//Idle state for clock is a low level; active state is a high level

	#define SPI_SW_CKE_CONFIG_DEFAULT	1		//Serial output data changes on transition from active clock state to Idle clock state. see CKP
	//#define SPI_SW_CKE_CONFIG_DEFAULT 0		//Serial output data changes on transition from Idle clock state to active clock state. see CKP

	#define SPI_SW_SCK_TRIS	TRISFbits.TRISF1
	#define SPI_SW_SDI_TRIS	TRISFbits.TRISF1
	#define SPI_SW_SDO_TRIS	TRISFbits.TRISF1

#endif
