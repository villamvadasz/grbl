#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#include "c_bootloader.h"
#include "fixedmemoryaddress.h"

#define PROGRAM_FLASH_END_ADRESS (0x9D000000+BMXPFMSZ-1)

//-------User configurable macros begin---------
#ifdef BOOTLOADER_IS_AN_UPDATER_APPLICATION
	#define MAJOR_VERSION 9
	#define MINOR_VERSION 9
#else
	#define MAJOR_VERSION 3
	#define MINOR_VERSION 0
#endif

#ifndef BOOTLOADER_SIZE
	#error not defined
#endif

#ifdef BOOTLOADER_IS_AN_UPDATER_APPLICATION
	#define APP_FLASH_BASE_ADDRESS 	0x9D000000 
	#define APP_FLASH_END_ADDRESS   (0x9D000000uL + BOOTLOADER_SIZE)

	#define BOOT_FLASH_BASE_ADDRESS 	0x9FC00000 
	#define BOOT_FLASH_END_ADDRESS      0x9FC02000
	#ifdef BOOTLOADER_WITH_DEE
		#define EEPROM_FLASH_BASE_ADDRESS 	ADDRESS_BOOTLOADER_SKIP_ERASE_START
		#define EEPROM_FLASH_END_ADDRESS   (ADDRESS_BOOTLOADER_SKIP_ERASE_END - 1)
	#endif
#else
	#define APP_FLASH_BASE_ADDRESS 	(0x9D000000uL + BOOTLOADER_SIZE)
	#define APP_FLASH_END_ADDRESS   PROGRAM_FLASH_END_ADRESS
	#ifdef BOOTLOADER_WITH_DEE
		#define EEPROM_FLASH_BASE_ADDRESS 	ADDRESS_BOOTLOADER_SKIP_ERASE_START
		#define EEPROM_FLASH_END_ADDRESS   (ADDRESS_BOOTLOADER_SKIP_ERASE_END - 1)
	#endif
#endif
/* Address of  the Flash from where the application starts executing */
/* Rule: Set APP_FLASH_BASE_ADDRESS to _RESET_ADDR value of application linker script*/

// For PIC32MX1xx and PIC32MX2xx Controllers only
#ifdef BOOTLOADER_IS_AN_UPDATER_APPLICATION
	#define USER_APP_RESET_ADDRESS 	(0x9FC00000)
#else
	#define USER_APP_RESET_ADDRESS 	((0x9D000000uL + BOOTLOADER_SIZE) + 0x1000)
#endif



//-------User configurable macros end-----------



#endif
