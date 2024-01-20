#ifndef _FIXEDMEMORYADDRESS_H_
#define _FIXEDMEMORYADDRESS_H_

	#if defined(__32MZ2048ECG144__) || defined (__32MZ2048EFM100__)
		#define ADDRESS_bootloader_request_A				0x8007F000
		#define ADDRESS_bootloader_request_B				0x8007F010
		#define ADDRESS_bootloader_was_reset_called			0x8007F020
		#define ADDRESS_bootloader_MAC						0x8007F030
		#define ADDRESS_exceptionLog						0x8007F200
		#define ADDRESS_NOINITRAM_BASE_ADDRESS_1 			0x8007F300 //use this address, because this seams to retain the values. Lower addresses still got initialized
		#define ADDRESS_GRBL_EEPROM_SYS_POSITION_ADDRESS_1 	0x8007F440
		#define ADDRESS_GRBL_EEPROM_SYS_POSITION_ADDRESS_2 	0x8007F450
		#define ADDRESS_GRBL_EEPROM_SYS_POSITION_ADDRESS_3 	0x8007F460
		#define ADDRESS_NOINITRAM_TESTER					0x8007F470
		/*Do not change next address, since the bootloader exe must be also updated then.*/
		#define ADDRESS_EEPROM_FLASH_ADDR_CNC		        0x9D030000

		#define ADDRESS_VERSION_DATE                        0x9D008100
		#define ADDRESS_VERSION_TIME                        0x9D008120
		#define ADDRESS_VERSION_ID_FIX                      0x9D008140
		#define ADDRESS_VERSION_MAKEFILE_NAME               0x9D008160
		#define ADDRESS_FILE_CRC                            0x9D008190
		#define ADDRESS_FILE_PATH                           0x9D00819C
	#elif defined (__32MX440F256H__) || defined (__32MX460F512L__) || defined (__32MX470F512L__) || defined (__32MX470F512H__) || defined(__32MX795F512H__ )
		#define ADDRESS_bootloader_request_A				0xA0000F00
		#define ADDRESS_bootloader_request_B				0xA0000F10
		#define ADDRESS_bootloader_was_reset_called			0xA0000F20
		#define ADDRESS_bootloader_MAC						0xA0000F30
		#define ADDRESS_exceptionLog						0xA0000200
		#define ADDRESS_NOINITRAM_BASE_ADDRESS_1 			0xA0000300 //use this address, because this seams to retain the values. Lower addresses still got initialized
		#define ADDRESS_GRBL_EEPROM_SYS_POSITION_ADDRESS_1 	0xA0000440
		#define ADDRESS_GRBL_EEPROM_SYS_POSITION_ADDRESS_2 	0xA0000450
		#define ADDRESS_GRBL_EEPROM_SYS_POSITION_ADDRESS_3 	0xA0000460
		#define ADDRESS_NOINITRAM_TESTER					0xA0000470
		/*Do not change next address, since the bootloader exe must be also updated then.*/
		#define ADDRESS_EEPROM_FLASH_ADDR_LORA		        0x9D030000
		/*Do not change next address, since the bootloader exe must be also updated then.*/
		#define ADDRESS_EEPROM_FLASH_ADDR_THINGSPEAK        0x9D030000
		/*Do not change next address, since the bootloader exe must be also updated then.*/
		#define ADDRESS_EEPROM_FLASH_ADDR_CNC		        0x9D030000

		#define ADDRESS_VERSION_DATE                        0x9D008100
		#define ADDRESS_VERSION_TIME                        0x9D008120
		#define ADDRESS_VERSION_ID_FIX                      0x9D008140
		#define ADDRESS_VERSION_MAKEFILE_NAME               0x9D008160
		#define ADDRESS_FILE_CRC                            0x9D008190
		#define ADDRESS_FILE_PATH                           0x9D00819C

		
	#else
		#error PIC not defined
	#endif

#endif
