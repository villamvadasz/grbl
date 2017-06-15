#include "grbl_serial.h"

#include "TCPIP.h"
#include "k_stdtype.h"
#include "ringBuffer.h"
#include "mal.h"
#include "ethernet.h"

#define RX_LEN_ETHERNET 512
#define TX_LEN_ETHERNET 512

typedef enum _BSDServerState {
	BSD_INIT = 0,
	BSD_CREATE_SOCKET,
	BSD_BIND,
	BSD_LISTEN,
	BSD_OPERATION
} BSDServerState;

unsigned int serverPort = 5123;    // server port
#define MAX_CLIENT (1) // Maximum number of simultanous connections accepted by the server.
SOCKET bsdServerSocket;   
SOCKET ClientSock[MAX_CLIENT];
struct sockaddr_in addr;
struct sockaddr_in addRemote;
int addrlen = sizeof(struct sockaddr_in);
BSDServerState bsdServerState = BSD_INIT;

RingBuffer myRingBuffer_eth_tx;
unsigned char eth_tx[TX_LEN_ETHERNET];
RingBuffer myRingBuffer_eth_rx;
unsigned char eth_rx[RX_LEN_ETHERNET];

unsigned long missedTxCharEthernet = 0;
unsigned long missedRxCharEthernet = 0;

unsigned char ReceivedDataBuffer_grbl_ethernet_multi[64];
unsigned char ToSendDataBuffer_grbl_ethernet_multi[64];
unsigned int ToSendDataBuffer_grbl_ethernet_multi_filled = 0;

volatile unsigned int do_serial_grbl_ethernet_multi_1ms = 0;
volatile unsigned int do_serial_grbl_ethernet_multi_100ms = 0;

int isCharOutBuffer_grbl_ethernet_multi(void);
void ethernet_grbl_multi_loadbufferTx(int isocket);
void ethernet_grbl_multi_loadbufferRx(int isocket);

//Compile this file only if GRBL is compiled also

void init_serial_grbl_ethernet_multi(void) {
	ringBuffer_initBuffer(&myRingBuffer_eth_tx, eth_tx, sizeof(eth_tx) / sizeof(*eth_tx));
	ringBuffer_initBuffer(&myRingBuffer_eth_rx, eth_rx, sizeof(eth_rx) / sizeof(*eth_rx));
}

void do_serial_grbl_ethernet_multi(void) {
	if (do_serial_grbl_ethernet_multi_100ms) {
		do_serial_grbl_ethernet_multi_100ms = 0;
	}
	if (do_serial_grbl_ethernet_multi_1ms) {
		do_serial_grbl_ethernet_multi_1ms = 0;
		{
			switch(bsdServerState) {
				case BSD_INIT: {
					if (ethernet_is_initialized()) {
						int i = 0;
						// Initialize all client socket handles so that we don't process 
						// them in the BSD_OPERATION state
						for (i = 0; i < MAX_CLIENT; i++) {
							ClientSock[i] = INVALID_SOCKET;
						}
							
						bsdServerState = BSD_CREATE_SOCKET;
					} else {
						break;
					}
					// No break needed
				}
				case BSD_CREATE_SOCKET: {
					// Create a socket for this server to listen and accept connections on
					bsdServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					if (bsdServerSocket == INVALID_SOCKET) {
						break;
					} else {
						bsdServerState = BSD_BIND;
					}
					
					// No break needed
				}
				case BSD_BIND: {
					// Bind socket to a local port
					addr.sin_port = serverPort;
					addr.sin_addr.S_un.S_addr = IP_ADDR_ANY;
					if( bind( bsdServerSocket, (struct sockaddr*)&addr, addrlen ) == SOCKET_ERROR ) {
						break;
					}
					
					bsdServerState = BSD_LISTEN;
					// No break needed
				}
				case BSD_LISTEN: {
					if(listen(bsdServerSocket, MAX_CLIENT) == 0) {
						bsdServerState = BSD_OPERATION;
					}

					// No break.  If listen() returns SOCKET_ERROR it could be because 
					// MAX_CLIENT is set to too large of a backlog than can be handled 
					// by the underlying TCP socket count (TCP_PURPOSE_BERKELEY_SERVER 
					// type sockets in TCPIPConfig.h).  However, in this case, it is 
					// possible that some of the backlog is still handleable, in which 
					// case we should try to accept() connections anyway and proceed 
					// with normal operation.
				}
				case BSD_OPERATION: {
					int isocket = 0; 
					for (isocket = 0; isocket < MAX_CLIENT; isocket++) {
						unsigned int newConnection = 0;
						// Accept any pending connection requests, assuming we have a place to store the socket descriptor
						if(ClientSock[isocket] == INVALID_SOCKET) {
							newConnection = 1;
							ClientSock[isocket] = accept(bsdServerSocket, (struct sockaddr*)&addRemote, &addrlen);
						}
						
						// If this socket is not connected then no need to process anything
						if(ClientSock[isocket] == INVALID_SOCKET) {
							continue;
						}

						if (newConnection) {
							newConnection = 0;
							{
								extern void grlb_notify_new_connection(void);
								grlb_notify_new_connection();
							}
						}
						{
							ethernet_grbl_multi_loadbufferRx(isocket);
							ethernet_grbl_multi_loadbufferTx(isocket);	
						}
					}
					break;
				}
				default: {
					bsdServerState = BSD_INIT;
					break;
				}
			}
		}
	}
}

void isr_serial_grbl_ethernet_multi_1ms(void) {
	do_serial_grbl_ethernet_multi_1ms = 1;
	{
		static unsigned int do_serial_grbl_ethernet_multi_100ms_cnt = 0;
		do_serial_grbl_ethernet_multi_100ms_cnt++;
		if (do_serial_grbl_ethernet_multi_100ms_cnt >= 100) {
			do_serial_grbl_ethernet_multi_100ms_cnt = 0;
			do_serial_grbl_ethernet_multi_100ms = 1;
		}
	}
}

void ethernet_grbl_multi_loadbufferRx(int isocket) {
	int length = 0;						
	// For all connected sockets, receive and send back the data
	length = recv( ClientSock[isocket], (char *)ReceivedDataBuffer_grbl_ethernet_multi, sizeof(ReceivedDataBuffer_grbl_ethernet_multi), 0);
	if (length == SOCKET_DISCONNECTED) {
		closesocket( ClientSock[isocket] );
		ClientSock[isocket] = INVALID_SOCKET;
	} else if (length == SOCKET_ERROR) {
		closesocket( ClientSock[isocket] );
		ClientSock[isocket] = INVALID_SOCKET;
	} else if (length >= 0) {
		{
			unsigned char data = 0;
			uint32 iloop = length;
			uint32 jloop = 0;
			while (iloop) {
				data = ReceivedDataBuffer_grbl_ethernet_multi[jloop];
				if (ringBuffer_addItem(&myRingBuffer_eth_rx, data) != -1) {
				} else {
					missedRxCharEthernet++;
					{
						extern void debug_missed_char_logger(unsigned char ch);
						debug_missed_char_logger(data);
					}
				}
				iloop--;
				jloop++;
			}
		}
	} else {
		closesocket( ClientSock[isocket] );
		ClientSock[isocket] = INVALID_SOCKET;
	}
}

void ethernet_grbl_multi_loadbufferTx(int isocket) {
	if (ToSendDataBuffer_grbl_ethernet_multi_filled) {
		int resultSend = send(ClientSock[isocket], (char *)ToSendDataBuffer_grbl_ethernet_multi, ToSendDataBuffer_grbl_ethernet_multi_filled, 0);
		if (resultSend == ToSendDataBuffer_grbl_ethernet_multi_filled) {
			ToSendDataBuffer_grbl_ethernet_multi_filled = 0;
		} else if (resultSend == SOCKET_ERROR) {
			//some error happend, or not because if buffer was full then also this is returned
			//retry later
		} else if (resultSend == 0) {
			//retry next time also
		} else if (resultSend > 0) {
			unsigned int x = 0;
			for (x = 0; x < (ToSendDataBuffer_grbl_ethernet_multi_filled - resultSend); x++) {
				ToSendDataBuffer_grbl_ethernet_multi[x] = ToSendDataBuffer_grbl_ethernet_multi[resultSend + x];
			}
			ToSendDataBuffer_grbl_ethernet_multi_filled -= resultSend;
		} else {
			ToSendDataBuffer_grbl_ethernet_multi_filled = 0;
		}
	} else {
		if (isCharOutBuffer_grbl_ethernet_multi() != -1) {
			unsigned char data = 0;
			uint32 iloop = 0;
			memset(ToSendDataBuffer_grbl_ethernet_multi, 0x00 , 64);
			while (ringBuffer_getItem(&myRingBuffer_eth_tx, &data) != 0) {
				ToSendDataBuffer_grbl_ethernet_multi[iloop] = data;
				iloop++;
				if (iloop >= 64) {
					break;
				}
			}
			ToSendDataBuffer_grbl_ethernet_multi_filled = iloop;
			
		} else {
			//Nothing to do, wait for bytes
		}
	}
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
		{
			extern void debug_missed_char_logger(unsigned char ch);
			debug_missed_char_logger(data);
		}
	}
}
