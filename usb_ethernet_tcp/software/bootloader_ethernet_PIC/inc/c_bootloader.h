#ifndef _C_BOOTLOADER_H_
#define _C_BOOTLOADER_H_

	#define XTAL_8MHZ
	//#define XTAL_10MHZ
	//#define XTAL_20MHZ
	//#define INT_OSC_8MHZ

	//#define BOOTLOADER_IS_AN_UPDATER_APPLICATION
	#define BOOTLOADER_DELAYED_AUTOAPPLICATION
	//#define BOOTLOADER_SWITCH_TRIGGER

	//#define WATCHDOG_WDI_TRIS TRISGbits.TRISG9
	//#define WATCHDOG_WDI_PORT PORTGbits.RG9
	//#define Trigger_WDI() (WATCHDOG_WDI_PORT = ((ReadCoreTimer() & 0x0200000) == 0))

#endif
