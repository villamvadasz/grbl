#ifndef _ETHERNET_H_
#define _ETHERNET_H_

	#include "GenericTypeDefs.h"
	#include "c_ethernet.h"

	extern BYTE SerializedMACAddress[6];

	extern void init_ethernet(void);
	extern unsigned int init_ethernet_Asynch(void);
	extern void do_ethernet(void);
	extern void isr_ethernet_1ms(void);
	extern void deinit_ethernet(void);

	extern void ethernet_set_mac_address(unsigned char *str);

	extern unsigned int ethernet_is_initialized(void);

#endif
