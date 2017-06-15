#include "grbl_serial.h"

#include "TCPIP.h"
#include "k_stdtype.h"
#include "ringBuffer.h"
#include "mal.h"

#define RX_LEN_ETHERNET 512
#define TX_LEN_ETHERNET 512

UDP_PORT         serverPort = 5123;    // server port
UDP_SOCKET       udpSkt = INVALID_UDP_SOCKET;        // communication handle

RingBuffer myRingBuffer_eth_tx;
unsigned char eth_tx[TX_LEN_ETHERNET];
RingBuffer myRingBuffer_eth_rx;
unsigned char eth_rx[RX_LEN_ETHERNET];

unsigned long missedTxCharEthernet = 0;
unsigned long missedRxCharEthernet = 0;

unsigned char ReceivedDataBuffer_grbl_ethernet_multi[64];
unsigned char ToSendDataBuffer_grbl_ethernet_multi[64];

int isCharOutBuffer_grbl_ethernet_multi(void);

//Compile this file only if GRBL is compiled also

void init_serial_grbl_ethernet_multi(void) {
	ringBuffer_initBuffer(&myRingBuffer_eth_tx, eth_tx, sizeof(eth_tx) / sizeof(*eth_tx));
	ringBuffer_initBuffer(&myRingBuffer_eth_rx, eth_rx, sizeof(eth_rx) / sizeof(*eth_rx));
}

void do_serial_grbl_ethernet_multi(void) {
	static unsigned int do_serial_grbl_ethernet_multi_sm = 0;
	switch (do_serial_grbl_ethernet_multi) {
		case 0 : {
			if (ethernet_is_initialized()) {
				// UDP open
				udpSkt = UDPOpen(serverPort, 0, 0);
				if (udpSkt != INVALID_UDP_SOCKET) {
					do_serial_grbl_ethernet_multi = 1;
				}
			}
			break;
		}
		case 1 : {
			while (1) {
				INT avlBytes = 0;

				avlBytes = UDPIsGetReady(udpSkt);
				
				if(avlBytes >= 64) {
					avlBytes = 64;
				}	
						  
				if(avlBytes) {
					INT nBytes = 0;
					nBytes = UDPGetArray((BYTE*)ReceivedDataBuffer_grbl_ethernet_multi, avlBytes );
					if(nBytes) {
						unsigned char data = 0;
						uint32 i = nBytes;
						uint32 j = 0;
					
						while (i) {
							data = ReceivedDataBuffer_grbl_ethernet_multi[j];
							//putChar_grbl_ethernet_multi(data);
							if (ringBuffer_addItem(&myRingBuffer_eth_rx, data) != -1) {
							} else {
								 missedRxCharEthernet++;
							}
							i--;
							j++;
						}
					}
				} else {
					break;
				}
			}
			{
				INT nBytes = 0;
				if (isCharOutBuffer_grbl_ethernet_multi() != -1) {
					nBytes = UDPIsPutReady(udpSkt);
					if (nBytes >= 64) {
						unsigned char data = 0;
						uint32 i = 0;
						memset(ToSendDataBuffer_grbl_ethernet_multi, 0x00 , 64);
						while (ringBuffer_getItem(&myRingBuffer_eth_tx, &data) != 0) {
							ToSendDataBuffer_grbl_ethernet_multi[i] = data;
							i++;
							if (i >= 64) {
								break;
							}
						}
						nBytes = UDPPutArray( (UINT8*)&ToSendDataBuffer_grbl_ethernet_multi, i);
						UDPFlush();	
					}
				} else {

				}
			}
			do_serial_grbl_ethernet_multi = 2;
			break;
		}
		default : {
			do_serial_grbl_ethernet_multi = 0;
			break;
		}
	}
}

void isr_serial_grbl_ethernet_multi_1ms(void) {
}

int isCharInBuffer_grbl_ethernet_multi(void) {
	int result = -1;
	unsigned int cnt = ringBuffer_getFillness(&myRingBuffer_eth_rx);
	if (cnt != 0) {
		result = cnt;
	}
	return result;
}

int isCharOutBuffer_grbl_ethernet_multi(void) {
	int result = -1;
	unsigned int cnt = ringBuffer_getFillness(&myRingBuffer_eth_tx);
	if (cnt != 0) {
		result = cnt;
	}
	return result;
}

int getChar_grbl_ethernet_multi(void) {
	int result = -1;
	unsigned char data = 0;
	if (ringBuffer_getItem(&myRingBuffer_eth_rx, &data) != 0) {
		result = data;
	}
	return result;
}

void putChar_grbl_ethernet_multi(unsigned char data) {
	if (ringBuffer_addItem(&myRingBuffer_eth_tx, data) != -1) {
	} else {
		missedTxCharEthernet++;
	}
}
