#ifndef _VERSION_H_
#define _VERSION_H_

	#include "k_stdtype.h"

	typedef enum _SoftwareType {
		TYPE_GRBL = 0x0012,
		TYPE_MP3 = 0x0013,
		TYPE_ST32F7 = 0x0014,
		TYPE_COMM = 0x0015,
		TYPE_IL3820 = 0x0016,
		TYPE_BME280 = 0x0017,
		TYPE_ESP8266_1 = 0x0018,
		TYPE_ESP8266_2 = 0x0019,
		TYPE_BOOTLOADER_SDCARD = 0x001A,
		TYPE_SIO_COM_MASTER_DUMMY = 0x001B,
		TYPE_SIO_COM_SLAVE_DUMMY = 0x001C,
		TYPE_SIO_COM_MASTER_ESP8266 = 0x001D,
		TYPE_SIO_COM_MASTER_ESP8266_BOOTLOADER = 0x001E,
		TYPE_OLIMEX_MP3_VS1053 = 0x001F,
		TYPE_SANDBOX = 0x0020,
		TYPE_OLIMEX_LCD = 0x0021,
		TYPE_SIO_COM_MASTER_NRF24L01 = 0x0022,
		TYPE_SIO_COM_SLAVE_NRF24L01 = 0x0023,
		TYPE_OLIMEX_IIC_PLAY = 0x0024,
		TYPE_OLIMEX_PLC = 0x0025,
		TYPE_OLIMEX_WIFI = 0x0026,
		TYPE_OLIMEX_WIFI_THINGSPEAK = 0x0027,
		TYPE_OLIMEX_WS2812B_TESTING = 0x0028,
		TYPE_OLIMEX_MOD_RTC2 = 0x0029,
		TYPE_OLIMEX_LORA = 0x002A,
	} SoftwareType;

	typedef struct _SoftwareIdentification {
		uint16 year;
		uint8 month;
		uint8 day;
		uint16 version;
		SoftwareType softwareType;
	} SoftwareIdentification;

	extern uint8 VERSION_ID[]; //user must implement this
	extern SoftwareIdentification softwareIdentification;
	extern const char VERSION_DATE[];
	extern const char VERSION_TIME[];
	extern const char VERSION_ID_FIX[];
	extern const char VERSION_MAKEFILE_NAME[];

#endif
