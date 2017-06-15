#include "grbl_serial.h"
#include "c_serial_usb.h"

typedef enum _GRBL_MULTI_SOURCE {
	GRBL_MULTI_SOURCE_USB = 0,
	GRBL_MULTI_SOURCE_ETHERNET,
} GRBL_MULTI_SOURCE;

extern void init_serial_grbl_usb_multi(void);
extern void do_serial_grbl_usb_multi(void);
extern void isr_serial_grbl_usb_multi_1ms(void);
extern int getChar_grbl_usb_multi(void);
extern void putChar_grbl_usb_multi(unsigned char data);

extern void init_serial_grbl_ethernet_multi(void);
extern void do_serial_grbl_ethernet_multi(void);
extern void isr_serial_grbl_ethernet_multi_1ms(void);
extern int isCharOutBuffer_grbl_ethernet_multi(void);
extern int getChar_grbl_ethernet_multi(void);
extern void putChar_grbl_ethernet_multi(unsigned char data);

GRBL_MULTI_SOURCE grbl_multi_source = GRBL_MULTI_SOURCE_USB;

__attribute__(( weak )) void init_serial_grbl_ethernet_multi(void) {} 
__attribute__(( weak )) void do_serial_grbl_ethernet_multi(void) {} 
__attribute__(( weak )) void isr_serial_grbl_ethernet_multi_1ms(void) {} 
__attribute__(( weak )) int getChar_grbl_ethernet_multi(void) {return -1;} 
__attribute__(( weak )) void putChar_grbl_ethernet_multi(unsigned char ch) {} 

__attribute__(( weak )) void init_serial_grbl_usb_multi(void) {} 
__attribute__(( weak )) void do_serial_grbl_usb_multi(void) {} 
__attribute__(( weak )) void isr_serial_grbl_usb_multi_1ms(void) {} 
__attribute__(( weak )) int getChar_grbl_usb_multi(void) {return -1;} 
__attribute__(( weak )) void putChar_grbl_usb_multi(unsigned char ch) {} 

void init_serial_grbl(void) {
	init_serial_grbl_usb_multi();
	init_serial_grbl_ethernet_multi();
}

void do_serial_grbl(void) {
	do_serial_grbl_usb_multi();
	do_serial_grbl_ethernet_multi();
}

void isr_serial_grbl_1ms(void) {
	isr_serial_grbl_usb_multi_1ms();
	isr_serial_grbl_ethernet_multi_1ms();
}

int getChar_grbl(void) {
	int result = -1;
	result = getChar_grbl_usb_multi();
	if (result != -1) {
		grbl_multi_source = GRBL_MULTI_SOURCE_USB;
	} else {
		result = getChar_grbl_ethernet_multi();
		if (result != -1) {
			grbl_multi_source = GRBL_MULTI_SOURCE_ETHERNET;
		}
	}
	return result;
}

void putChar_grbl(unsigned char data) {
	switch (grbl_multi_source) {
		case GRBL_MULTI_SOURCE_USB : {
			putChar_grbl_usb_multi(data);
		}
		case GRBL_MULTI_SOURCE_ETHERNET : {
			putChar_grbl_ethernet_multi(data);
		}
		default : {
			break;
		}
	}
}
