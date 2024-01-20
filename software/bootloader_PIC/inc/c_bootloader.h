#ifndef _C_BOOTLOADER_H_
#define _C_BOOTLOADER_H_

	#ifndef OSC_USER_DEFINE
		#define XTAL_8MHZ
		//#define XTAL_10MHZ
		//#define XTAL_20MHZ
		#define INT_OSC_8MHZ

		//#define SYS_FREQ_10MHZ	//Bootloader does not work on this frequency
		//#define SYS_FREQ_20MHZ
		//#define SYS_FREQ_40MHZ
		#define SYS_FREQ_80MHZ
	#endif

	#ifdef SYS_FREQ_10MHZ
		#define SYS_FREQ 			(10000000L)
		#define PB_DIV         		2
	#endif
	#ifdef SYS_FREQ_20MHZ
		#define SYS_FREQ 			(20000000L)
		#define PB_DIV         		2
	#endif
	#ifdef SYS_FREQ_40MHZ
		#define SYS_FREQ 			(40000000L)
		#define PB_DIV         		2
	#endif
	#ifdef SYS_FREQ_80MHZ
		#define SYS_FREQ 			(80000000L)
		#define PB_DIV         		2
	#endif

	//#define BOOTLOADER_IS_AN_UPDATER_APPLICATION
	#define BOOTLOADER_DELAYED_AUTOAPPLICATION
	//#define BOOTLOADER_SWITCH_TRIGGER
	#define BOOTLOADER_WITH_DEE
	#define USE_GRBL_STUFFS

#endif
