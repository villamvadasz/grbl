#include "serial_usb.h"
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

int isCharOutBuffer_usb(void);

#include "usb.h"
#include "usb_function_cdc.h"
unsigned char ReceivedDataBuffer[64];
unsigned char ToSendDataBuffer[64];

void init_serial_usb(void) {
	ringBuffer_initBuffer(&myRingBuffer_hid_tx, hid_tx, sizeof(hid_tx) / sizeof(*hid_tx));
	ringBuffer_initBuffer(&myRingBuffer_hid_rx, hid_rx, sizeof(hid_rx) / sizeof(*hid_rx));
}

void do_serial_usb(void) {
	// User Application USB tasks
	if ((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) {
	} else {
		{
			if (isCharOutBuffer_usb() != -1) {
				if(USBUSARTIsTxTrfReady()) {
					unsigned char data = 0;
					uint32 i = 0;
					memset(ToSendDataBuffer, 0x00 , 64);
					while (ringBuffer_getItem(&myRingBuffer_hid_tx, &data) != 0) {
						ToSendDataBuffer[i] = data;
						i++;
						if (i >= 64) {
							break;
						}
					}
					putUSBUSART((char *)ToSendDataBuffer, i);
				}
			}
		}
		CDCTxService();
		{
			unsigned char cdcInCntWrite = getsUSBUSART((char *)ReceivedDataBuffer, (sizeof(ReceivedDataBuffer) / sizeof(*ReceivedDataBuffer)) );
			if (cdcInCntWrite != 0) {				//Check if data was received from the host.
				unsigned char data = 0;
				uint32 i = cdcInCntWrite;
				uint32 j = 0;
				while (i) {
					data = ReceivedDataBuffer[j];
					if (ringBuffer_addItem(&myRingBuffer_hid_rx, data) != -1) {
					} else {
						 missedRxCharHID++;
					}
					i--;
					j++;
				}
			}
		}
	}
}

int isCharInBuffer_usb(void) {
	int result = -1;
	unsigned int cnt = ringBuffer_getFillness(&myRingBuffer_hid_rx);
	if (cnt != 0) {
		result = cnt;
	}
	return result;
}

int isCharOutBuffer_usb(void) {
	int result = -1;
	unsigned int cnt = ringBuffer_getFillness(&myRingBuffer_hid_tx);
	if (cnt != 0) {
		result = cnt;
	}
	return result;
}

int getChar_usb(void) {
	int result = -1;
	unsigned char data = 0;
	if (ringBuffer_getItem(&myRingBuffer_hid_rx, &data) != 0) {
		result = data;
	}
	return result;
}

void putChar_usb(unsigned char data) {
	if (ringBuffer_addItem(&myRingBuffer_hid_tx, data) != -1) {
	} else {
		missedTxCharHID++;
	}
}

void putString_usb(char *str) {
	if (str != NULL) {
		unsigned int x = 0;
		unsigned int len = strlen(str);
		for (x = 0; x < len; x++) {
			putChar_usb(str[x]);
		}
	}
}
