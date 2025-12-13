#ifndef _C_SPI2_H_
#define _C_SPI2_H_

	#error You have included an example of c_spi.h

	#include "mal.h"

	#define	SPI2_PORT_SYNC()	_sync()

	#define SPI2_USER_VS1053 1
	#define SPI2_USER_SDCARD 2
	#define SPI2_USER_XTP2046 3
	#define SPI2_USER_ILI9341 4
	#define SPI2_USER_XC1276 5
	#define SPI2_USER_NRF24l01 6
	#define SPI2_USER_ENC 7

	#define SPI2_BRG_CONFIG_DEFAULT 4

	#define	SPI2_CKP_CONFIG_DEFAULT 1		//Idle state for clock is a high level; active state is a low level
	//#define	SPI2_CKP_CONFIG_DEFAULT 0	//Idle state for clock is a low level; active state is a high level

	#define SPI2_CKE_CONFIG_DEFAULT	1		//Serial output data changes on transition from active clock state to Idle clock state. see CKP
	//#define SPI2_CKE_CONFIG_DEFAULT 0		//Serial output data changes on transition from Idle clock state to active clock state. see CKP

	#define SPI2_SCK_TRIS	TRISFbits.TRISF1
	#define SPI2_SDI_TRIS	TRISFbits.TRISF1
	#define SPI2_SDO_TRIS	TRISFbits.TRISF1
	
	#define SPI2_LOCK_COLLISION_CALLOUT()		;

#endif
