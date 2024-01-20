#ifndef _C_MAIN_H_
#define _C_MAIN_H_

	#ifdef __32MZ2048ECG144__
		#define XTAL_12MHZ
		//#define XTAL_20MHZ
		#define SYS_FREQ_200MHZ
		#ifdef SYS_FREQ_200MHZ
			#define SYS_FREQ 			(200000000L)
			#define PB1_DIV         		2	//100MHz
			#define PB2_DIV         		5	//40MHz
			#define PB3_DIV         		2	//100MHz
			#define PB4_DIV         		2	//100MHz
			#define PB5_DIV         		2	//100MHz
			#define PB6_DIV         		2	//100MHz
			#define PB7_DIV         		1	//200MHz
			#define PB8_DIV         		1	//200MHz			

			#define GetSystemClock()		SYS_FREQ
			#define GetInstructionClock()	(GetPeripheral7Clock())
			#define GetPeripheral1Clock()	(GetSystemClock()/PB1_DIV)
			#define GetPeripheral2Clock()	(GetSystemClock()/PB2_DIV)
			#define GetPeripheral3Clock()	(GetSystemClock()/PB3_DIV)
			#define GetPeripheral4Clock()	(GetSystemClock()/PB4_DIV)
			#define GetPeripheral5Clock()	(GetSystemClock()/PB5_DIV)
			#define GetPeripheral6Clock()	(GetSystemClock()/PB6_DIV)
			#define GetPeripheral7Clock()	(GetSystemClock()/PB7_DIV)
			#define GetPeripheral8Clock()	(GetSystemClock()/PB8_DIV)

		#endif
	#else
		#define XTAL_8MHZ
		//#define XTAL_10MHZ
		//#define XTAL_20MHZ
		//#define INT_OSC_8MHZ
	
		//#define SYS_FREQ_10MHZ
		//#define SYS_FREQ_20MHZ
		//#define SYS_FREQ_40MHZ
		#define SYS_FREQ_80MHZ
	
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

		#define GetSystemClock()		SYS_FREQ
		#define GetInstructionClock()	(GetSystemClock()/1)
		#define GetPeripheralClock()	(GetSystemClock()/PB_DIV)

	#endif
#endif
