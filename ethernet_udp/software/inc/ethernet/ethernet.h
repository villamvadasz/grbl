#ifndef _ETHERNET_H_
#define _ETHERNET_H_

#include "GenericTypeDefs.h"

extern BYTE SerializedMACAddress[6];

extern void init_ethernet(void);
extern void do_ethernet(void);
extern void isr_ethernet_1ms(void);

extern void ethernet_set_mac_address(unsigned char *str);
extern unsigned char hexStringToChar(unsigned char *str);

#endif
