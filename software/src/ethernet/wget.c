#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "k_stdtype.h"
#include "TCPIP.h"
#include "ringBuffer.h"
#include "wifi.h"
#include "tmr.h"

#if defined(STACK_USE_WGET)
#include "tokenize.h"
#include "wget.h"

#define WGET_SERVER_STR_LEN	128
#define WGET_RX_LEN	(1024uL*4uL)

uint8 wget_number_of_crlf = 0;
uint8 wget_number_of_reached = 0;

char wget_ServerName[WGET_SERVER_STR_LEN];
WORD wget_ServerPort = 80;
char wget_RemoteURL[WGET_SERVER_STR_LEN];
uint32 wget_trigger_get = 0;
uint32 wget_data_is_expected = 0;

RingBuffer myRingBuffer_wget;
unsigned char wget_rx[WGET_RX_LEN];

unsigned int wget_cancel = 0;
unsigned int wget_disconnect = 0;

static uint32_t wget_Timer;
static TCP_SOCKET wget_MySocket = INVALID_SOCKET;
static NODE_INFO wget_Server;

typedef enum _wget_States {
	SM_HOME = 0,
	SM_TRIGGER_NAME_RESOLVE,
	SM_NAME_RESOLVE,
	SM_ARP_START_RESOLVE,
	SM_ARP_RESOLVE,
	SM_SOCKET_OBTAIN,
	SM_SOCKET_OBTAINED,
	SM_PROCESS_RESPONSE,
	SM_DISCONNECT,
	SM_WAIT_BUFFER_CONSUMED,
	SM_DONE
} wget_states;

wget_states wget_State = SM_HOME;

char wget_header_line[128];
uint32 wget_header_line_pos = 0;
const char wget_find_content_str[] = "Content-Length:";
uint32 wget_find_content_str_pos = 0;
uint32 wget_content_length = 0;
int wget_line_len = 0;
int wget_find_len = 0;
unsigned int wget_process_char_state = 0;

void wget_process_char(char ch);
void wget_process_line(char *line);
	
void init_wget(void) {
	ringBuffer_initBuffer(&myRingBuffer_wget, wget_rx, sizeof(wget_rx) / sizeof(*wget_rx));

	//wget_url("http://kyrk.villamvadasz.hu/_flash/test/release_440F256H.hex");
	//wget_set_servername("kyrk.villamvadasz.hu", 80, "_flash/test/release_440F256H.hex");
}

void do_wget(void) {
	switch(wget_State) {
		case SM_HOME: {
			if (wget_trigger_get) {
				wget_trigger_get = 0;
				wget_State = SM_TRIGGER_NAME_RESOLVE;
			}
			break;
		}
		case SM_TRIGGER_NAME_RESOLVE : {
			wget_cancel = 0;
			wget_disconnect = 0;
			// Obtain ownership of the DNS resolution module
			if(!DNSBeginUsage()) {
				break;
			}

			// Obtain the IP address associated with the common ServerName
			DNSResolve((unsigned char *)wget_ServerName, DNS_TYPE_A);
			wget_process_char_state = 0;
			wget_State = SM_NAME_RESOLVE;
			break;
		}
		case SM_NAME_RESOLVE: {
			// Wait for the DNS server to return the requested IP address
			if(!DNSIsResolved(&wget_Server.IPAddr)) {
				break;
			}

			// Release the DNS module, we no longer need it
			if(!DNSEndUsage()) {
				// An invalid IP address was returned from the DNS 
				// server.  Quit and fail permanantly if host is not valid.
				wget_State = SM_DONE;
				break;
			}
			if (wget_cancel) {
				wget_State = SM_DONE;
			} else {
				wget_State = SM_ARP_START_RESOLVE;
			}
			break;
		}
		case SM_ARP_START_RESOLVE: {
			if (wget_cancel) {
				wget_State = SM_DONE;
			} else {
				// Obtain the MAC address associated with the server's IP address (either direct MAC address on same subnet, or the MAC address of the Gateway machine)
				ARPResolve(&wget_Server.IPAddr);
				wget_Timer = getGlobalTime();
				wget_State = SM_ARP_RESOLVE;
			}
			break;
		}
		case SM_ARP_RESOLVE: {
			// Wait for the MAC address to finish being obtained
			if(!ARPIsResolved(&wget_Server.IPAddr, &wget_Server.MACAddr)) {
				// Time out if too much time is spent in this state
				if((getGlobalTime()- wget_Timer) > 1000) {
					// Retransmit ARP request
					wget_State = SM_ARP_START_RESOLVE;
				}
				break;
			}
			if (wget_cancel) {
				wget_State = SM_DONE;
			} else {
				wget_State = SM_SOCKET_OBTAIN;
			}
			break;
		}
		case SM_SOCKET_OBTAIN: {
			// Connect a socket to the remote TCP server
			wget_MySocket = TCPConnect(&wget_Server, wget_ServerPort);
			
			// Abort operation if no TCP sockets are available
			// If this ever happens, incrementing MAX_TCP_SOCKETS in 
			// StackTsk.h may help (at the expense of more global memory 
			// resources).
			if (wget_MySocket == INVALID_SOCKET) {
				if (wget_cancel) {
					wget_State = SM_DONE;
				}
				break;
			}

			if (wget_cancel) {
				wget_State = SM_DISCONNECT;
			} else {
				wget_State = SM_SOCKET_OBTAINED;
			}
			wget_Timer = getGlobalTime();
			break;
		}
		case SM_SOCKET_OBTAINED: {
			// Wait for the remote server to accept our connection request
			if(!TCPIsConnected(wget_MySocket)) {
				// Time out if too much time is spent in this state
				if ((getGlobalTime()- wget_Timer) > 5000) {
					// Close the socket so it can be used by other modules
					TCPDisconnect(wget_MySocket);
					wget_MySocket = INVALID_SOCKET;
					if (wget_cancel) {
						wget_State = SM_DONE;
					} else {
						wget_State = SM_SOCKET_OBTAIN;
					}
				}
				break;
			}

			wget_Timer = getGlobalTime();

			// Make certain the socket can be written to
			if(!TCPIsPutReady(wget_MySocket)) {
				break;
			}
			
			// Place the application protocol data into the transmit buffer.  For this example, we are connected to an HTTP server, so we'll send an HTTP GET request.
			TCPPutROMString(wget_MySocket, (ROM BYTE*)"GET ");
			TCPPutROMString(wget_MySocket, wget_RemoteURL);
			TCPPutROMString(wget_MySocket, (ROM BYTE*)" HTTP/1.1\r\nHost: ");
			TCPPutString(wget_MySocket, (unsigned char *)wget_ServerName);
			TCPPutROMString(wget_MySocket, (ROM BYTE*)"\r\n\r\n");

			// Send the packet
			TCPFlush(wget_MySocket);
			if (wget_cancel) {
				wget_State = SM_DISCONNECT;
			} else {
				wget_number_of_crlf = 0;
				wget_number_of_reached = 0;
				wget_State = SM_PROCESS_RESPONSE;
			}
			break;
		}
		case SM_PROCESS_RESPONSE: {
			if(!TCPIsConnected(wget_MySocket)) {
				wget_State = SM_DISCONNECT;
				break;
			}
	
			if(!TCPIsGetReady(wget_MySocket)) {
				if (wget_cancel) {
					wget_State = SM_DISCONNECT;
				}
				break;
			}
			while (1) {
				if (ringBuffer_getEmptyness(&myRingBuffer_wget)) {
					uint8_t ch = 0;
					BOOL result = FALSE;
					result = TCPGet(wget_MySocket, &ch);
					if (result == TRUE) {
						wget_process_char(ch);
					} else {
						break;
					}
				}
			}
			break;
		}
		case SM_DISCONNECT: {
			// Close the socket so it can be used by other modules
			// For this application, we wish to stay connected, but this state will still get entered if the remote server decides to disconnect
			wget_data_is_expected = 0;
			TCPDisconnect(wget_MySocket);
			wget_MySocket = INVALID_SOCKET;
			wget_State = SM_WAIT_BUFFER_CONSUMED;
			break;
		}
		case SM_WAIT_BUFFER_CONSUMED : {
			if (wget_disconnect) {
				wget_getByte();
			}
			unsigned int cnt = ringBuffer_getFillness(&myRingBuffer_wget);
			if (cnt == 0) {
				wget_State = SM_DONE;
			}
			break;
		}
		case SM_DONE: {
			wget_data_is_expected = 0;
			wget_State = SM_HOME;
			break;
		}
		default : {
			wget_State = SM_HOME;
			break;
		}
	}
}

int wget_url(char *url) {
	int result = -1;
	if (wget_State == SM_HOME) {
		if (url != NULL) {
			int len = strlen(url);
			if (len < WGET_SERVER_STR_LEN) {
				//http://kyrk.villamvadasz.hu/_flash/test/release_440F256H.hex
				//http://kyrk.villamvadasz.hu:8080/_flash/test/release_440F256H.hex
				//kyrk.villamvadasz.hu:8080/_flash/test/release_440F256H.hex
				if (strstr(url, "http://")) {
					//char endMarker = '/';
					char *end = NULL;
					url += 7;
	
					end = strchr(url, ':');
					if (end != NULL) {
						//kyrk.villamvadasz.hu:8080/_flash/test/release_440F256H.hex
						//endMarker = ':';
					} else {
						end = strchr(url, '/');
						if (end != NULL) {
							//kyrk.villamvadasz.hu/_flash/test/release_440F256H.hex
							//endMarker = '/';
						} else {
							result = 1;
							return result;
						}
					}
	
					if (end != NULL) {
						int count = (int)(end - url);
						memcpy(wget_ServerName, url, count);
						wget_ServerName[count] = 0;
						
						url += count;
						strcpy(wget_RemoteURL, (const char *)url);
						wget_ServerPort = 80;
	
						wget_process_char_state = 0;
						wget_data_is_expected = 1;
						wget_trigger_get = 1;
						result = 0;
					}
				}
			}
		}
	}
	return result;
}

int wget_getByte(void) {
	int result = -2;//Finished
	unsigned char data = 0;
	if (ringBuffer_getItem(&myRingBuffer_wget, &data) ) {
		result = data;
	} else {
		if (wget_data_is_expected) {
			result = -1;//Might be some more byte later so keep on calling me
		} else {
			result = -2;//Finished receiving
		}
	}
	return result;
}

void wget_url_cancel(void) {
	wget_cancel = 1;
	wget_disconnect = 1;
}

void wget_process_char(char ch) {
	switch (wget_process_char_state) {
		case 0 : {
			uint8 line_found = 0;
			if ((ch == '\r') || (ch == '\n')) {
				wget_number_of_crlf++;
				if (wget_number_of_crlf >= 4) {
					wget_number_of_reached = 1;
					wget_process_char_state = 1;
					break;
				}
				ch = 0;
				line_found = 1;
			} else {
				wget_number_of_crlf = 0;
			}
			wget_header_line[wget_header_line_pos] = ch;
			wget_header_line_pos++;
			if (wget_header_line_pos >= (sizeof(wget_header_line) / sizeof(*wget_header_line)) ) {
				wget_header_line_pos = 0;
			}
			if (line_found) {
				line_found = 0;
				//Content-Length: 88
				if (*wget_header_line != 0) {
					wget_process_line(wget_header_line);
				}
				wget_header_line_pos = 0;
			}
			break;
		}
		case 1 : {
			ringBuffer_addItem(&myRingBuffer_wget, ch);
			if (wget_content_length > 0) {
				wget_content_length--;
			}
			if (wget_content_length == 0) {
				wget_process_char_state = 2;
				wget_cancel = 1;
			}
			break;
		}
		case 2 : {//dump further chars
			break;
		}
		default : {
			wget_process_char_state = 0;
			break;
		}
	}
}

void wget_process_line(char *line) {
	if (line != NULL) {
		if (isStrPresentNonCaseSensitive(line, wget_find_content_str, 0, strlen(wget_find_content_str))) {
			uint32 i = 0;
			wget_content_length = 0;

			wget_line_len = strlen(line);
			wget_find_len = strlen(wget_find_content_str);

			for (i = wget_find_len; i < wget_line_len; i++) {
				if ((line[i] >= '0') && (line[i] <= '9'))  {
					wget_content_length *= 10;
					wget_content_length += line[i] - '0';
				}
			}
		}
	}
}

#endif
