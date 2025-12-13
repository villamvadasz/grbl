#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "k_stdtype.h"
#include "TCPIP.h"
#include "wifi.h"

#if defined(STACK_USE_WGET_FSIO)
#include "tmr.h"
#include "tokenize.h"
#include "wget.h"
#include "FSIO.h"

#define WGET_FSIO_SERVER_STR_LEN	128
#define WGET_FSIO_HEADER_LINE_LEN	128
#define WGET_FSIO_RX_LEN			(1024uL*4uL)
#define WGET_FSIO_CHUNGS_TO_WRITE 	512uL
#define WGET_FSIO_FILE_NAME_LENGTH	(8uL+1uL+3uL+1uL)

typedef enum _wget_fsio_States {
	SM_HOME = 0,
	SM_OPEN_FILE,
	SM_TRIGGER_NAME_RESOLVE,
	SM_NAME_RESOLVE,
	SM_ARP_START_RESOLVE,
	SM_ARP_RESOLVE,
	SM_SOCKET_OBTAIN,
	SM_SOCKET_OBTAINED,
	SM_PROCESS_RESPONSE,
	SM_DISCONNECT,
	SM_CLOSE_FILE,
	SM_DONE
} wget_fsio_states;

wget_fsio_states wget_fsio_State = SM_DONE;

char wget_fsio_ServerName[WGET_FSIO_SERVER_STR_LEN];
WORD wget_fsio_ServerPort = 80;
char wget_fsio_RemoteURL[WGET_FSIO_SERVER_STR_LEN];

uint8 wget_fsio_trigger_get = 0;
uint8 wget_fsio_data_is_expected = 0;
uint8 wget_fsio_cancel = 0;
uint8 wget_fsio_disconnect = 0;
uint32_t wget_fsio_Timer = 0;
uint8 wget_fsio_number_of_crlf = 0;
uint8 wget_fsio_number_of_reached = 0;
unsigned int wget_fsio_line_len = 0;
unsigned int wget_fsio_find_len = 0;
const char wget_fsio_find_content_str[] = "Content-Length:";
uint32 wget_fsio_find_content_str_pos = 0;
uint32 wget_fsio_content_length = 0;
uint8 wget_fsio_process_char_state = 0;
unsigned char wget_fsio_wasError = 0;
unsigned char wget_fsio_rx[WGET_FSIO_RX_LEN];
unsigned int wget_fsio_rx_pos = 0;
TCP_SOCKET wget_fsio_MySocket = INVALID_SOCKET;
NODE_INFO wget_fsio_Server;
char wget_fsio_header_line[WGET_FSIO_HEADER_LINE_LEN];
uint32 wget_fsio_header_line_pos = 0;
FSFILE * wget_fsio_dst = NULL;
char wget_fsio_filename[WGET_FSIO_FILE_NAME_LENGTH];

void wget_fsio_process_char(char ch);
void wget_fsio_process_line(char *line);

void init_wget_fsio(void) {
}

void do_wget_fsio(void) {
	switch(wget_fsio_State) {
		case SM_HOME: {
			if (wget_fsio_trigger_get) {
				wget_fsio_trigger_get = 0;
				wget_fsio_State = SM_OPEN_FILE;
			}
			break;
		}
		case SM_OPEN_FILE : {
			wget_fsio_dst = FSfopen(wget_fsio_filename, FS_WRITE);			
			if (wget_fsio_dst != NULL) {
				wget_fsio_State = SM_TRIGGER_NAME_RESOLVE;
			}	
			break;
		}
		case SM_TRIGGER_NAME_RESOLVE : {
			wget_fsio_cancel = 0;
			wget_fsio_disconnect = 0;
			// Obtain ownership of the DNS resolution module
			if(!DNSBeginUsage()) {
				break;
			}

			// Obtain the IP address associated with the common ServerName
			DNSResolve((unsigned char *)wget_fsio_ServerName, DNS_TYPE_A);
			wget_fsio_process_char_state = 0;
			wget_fsio_State = SM_NAME_RESOLVE;
			break;
		}
		case SM_NAME_RESOLVE: {
			// Wait for the DNS server to return the requested IP address
			if(!DNSIsResolved(&wget_fsio_Server.IPAddr)) {
				break;
			}

			// Release the DNS module, we no longer need it
			if(!DNSEndUsage()) {
				// An invalid IP address was returned from the DNS 
				// server.  Quit and fail permanantly if host is not valid.
				wget_fsio_State = SM_DONE;
				break;
			}
			if (wget_fsio_cancel) {
				wget_fsio_State = SM_DONE;
			} else {
				wget_fsio_State = SM_ARP_START_RESOLVE;
			}
			break;
		}
		case SM_ARP_START_RESOLVE: {
			if (wget_fsio_cancel) {
				wget_fsio_State = SM_DONE;
			} else {
				// Obtain the MAC address associated with the server's IP address (either direct MAC address on same subnet, or the MAC address of the Gateway machine)
				ARPResolve(&wget_fsio_Server.IPAddr);
				wget_fsio_Timer = getGlobalTime();
				wget_fsio_State = SM_ARP_RESOLVE;
			}
			break;
		}
		case SM_ARP_RESOLVE: {
			// Wait for the MAC address to finish being obtained
			if(!ARPIsResolved(&wget_fsio_Server.IPAddr, &wget_fsio_Server.MACAddr)) {
				// Time out if too much time is spent in this state
				if((getGlobalTime()- wget_fsio_Timer) > 1000) {
					// Retransmit ARP request
					wget_fsio_State = SM_ARP_START_RESOLVE;
				}
				break;
			}
			if (wget_fsio_cancel) {
				wget_fsio_State = SM_DONE;
			} else {
				wget_fsio_State = SM_SOCKET_OBTAIN;
			}
			break;
		}
		case SM_SOCKET_OBTAIN: {
			// Connect a socket to the remote TCP server
			wget_fsio_MySocket = TCPConnect(&wget_fsio_Server, wget_fsio_ServerPort);
			
			// Abort operation if no TCP sockets are available
			// If this ever happens, incrementing MAX_TCP_SOCKETS in 
			// StackTsk.h may help (at the expense of more global memory 
			// resources).
			if (wget_fsio_MySocket == INVALID_SOCKET) {
				if (wget_fsio_cancel) {
					wget_fsio_State = SM_DONE;
				}
				break;
			}

			if (wget_fsio_cancel) {
				wget_fsio_State = SM_DISCONNECT;
			} else {
				wget_fsio_State = SM_SOCKET_OBTAINED;
			}
			wget_fsio_Timer = getGlobalTime();
			break;
		}
		case SM_SOCKET_OBTAINED: {
			// Wait for the remote server to accept our connection request
			if(!TCPIsConnected(wget_fsio_MySocket)) {
				// Time out if too much time is spent in this state
				if ((getGlobalTime()- wget_fsio_Timer) > 5000) {
					// Close the socket so it can be used by other modules
					TCPDisconnect(wget_fsio_MySocket);
					wget_fsio_MySocket = INVALID_SOCKET;
					if (wget_fsio_cancel) {
						wget_fsio_State = SM_DONE;
					} else {
						wget_fsio_State = SM_SOCKET_OBTAIN;
					}
				}
				break;
			}

			wget_fsio_Timer = getGlobalTime();

			// Make certain the socket can be written to
			if(!TCPIsPutReady(wget_fsio_MySocket)) {
				break;
			}
			
			// Place the application protocol data into the transmit buffer.  For this example, we are connected to an HTTP server, so we'll send an HTTP GET request.
			TCPPutROMString(wget_fsio_MySocket, (ROM BYTE*)"GET ");
			TCPPutROMString(wget_fsio_MySocket, wget_fsio_RemoteURL);
			TCPPutROMString(wget_fsio_MySocket, (ROM BYTE*)" HTTP/1.1\r\nHost: ");
			TCPPutString(wget_fsio_MySocket, (unsigned char *)wget_fsio_ServerName);
			TCPPutROMString(wget_fsio_MySocket, (ROM BYTE*)"\r\n\r\n");

			// Send the packet
			TCPFlush(wget_fsio_MySocket);
			if (wget_fsio_cancel) {
				wget_fsio_State = SM_DISCONNECT;
			} else {
				wget_fsio_number_of_crlf = 0;
				wget_fsio_number_of_reached = 0;
				wget_fsio_State = SM_PROCESS_RESPONSE;
			}
			break;
		}
		case SM_PROCESS_RESPONSE: {
			if(!TCPIsConnected(wget_fsio_MySocket)) {
				wget_fsio_State = SM_DISCONNECT;
				break;
			}
	
			if(!TCPIsGetReady(wget_fsio_MySocket)) {
				if (wget_fsio_cancel) {
					wget_fsio_State = SM_DISCONNECT;
				}
				break;
			}
			while (1) {
				uint8_t ch = 0;
				BOOL result = FALSE;
				result = TCPGet(wget_fsio_MySocket, &ch);
				if (result == TRUE) {
					wget_fsio_process_char(ch);
				} else {
					break;
				}
			}
			break;
		}
		case SM_DISCONNECT: {
			// Close the socket so it can be used by other modules
			// For this application, we wish to stay connected, but this state will still get entered if the remote server decides to disconnect
			wget_fsio_data_is_expected = 0;
			TCPDisconnect(wget_fsio_MySocket);
			wget_fsio_MySocket = INVALID_SOCKET;
			wget_fsio_State = SM_CLOSE_FILE;
			break;
		}
		case SM_CLOSE_FILE : {
			if (FSfclose(wget_fsio_dst) != 0) {
				wget_fsio_wasError = 1;
			}
			wget_fsio_State = SM_DONE;
			break;
		}
		case SM_DONE: {
			wget_fsio_data_is_expected = 0;
			wget_fsio_State = SM_HOME;
			break;
		}
		default : {
			wget_fsio_State = SM_HOME;
			break;
		}
	}
}

unsigned int wget_fsio_url(char *url, char *file) {
	int result = -1;
	if (wget_fsio_State == SM_HOME) {
		if ((url != NULL) && (file != NULL)) {
			unsigned int file_len = strlen(file);
			if (file_len <= ((sizeof(wget_fsio_filename) / sizeof(*wget_fsio_filename)) - 1) ) {
				strcpy(wget_fsio_filename, file);
				int len = strlen(url);
				if (len < WGET_FSIO_SERVER_STR_LEN) {
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
							memcpy(wget_fsio_ServerName, url, count);
							wget_fsio_ServerName[count] = 0;
							
							url += count;
							strcpy(wget_fsio_RemoteURL, (const char *)url);
							wget_fsio_ServerPort = 80;
		
							wget_fsio_process_char_state = 0;
							wget_fsio_data_is_expected = 1;
							wget_fsio_trigger_get = 1;
							result = 0;
						}
					}
				}
			}
		}
	}
	return result;
}

int wget_fsio_finished(void) {
	int result = -1; //busy

	if (wget_fsio_data_is_expected) {
		result = -1;
	} else {
		if (wget_fsio_wasError) {
			result = 1;//Error
		} else {
			result = 0;//Ok
		}
	}
	return result;
}

void wget_fsio_url_cancel(void) {
	wget_fsio_cancel = 1;
	wget_fsio_disconnect = 1;
}

void wget_fsio_process_char(char ch) {
	switch (wget_fsio_process_char_state) {
		case 0 : {
			uint8 line_found = 0;
			if ((ch == '\r') || (ch == '\n')) {
				wget_fsio_number_of_crlf++;
				if (wget_fsio_number_of_crlf >= 4) {
					wget_fsio_number_of_reached = 1;
					wget_fsio_process_char_state = 1;
					break;
				}
				ch = 0;
				line_found = 1;
			} else {
				wget_fsio_number_of_crlf = 0;
			}
			wget_fsio_header_line[wget_fsio_header_line_pos] = ch;
			wget_fsio_header_line_pos++;
			if (wget_fsio_header_line_pos >= (sizeof(wget_fsio_header_line) / sizeof(*wget_fsio_header_line)) ) {
				wget_fsio_header_line_pos = 0;
			}
			if (line_found) {
				line_found = 0;
				//Content-Length: 88
				if (*wget_fsio_header_line != 0) {
					wget_fsio_process_line(wget_fsio_header_line);
				}
				wget_fsio_header_line_pos = 0;
			}
			break;
		}
		case 1 : {
			wget_fsio_rx[wget_fsio_rx_pos] = ch;
			wget_fsio_rx_pos++;
			if (wget_fsio_rx_pos >= WGET_FSIO_CHUNGS_TO_WRITE) {
				size_t nWrite = 0;
				size_t len = wget_fsio_rx_pos;
				nWrite = FSfwrite(&wget_fsio_rx[0], 1, len, wget_fsio_dst);
				wget_fsio_rx_pos = 0;
				if (nWrite != len) {
					wget_fsio_wasError = 1;
				}
			}
			if (wget_fsio_content_length > 0) {
				wget_fsio_content_length--;
			}
			if (wget_fsio_content_length == 0) {
				wget_fsio_process_char_state = 2;
				wget_fsio_cancel = 1;
				if (wget_fsio_rx_pos != 0) {
					size_t nWrite = 0;
					size_t len = wget_fsio_rx_pos;
					nWrite = FSfwrite(&wget_fsio_rx[0], 1, len, wget_fsio_dst);
					wget_fsio_rx_pos = 0;
					if (nWrite != len) {
						wget_fsio_wasError = 1;
					}
				}
			}
			break;
		}
		case 2 : {//dump further chars
			break;
		}
		default : {
			wget_fsio_process_char_state = 0;
			break;
		}
	}
}

void wget_fsio_process_line(char *line) {
	if (line != NULL) {
		if (isStrPresentNonCaseSensitive(line, wget_fsio_find_content_str, 0, strlen(wget_fsio_find_content_str))) {
			uint32 i = 0;
			wget_fsio_content_length = 0;

			wget_fsio_line_len = strlen(line);
			wget_fsio_find_len = strlen(wget_fsio_find_content_str);

			for (i = wget_fsio_find_len; i < wget_fsio_line_len; i++) {
				if ((line[i] >= '0') && (line[i] <= '9'))  {
					wget_fsio_content_length *= 10;
					wget_fsio_content_length += line[i] - '0';
				}
			}
		}
	}
}

#endif
