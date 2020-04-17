#ifndef _ETHERNET_H_
#define _ETHERNET_H_

//Dummy header file to stub real one for GRBL
extern unsigned char SerializedMACAddress[6];

extern void ethernet_set_mac_address(char *str);
extern unsigned char hexStringToChar(unsigned char *str);
#endif

