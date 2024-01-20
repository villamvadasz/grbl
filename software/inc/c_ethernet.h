#ifndef _C_ETHERNET_H_
#define _C_ETHERNET_H_

	#define ETHERNET_USE_ASYNC_INIT
	#if defined(DEFAULTS_PIC32_MX_CNC_1_0_2) || defined(DEFAULTS_PIC32_MX_CNC_1_0_2_SIMULATOR)
		#define ETHERNET_CNC_1_0_2
	#else
		#define ETHERNET_OLIMEX
	#endif

	#define C_ETHERNET_HOST_NAME			"cnc_1_0_0"

#endif
