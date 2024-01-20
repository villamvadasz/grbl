#ifndef _NOINITRAM_H_
#define _NOINITRAM_H_

	extern unsigned char noinitram_IsValid(void);
	extern int noinitram_getByte(unsigned int address);
	extern void noinitram_getByteArray(unsigned int address, unsigned char *data, unsigned int length);
	extern void noinitram_setByte(unsigned int address, unsigned char data);
	extern void noinitram_setByteArray(unsigned int address, unsigned char *data, unsigned int length);
	
#endif
