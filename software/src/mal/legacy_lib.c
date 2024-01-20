#include "mal.h"

char * ultoa(unsigned long val, char * buf, int base)
{
	unsigned	long	v;
	char		c;

	v = val;
	do {
		v /= base;
		buf++;
	} while(v != 0);
	*buf-- = 0;
	do {
		c = val % base;
		val /= base;
		if(c >= 10)
			c += 'A'-'0'-10;
		c += '0';
		*buf-- = c;
	} while(val != 0);
	return buf;
}

char *utoa(unsigned val, char * buf, int base)
{
	unsigned	v;
	char		c;

	v = val;
	do {
		v /= base;
		buf++;
	} while(v != 0);
	*buf-- = 0;
	do {
		c = val % base;
		val /= base;
		if(c >= 10)
			c += 'A'-'0'-10;
		c += '0';
		*buf-- = c;
	} while(val != 0);
	return buf;
}

char * itoa(int val, char * buf, int base)
{
	char *	cp = buf;

	if(val < 0) {
		*buf++ = '-';
		val = -val;
	}
	utoa(val, buf, base);
	return cp;
}
