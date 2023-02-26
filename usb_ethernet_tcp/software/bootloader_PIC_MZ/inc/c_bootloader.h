#ifndef _C_BOOTLOADER_H_
#define _C_BOOTLOADER_H_

	#ifndef OSC_USER_DEFINE
		#ifdef __32MZ2048ECG144__
			//#define XOSC_24MHZ
			//#define XTAL_24MHZ
			#define XTAL_12MHZ
			//#define INT_OSC_8MHZ //here no USB, so check clock_switch function
			//#define INT_OSC_8MHZ_EC_24 //needs an external 24MHz or 12MHz clock source for USB
			//#define INT_OSC_8MHZ_HS_24 //needs an external 24MHz or 12MHz clock source for USB

			//#define SYS_FREQ_100MHZ
			#define SYS_FREQ_200MHZ
		#else
			//#define XOSC_24MHZ
			//#define XTAL_24MHZ
			//#define INT_OSC_8MHZ //here no USB, so check clock_switch function
			#define INT_OSC_8MHZ_EC_24 //needs an external 24MHz or 12MHz clock source for USB
			//#define INT_OSC_8MHZ_HS_24 //needs an external 24MHz or 12MHz clock source for USB

			#define SYS_FREQ_100MHZ
			//#define SYS_FREQ_200MHZ
		#endif
	#endif

	#ifdef SYS_FREQ_100MHZ
		#define SYS_FREQ 			(100000000L)
	#endif
	#ifdef SYS_FREQ_200MHZ
		#define SYS_FREQ 			(200000000L)
	#endif

	//#define BOOTLOADER_IS_AN_UPDATER_APPLICATION
	#define BOOTLOADER_DELAYED_AUTOAPPLICATION
	//#define BOOTLOADER_SWITCH_TRIGGER

#endif
