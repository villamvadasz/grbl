#ifndef _SERIAL_USB_H_
#define _SERIAL_USB_H_

	extern void init_serial_usb(void);
	extern void do_serial_usb(void);

	extern int isCharInBuffer_usb(void);
	extern int getChar_usb(void);
	extern void putChar_usb(unsigned char data);
	extern void putString_usb(char *str);

#endif
