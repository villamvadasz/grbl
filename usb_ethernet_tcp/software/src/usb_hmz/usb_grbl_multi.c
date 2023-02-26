#include "grbl_serial.h"
#include "c_serial_usb.h"

#include "k_stdtype.h"
#include "ringBuffer.h"
#include "mal.h"

RingBuffer myRingBuffer_hid_tx;
unsigned char hid_tx[TX_LEN_USB];
RingBuffer myRingBuffer_hid_rx;
unsigned char hid_rx[RX_LEN_USB];

unsigned long missedTxCharHID = 0;
unsigned long missedRxCharHID = 0;

int isCharOutBuffer_grbl_usb_multi(void);

__attribute__(( weak )) int isCharOutBuffer_usb(void) {return isCharOutBuffer_grbl_usb_multi();} 


void init_serial_grbl_usb_multi(void) {
	ringBuffer_initBuffer(&myRingBuffer_hid_tx, hid_tx, sizeof(hid_tx) / sizeof(*hid_tx));
	ringBuffer_initBuffer(&myRingBuffer_hid_rx, hid_rx, sizeof(hid_rx) / sizeof(*hid_rx));
}

void do_serial_grbl_usb_multi(void) {
}

void isr_serial_grbl_usb_multi_1ms(void) {
}

int isCharInBuffer_grbl_usb_multi(void) {
	int result = -1;
	unsigned int cnt = ringBuffer_getFillness(&myRingBuffer_hid_rx);
	if (cnt != 0) {
		result = cnt;
	}
	return result;
}

int isCharOutBuffer_grbl_usb_multi(void) {
	int result = -1;
	unsigned int cnt = ringBuffer_getFillness(&myRingBuffer_hid_tx);
	if (cnt != 0) {
		result = cnt;
	}
	return result;
}

int getChar_grbl_usb_multi(void) {
	int result = -1;
	unsigned char data = 0;
	if (ringBuffer_getItem(&myRingBuffer_hid_rx, &data) != 0) {
		result = data;
	}
	return result;
}

void putChar_grbl_usb_multi(unsigned char data) {
	if (ringBuffer_addItem(&myRingBuffer_hid_tx, data) != -1) {
	} else {
		missedTxCharHID++;
	}
}

