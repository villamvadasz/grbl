/*********************************************************************
 *
 *	MQTT Internet Of Things (MQTT) Client
 *	Module for Microchip TCP/IP Stack
 *	-Provides ability to receive Emails
 *	-Reference: RFC 2821
 *
 *********************************************************************
 * FileName:		MQTT.c
 * Dependencies:	TCP, ARP, DNS, Tick
 * Processor:		PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:		Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:		 Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc. All rights reserved.
 * Copyright (C) 2013,2014 Cyberdyne. All rights reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i) the Software when embedded on a Microchip microcontroller or
 *	 digital signal controller product ("Device") which is
 *	 integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.

 *	https://developer.ibm.com/iot/recipes/improvise/
 *	https://internetofthings.ibmcloud.com/dashboard/#/organizations/mkxk7/devices
 *	https://developer.ibm.com/iot/recipes/improvise-registered-devices/
 *
 * Author				Date	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder	 3/03/06	Original
 * Howard Schlunder			11/2/06	Vastly improved for release
 * Dario Greggio				30/9/14	client MQTT (IoT), inizio
 * - A simple client for MQTT. Original Code - Nicholas O'Leary http://knolleary.net
 * - Adapted for Spark Core by Chris Howard - chris@kitard.com (Based on PubSubClient 1.9.1)
 * D. Guz							20/01/15 Fix, cleanup, testing
 ********************************************************************/
#define __MQTT_C

#include "tcpip.h"


#if defined(STACK_USE_MQTT_CLIENT)

#include "MQTT.h"
#include "tmr.h"

#include "generictypedefs.h"

/****************************************************************************
 Section:
	MQTT Client Configuration Parameters
 ***************************************************************************/
#define MQTT_PORT					1883					// Default port to use when unspecified
#define MQTT_PORT_SECURE	8883					// Default port to use when unspecified
#define TICK_MILLISECONDS 1
#define TICK_SECOND 1000
#define MQTT_SERVER_REPLY_TIMEOUT	(TICK_SECOND*8)		// How long to wait before assuming the connection has been dropped (default 8 seconds)


/****************************************************************************
 Section:
	MQTT Client Public Variables
 ***************************************************************************/
// The global set of MQTT_POINTERS.
// Set these parameters after calling MQTTBeginUsage successfully.
MQTT_POINTERS MQTTClient;

/****************************************************************************
 Section:
	MQTT Client Internal Variables
 ***************************************************************************/
// Message state machine for the MQTT Client
typedef enum _MQTTStateEnum {
	MQTT_HOME = 0,
	MQTT_BEGIN,
	MQTT_NAME_RESOLVE,
	MQTT_OBTAIN_SOCKET,
	MQTT_SOCKET_OBTAINED,
	MQTT_CONNECT,
	MQTT_CONNECT_ACK,
	MQTT_PING,
	MQTT_PINGRESP,
	MQTT_PING_ACK,
	MQTT_PUBLISH,
	MQTT_PUBLISH_ACK,
	MQTT_SUBSCRIBE,
	MQTT_SUBSCRIBE_ACK,
	MQTT_PUBACK,
	MQTT_PUBREC,
	MQTT_PUBCOMP,
	MQTT_UNSUBSCRIBE,
	MQTT_UNSUBSCRIBE_ACK,
	MQTT_DISCONNECT,
	MQTT_CLOSE,
	MQTT_RECONNECT,
	MQTT_IDLE
} MQTTStateEnum;

uint8 MQTTReadPacket(uint8 *mqtt_control_type, uint8 *mqtt_flags, uint16 *mqtt_packet_id, uint8 **variable_header, uint8 **payload, uint32 *length);

IP_ADDR MQTTServer;						// IP address of the remote MQTT server
TCP_SOCKET MySocket = INVALID_SOCKET;	// Socket currently in use by the MQTT client
BYTE MQTTBufferTX[MQTT_MAX_PACKET_SIZE];
BYTE MQTTBufferRX[MQTT_MAX_PACKET_SIZE];
WORD MQTTBufferRX_write_cnt = 0;
WORD MQTTBufferRX_variable_header_cnt = 0;
WORD MQTTBufferRX_payload_cnt = 0;
uint16 MQTTBufferRX_Packet_Id = 0;
WORD MQTT_lastActivity = 0;
WORD MQTT_lastReceivedActivity = 0;
WORD LastPingTick = 0;
MQTTStateEnum MQTTState = 0;
unsigned char topic_received[128];
WORD last_received_packet_length = 0;
unsigned char MQTT_packet_received = 0;
uint32 mqtt_not_implemented = 0;
unsigned char topic_callback[MQTT_MAX_PACKET_SIZE];
unsigned char payload_callback[MQTT_MAX_PACKET_SIZE];
sint32 MQTTPublish_MQTTPublish = MQTT_BUSY;
sint32 MQTTPublish_MQTTBegin = MQTT_BUSY;
sint32 MQTTPublish_MQTTEnd = MQTT_BUSY;
sint32 MQTTPublish_MQTTConnect = MQTT_BUSY;
sint32 MQTTPublish_MQTTPing = MQTT_BUSY;
sint32 MQTTPublish_MQTTSubscribe = MQTT_BUSY;
sint32 MQTTPublish_MQTTUnSubscribe = MQTT_BUSY;
sint32 MQTTPublish_MQTTDisconnect = MQTT_BUSY;
unsigned int MQTTFlagsMQTTInUse = 0;
unsigned int MQTTFlagsConnectedOnce = 0;
unsigned int MQTTFlagsPingOutstanding = 0;

/****************************************************************************
 Section:
	MQTT Client Internal Function Prototypes
 ***************************************************************************/


/****************************************************************************
 Section:
	MQTT Function Implementations
 ***************************************************************************/

/*****************************************************************************
 Function:
	BOOL MQTTBeginUsage(void)

 Summary:
	Requests control of the MQTT client module.

 Description:
	Call this function before calling any other MQTT Client APIs. This 
	function obtains a lock on the MQTT Client, which can only be used by
	one stack application at a time. Once the application is finished
	with the MQTT client, it must call MQTTEndUsage to release control
	of the module to any other waiting applications.
	
	This function initializes all the MQTT state machines and variables
	back to their default state.

 Precondition:
	None

 Parameters:
	None

 Return Values:
	TRUE - The application has successfully obtained control of the module
	FALSE - The MQTT module is in use by another application. Call 
		MQTTBeginUsage again later, after returning to the main program loop
 ***************************************************************************/
BOOL MQTTBeginUsage(void) {
	BOOL result = FALSE;
	
	if(MQTTFlagsMQTTInUse) {
	} else {
		MQTTFlagsMQTTInUse = 0;
		MQTTFlagsConnectedOnce = 0;
		MQTTFlagsPingOutstanding = 0;
		MQTTFlagsMQTTInUse = TRUE;
		MQTTState = MQTT_BEGIN;
		memset((void*)&MQTTClient, 0x00, sizeof(MQTTClient));
		MQTTClient.Ver = MQTTPROTOCOLVERSION;
		MQTTClient.KeepAlive = MQTT_KEEPALIVE_LONG;
		MQTTClient.MsgId = 1;
		result = TRUE;
	}
	return result;
}

/*****************************************************************************
 Function:
	WORD MQTTEndUsage(void)

 Summary:
	Releases control of the MQTT client module.

 Description:
	Call this function to release control of the MQTT client module once
	an application is finished using it. This function releases the lock
	obtained by MQTTBeginUsage, and frees the MQTT client to be used by 
	another application.

 Precondition:
	MQTTBeginUsage returned TRUE on a previous call.

 Parameters:
	None

 Return Values:
	MQTT_SUCCESS - A message was successfully sent
	MQTT_RESOLVE_ERROR - The MQTT server could not be resolved
	MQTT_CONNECT_ERROR - The connection to the MQTT server failed or was prematurely terminated
	1-199 and 300-399 - The last MQTT server response code BOH!
 ***************************************************************************/
WORD MQTTEndUsage(void) {

	if(!MQTTFlagsMQTTInUse) {
		return 0xFFFF;
	}

	// Release the DNS module, if in use
	if(MQTTState == MQTT_NAME_RESOLVE) {
		DNSEndUsage();
	}
	
	if(MQTTClient.bConnected) {
		MQTTDisconnect();
	}

	// Release the TCP socket, if in use
	if(MySocket != INVALID_SOCKET) {
		TCPDisconnect(MySocket);
		MySocket = INVALID_SOCKET;
	}
	
	// Release the MQTT module
	MQTTFlagsMQTTInUse = FALSE;
	MQTTState = MQTT_HOME;

	return MQTTPublish_MQTTEnd;
}

/*****************************************************************************
 Function:
	void MQTTTask(void)

 Summary:
	Performs any pending MQTT client tasks

 Description:
	This function handles periodic tasks associated with the MQTT client,
	such as processing initial connections and command sequences.

 Precondition:
	None

 Parameters:
	None

 Returns:
	None

 Remarks:
	This function acts as a task (similar to one in an RTOS). It
	performs its task in a co-operative manner, and the main application
	must call this function repeatedly to ensure that all open or new
	connections are served in a timely fashion.
 ***************************************************************************/
void MQTTTask(void) {
	static WORD nextMsgId;
	{
		{
			
			//Packet receiver
			uint16_t received_len = 0;
			if (MySocket != INVALID_SOCKET) {
				if (TCPIsConnected(MySocket)) {
					received_len = TCPIsGetReady(MySocket);
					if (received_len != 0) {
						uint16_t i = 0;
						for (i = 0; i < received_len; i++) {
							if (MQTTBufferRX_write_cnt < MQTT_MAX_PACKET_SIZE) {
								TCPGet(MySocket, &MQTTBufferRX[MQTTBufferRX_write_cnt++]);
								MQTT_lastReceivedActivity = getGlobalTime();
							} else {
								//Missing char
								memset(MQTTBufferRX, 0x00, MQTT_MAX_PACKET_SIZE);
								MQTTBufferRX_write_cnt = 0;
							}
						}
					}
				} else {
					MQTTBufferRX_write_cnt = 0;
				}
			} else {
				MQTTBufferRX_write_cnt = 0;
			}
			if (MQTTBufferRX_write_cnt) {
				WORD t = getGlobalTime();
				if (t - MQTT_lastReceivedActivity > TICK_MILLISECONDS * 250) {
					memset(MQTTBufferRX, 0x00, MQTT_MAX_PACKET_SIZE);
					MQTTBufferRX_write_cnt = 0;
				}
			}
		}
		{
			//Fixed Header
			uint8 MQTT_Control_Packet_Type = 0;
			uint8 MQTT_Flags = 0;
			uint32 MQTT_Remaining_Length = 0;
			//Packet parser
			if (MQTTBufferRX_write_cnt >= 1) {
				MQTT_Control_Packet_Type = (MQTTBufferRX[0] & 0xF0);
				MQTT_Flags = (MQTTBufferRX[0] & 0x0F);
			}
			uint32 nextByte = 0;
			if (MQTTBufferRX_write_cnt >= 2) {
				uint16 multiplier = 1;
				MQTT_Remaining_Length += (MQTTBufferRX[1] & 0x7F) * multiplier;
				if ((MQTTBufferRX[1] & 0x80)) {
					if (MQTTBufferRX_write_cnt >= 3) {
						multiplier = 128;
						MQTT_Remaining_Length += (MQTTBufferRX[2] & 0x7F) * multiplier;
						if ((MQTTBufferRX[2] & 0x80)) {
							if (MQTTBufferRX_write_cnt >= 4) {
								multiplier *= 128;
								MQTT_Remaining_Length += (MQTTBufferRX[3] & 0x7F) * multiplier;
								if ((MQTTBufferRX[3] & 0x80)) {
									if (MQTTBufferRX_write_cnt >= 5) {
										multiplier *= 128;
										MQTT_Remaining_Length += (MQTTBufferRX[4] & 0x7F) * multiplier;
										nextByte = 5;
									}
								} else {
									nextByte = 4;
								}
							}
						} else {
							nextByte = 3;
						}
					}
				} else {
					nextByte = 2;
				}
				if (MQTTBufferRX_write_cnt == MQTT_Remaining_Length + nextByte) {
					last_received_packet_length = MQTT_Remaining_Length;
					MQTT_packet_received = 1;

					MQTTBufferRX_variable_header_cnt = nextByte;
					//Variable Header
					uint16 PacketIdentifier = 0;
					if (MQTTBufferRX_write_cnt >= (nextByte + 1) + 1) {//Need two more byte for this
						switch (MQTT_Control_Packet_Type) {
							case MQTTSUBACK : {
								PacketIdentifier = MQTTBufferRX[nextByte];
								PacketIdentifier <<= 8;
								PacketIdentifier = MQTTBufferRX[nextByte + 1];
								nextByte += 2;
								break;
							}
							case MQTTPUBLISH : {
								uint32 topic_len = 0;
								topic_len = MQTTBufferRX[nextByte];
								topic_len <<= 8;
								topic_len = MQTTBufferRX[nextByte + 1];
								nextByte += 2; //Length of Topic

								nextByte += topic_len; //Topic
								if ((MQTT_Flags & MQTTQOSMASK) == MQTTQOS0) {
									nextByte += 0;
								} else if (((MQTT_Flags & MQTTQOSMASK) == MQTTQOS1) || ((MQTT_Flags & MQTTQOSMASK) == MQTTQOS2)) {
									PacketIdentifier = MQTTBufferRX[nextByte];
									PacketIdentifier <<= 8;
									PacketIdentifier = MQTTBufferRX[nextByte + 1];
									nextByte += 2;
								}
								break;
							}
							case MQTTCONNECT : {
								nextByte += 6;//Protocol Name
								nextByte += 1;//Protocol Level
								nextByte += 1;//Connect Flags
								nextByte += 2;//Keep Alive
								break;
							}
							case MQTTCONNACK : {
								nextByte += 1;//Connect Acknowledge Flags
								nextByte += 1;//Connect Return Code
								break;
							}
							default : {
								mqtt_not_implemented ++;
								break;
							}
						}
						MQTTBufferRX_Packet_Id = PacketIdentifier;
					}
					//Payload
					MQTTBufferRX_payload_cnt = nextByte;
					switch (MQTT_Control_Packet_Type) {
						case MQTTCONNECT : {
							break;
						}
						case MQTTCONNACK : {
							break;
						}
						case MQTTPUBLISH : {
							break;
						}
						case MQTTPUBACK : {
							break;
						}
						case MQTTPUBREC : {
							break;
						}
						case MQTTPUBREL : {
							break;
						}
						case MQTTPUBCOMP : {
							break;
						}
						case MQTTSUBSCRIBE : {
							break;
						}
						case MQTTSUBACK : {
							break;
						}
						case MQTTUNSUBSCRIBE : {
							break;
						}
						case MQTTUNSUBACK : {
							break;
						}
						case MQTTPINGREQ : {
							break;
						}
						case MQTTPINGRESP : {
							break;
						}
						case MQTTDISCONNECT : {
							break;
						}
						case MQTTReserved : {
							break;
						}
						default : {
							mqtt_not_implemented ++;
							break;
						}
					}
					MQTTBufferRX_write_cnt = 0;
				}
			}
		}
	}

	switch(MQTTState) {
		case MQTT_HOME: {
			// MQTTBeginUsage() is the only function which will kick 
			// the state machine into the next state
			break;
		}
		case MQTT_BEGIN: {

			// Obtain ownership of the DNS resolution module
			if(!DNSBeginUsage())
				break;

			// Obtain the IP address associated with the MQTT mail server
			if(MQTTClient.Server.szRAM) {
				DNSResolve((uint8_t *)MQTTClient.Server.szRAM, DNS_TYPE_A);
			} else {
				MQTTState = MQTT_HOME;		// can't do anything
				break;
			}
			
			MQTT_lastActivity = getGlobalTime();
			MQTTState++;
			break;
		}
		case MQTT_NAME_RESOLVE: {
			// Wait for the DNS server to return the requested IP address
			if(!DNSIsResolved(&MQTTServer))	{
				// Timeout after 6 seconds of unsuccessful DNS resolution
				if(getGlobalTime() - MQTT_lastActivity > 6 * TICK_SECOND)	{
					MQTTPublish_MQTTBegin = MQTT_RESOLVE_ERROR;
					MQTTState = MQTT_HOME;
					DNSEndUsage();
				}
				break;
			}

			// Release the DNS module, we no longer need it
			if(!DNSEndUsage()) {
				// An invalid IP address was returned from the DNS 
				// server. Quit and fail permanantly if host is not valid.
				MQTTPublish_MQTTBegin = MQTT_RESOLVE_ERROR;
				MQTTState = MQTT_HOME;
				break;
			}

			MQTTState++;
			// No need to break here
		}
		case MQTT_OBTAIN_SOCKET: {
			// Connect a TCP socket to the remote MQTT server
			MySocket = TCPOpen(MQTTServer.Val, TCP_OPEN_IP_ADDRESS, MQTTClient.ServerPort, TCP_PURPOSE_MQTT_CLIENT_CLIENT);
			
			// Abort operation if no TCP sockets are available
			// If this ever happens, add some more 
			// TCP_PURPOSE_MQTT_CLIENT_CLIENT sockets in TCPIPConfig.h
			if(MySocket == INVALID_SOCKET)
				break;

			MQTTState = MQTT_SOCKET_OBTAINED;
			MQTT_lastActivity = getGlobalTime();
			// No break; fall into MQTT_SOCKET_OBTAINED
		}
		case MQTT_SOCKET_OBTAINED: {
			if(!TCPIsConnected(MySocket)) {
				// Don't stick around in the wrong state if the
				// server was connected, but then disconnected us.
				// Also time out if we can't establish the connection to the MQTT server
				if(MQTTFlagsConnectedOnce || ((LONG)(getGlobalTime() - MQTT_lastActivity) > (LONG)(MQTT_SERVER_REPLY_TIMEOUT)))	{
					MQTTPublish_MQTTBegin = MQTT_CONNECT_ERROR;
					MQTTState = MQTT_CLOSE;
				}
				break;
			} else {
				MQTTFlagsConnectedOnce = TRUE;
			}
		}
		case MQTT_CONNECT: {
			if(!MQTTConnected()) {

				if(MQTTFlagsConnectedOnce) {
					nextMsgId = 1;
					BYTE d[9] = {0x00,0x06,'M','Q','I','s','d','p',MQTTPROTOCOLVERSION};
					// Leave room in the buffer for header and variable length field
					WORD length = 5;
					unsigned int j;

					for(j = 0; j < 9; j++) {
						MQTTBufferTX[length++] = d[j];
					}
					BYTE v;
					if(MQTTClient.WillTopic.szRAM) {
						MQTTClient.QOS=MQTTClient.WillQOS;
						v = 0x06 | (MQTTClient.WillQOS<<3) | (MQTTClient.WillRetain<<5);
					} else {
						v = 0x02;
					}
					if(MQTTClient.Username.szRAM) {
						v |= 0x80;
						if(MQTTClient.Password.szRAM) {
							v = v | (0x80>>1);
						}
 					}

					MQTTBufferTX[length++] = v;

					MQTTBufferTX[length++] = HIBYTE(MQTTClient.KeepAlive);
					MQTTBufferTX[length++] = LOBYTE(MQTTClient.KeepAlive);

					length = MQTTWriteString(MQTTClient.ConnectId.szRAM,MQTTBufferTX,length);
					if(MQTTClient.WillTopic.szRAM) {
						length = MQTTWriteString(MQTTClient.WillTopic.szRAM,MQTTBufferTX,length);
						length = MQTTWriteString(MQTTClient.WillMessage.szRAM,MQTTBufferTX,length);
					}

					if(MQTTClient.Username.szRAM) {		// il check su union è ok!
						length = MQTTWriteString(MQTTClient.Username.szRAM,MQTTBufferTX,length);
						if(MQTTClient.Password.szRAM) {
							length = MQTTWriteString(MQTTClient.Password.szRAM,MQTTBufferTX,length);
						}
					}
					if (MQTTWrite (MQTTCONNECT, MQTTBufferTX,length - 5) ) {
						MQTT_lastActivity = getGlobalTime();
						MQTTState = MQTT_CONNECT_ACK;
						MQTTPublish_MQTTBegin = MQTT_SUCCESS;
					}
				}
			}
			break;
		}
		case MQTT_CONNECT_ACK: {
			uint8 mqtt_control_type = 0;
			uint8 mqtt_flags = 0;
			uint16 mqtt_packet_id = 0;
			uint8 *variable_header = NULL;
			uint8 *payload = NULL;
			uint32 length = 0;
			uint8 packetReceived = MQTTReadPacket(&mqtt_control_type, &mqtt_flags, &mqtt_packet_id, &variable_header, &payload, &length);
			if ((packetReceived) && (mqtt_control_type == MQTTCONNACK)) {
				if(length >= 2) {
					switch(variable_header[1]) {
						case 0:{
							MQTT_lastActivity = getGlobalTime();
							MQTTFlagsPingOutstanding = FALSE;
							MQTTClient.bConnected = TRUE;
							MQTTPublish_MQTTConnect = MQTT_SUCCESS;
							break;
						}
						case 1: {		// unacceptable protocol version
							MQTTClient.bConnected = FALSE;		// 
							MQTTPublish_MQTTConnect = MQTT_BAD_PROTOCOL;
							break;
						}
						case 2:	{	// identifier rejected
							MQTTClient.bConnected = FALSE;		// 
							MQTTPublish_MQTTConnect = MQTT_IDENT_REJECTED;
							break;
						}
						case 3:	{	// server unavailable
							MQTTClient.bConnected = FALSE;		// 
							MQTTPublish_MQTTConnect = MQTT_SERVER_UNAVAILABLE;
							break;
						}
						case 4:	{	// bad user o password
							MQTTClient.bConnected = FALSE;		// 
							MQTTPublish_MQTTConnect = MQTT_BAD_USER_PASW;
							break;
						}
						case 5: {		// unauthorized
							MQTTClient.bConnected = FALSE;		// 
							MQTTPublish_MQTTConnect = MQTT_UNAUTHORIZED;
							break;
						}
						default : {
							break;
						}
					}
					MQTTState = MQTT_IDLE;	//go to idle now
				}
			} else {
				WORD t = getGlobalTime();
				if (t - MQTT_lastActivity > TICK_SECOND * 10) {
					MQTTState = MQTT_RECONNECT;
					break;
				}
			}
			break;
		}
		case MQTT_PING: {
			MQTTBufferTX[0] = MQTTPINGREQ;
			MQTTBufferTX[1] = 0;
			if(TCPIsPutReady(MySocket) >= 2) {
				MQTTPutArray(MQTTBufferTX, 2);
				MQTT_lastActivity = getGlobalTime();
				MQTTState = MQTT_PINGRESP;
			} else {
				WORD t = getGlobalTime();
				if (t - MQTT_lastActivity > TICK_SECOND * 10) {
					MQTTPublish_MQTTPing = MQTT_OPERATION_FAILED;
					MQTTState = MQTT_RECONNECT;
				}
			}
			break;
		}
		case MQTT_PINGRESP : {
			uint8 mqtt_control_type = 0;
			uint8 mqtt_flags = 0;
			uint16 mqtt_packet_id = 0;
			uint8 *variable_header = NULL;
			uint8 *payload = NULL;
			uint32 length = 0;
			uint8 packetReceived = MQTTReadPacket(&mqtt_control_type, &mqtt_flags, &mqtt_packet_id, &variable_header, &payload, &length);
			if ((packetReceived) && (mqtt_control_type == MQTTPINGRESP)) {
				MQTTPublish_MQTTPing = MQTT_SUCCESS;
				MQTTState = MQTT_IDLE;
			} else {
				WORD t = getGlobalTime();
				if (t - MQTT_lastActivity > TICK_SECOND * 10) {
				MQTTPublish_MQTTPing = MQTT_FAIL;
					MQTTState = MQTT_IDLE;
				}
			}
			break;
		}
		case MQTT_PING_ACK: {
			MQTTBufferTX[0] = MQTTPINGRESP;
			MQTTBufferTX[1] = 0;
			if(TCPIsPutReady(MySocket) >= 2) {
				MQTTPutArray(MQTTBufferTX, 2);
				MQTT_lastActivity = getGlobalTime();
				MQTTState = MQTT_IDLE;
			} else {
				WORD t = getGlobalTime();
				if (t - MQTT_lastActivity > TICK_SECOND * 10) {
					MQTTPublish_MQTTPing = MQTT_FAIL;
					MQTTState = MQTT_RECONNECT;
				}
			}
			break;
		}
		case MQTT_PUBLISH: {
			if(MQTTConnected()) {
				// Leave room in the buffer for header and variable length field
				WORD length = 5;
				length = MQTTWriteString(MQTTClient.Topic.szRAM, MQTTBufferTX,length);
				WORD i;
				for(i = 0;i < MQTTClient.Plength; i++) {
					MQTTBufferTX[length++] = MQTTClient.Payload.szRAM[i];		// idem ROM/RAM ..
				}
				BYTE header = MQTTPUBLISH | (MQTTClient.QOS ? (MQTTClient.QOS == 2 ? MQTTQOS2 : MQTTQOS1) : MQTTQOS0);
				if(MQTTClient.Retained) {
					header |= 1;
				}
				if (MQTTWrite(header, MQTTBufferTX, length - 5) ) {
					MQTT_lastActivity = getGlobalTime();
					MQTTState = MQTT_PUBLISH_ACK;
				} else {
					WORD t = getGlobalTime();
					if (t - MQTT_lastActivity > TICK_SECOND * 10) {
						MQTTPublish_MQTTPublish = MQTT_FAIL;
						MQTTState = MQTT_IDLE;
					}
				}
			} else {
				MQTTPublish_MQTTPublish = MQTT_OPERATION_FAILED;
				MQTTState = MQTT_IDLE;
			}
			break;
		}
		case MQTT_PUBLISH_ACK: {
			if(MQTTClient.QOS > 0) {
				uint8 mqtt_control_type = 0;
				uint8 mqtt_flags = 0;
				uint16 mqtt_packet_id = 0;
				uint8 *variable_header = NULL;
				uint8 *payload = NULL;
				uint32 length = 0;
				uint8 packetReceived = MQTTReadPacket(&mqtt_control_type, &mqtt_flags, &mqtt_packet_id, &variable_header, &payload, &length);
				if ((packetReceived) && (mqtt_control_type == MQTTPUBACK)) {
					if(length >= 2) {
						MQTTPublish_MQTTPublish = MQTT_SUCCESS;
						MQTTState = MQTT_IDLE;
					}
				} else {
					WORD t = getGlobalTime();
					if (t - MQTT_lastActivity > TICK_SECOND * 10) {
						MQTTPublish_MQTTPublish = MQTT_FAIL;
						MQTTState = MQTT_IDLE;
					}
				}
			} else {
				MQTTPublish_MQTTPublish = MQTT_SUCCESS;
				MQTTState = MQTT_IDLE;
			}
			break;
		}
		case MQTT_SUBSCRIBE: {
			if(MQTTConnected()) {
				// Leave room in the buffer for header and variable length field
				WORD length = 5;
				nextMsgId++;
				if(nextMsgId == 0) {
					 nextMsgId = 1;
				}

				MQTTBufferTX[length++] = HIBYTE(nextMsgId);
				MQTTBufferTX[length++] = LOBYTE(nextMsgId);
				length = MQTTWriteString(MQTTClient.TopicSubscribe.szRAM, MQTTBufferTX, length);
				MQTTBufferTX[length++] = MQTTClient.QOSSubScribe;

				if (MQTTWrite(MQTTSUBSCRIBE | MQTTQOS1, MQTTBufferTX, length - 5)) {//Subscribe itself is fixed QOS
					MQTTState = MQTT_SUBSCRIBE_ACK;
				} else {
					WORD t = getGlobalTime();
					if (t - MQTT_lastActivity > TICK_SECOND * 10) {
						MQTTPublish_MQTTSubscribe = MQTT_OPERATION_FAILED;
						MQTTState = MQTT_IDLE;
					}
				}
			} else {
				MQTTPublish_MQTTSubscribe = MQTT_OPERATION_FAILED;
				MQTTState = MQTT_IDLE;
			}
			break;
		}
		case MQTT_SUBSCRIBE_ACK: {
			uint8 mqtt_control_type = 0;
			uint8 mqtt_flags = 0;
			uint16 mqtt_packet_id = 0;
			uint8 *variable_header = NULL;
			uint8 *payload = NULL;
			uint32 length = 0;
			uint8 packetReceived = MQTTReadPacket(&mqtt_control_type, &mqtt_flags, &mqtt_packet_id, &variable_header, &payload, &length);
			if ((packetReceived) && (mqtt_control_type == MQTTSUBACK)) {
				if (length >= 3) {
					if ((payload[0] == 0x00) || (payload[0] == 0x01) || (payload[0] == 0x02)) {
						MQTTPublish_MQTTSubscribe = MQTT_SUCCESS;
						MQTTState = MQTT_IDLE;
					} else if (payload[0] == 0x80) {
						MQTTPublish_MQTTSubscribe = MQTT_OPERATION_FAILED;
						MQTTState = MQTT_IDLE;
					}
				}
			} else {
				WORD t = getGlobalTime();
				if (t - MQTT_lastActivity > TICK_SECOND * 10) {
					MQTTPublish_MQTTSubscribe = MQTT_OPERATION_FAILED;
					MQTTState = MQTT_IDLE;
				}
			}
			break;
		}
		case MQTT_PUBACK: {	
			if(MQTTConnected()) {
				// Leave room in the buffer for header and variable length field
				WORD length = 5;
				MQTTBufferTX[length++] = HIBYTE(MQTTClient.MsgId);
				MQTTBufferTX[length++] = LOBYTE(MQTTClient.MsgId);
				if (MQTTWrite(MQTTPUBACK, MQTTBufferTX, length - 5)) {
					MQTTPublish_MQTTPublish = MQTT_SUCCESS;
					MQTTState = MQTT_IDLE;
				} else {
					WORD t = getGlobalTime();
					if (t - MQTT_lastActivity > TICK_SECOND * 10) {
						MQTTPublish_MQTTPublish = MQTT_FAIL;
						MQTTState = MQTT_IDLE;
					}
				}
			} else {
				MQTTPublish_MQTTPublish = MQTT_FAIL;
				MQTTState = MQTT_IDLE;
			}
			break;
		}
		case MQTT_PUBREC : {
			if(MQTTConnected()) {
				// Leave room in the buffer for header and variable length field
				WORD length = 5;
				MQTTBufferTX[length++] = HIBYTE(MQTTClient.MsgId);
				MQTTBufferTX[length++] = LOBYTE(MQTTClient.MsgId);
				if (MQTTWrite(MQTTPUBREC, MQTTBufferTX, length - 5)) {
					MQTTPublish_MQTTPublish = MQTT_SUCCESS;
					MQTTState = MQTT_IDLE;
				} else {
					WORD t = getGlobalTime();
					if (t - MQTT_lastActivity > TICK_SECOND * 10) {
						MQTTPublish_MQTTPublish = MQTT_FAIL;
						MQTTState = MQTT_IDLE;
					}
				}
			} else {
					MQTTPublish_MQTTPublish = MQTT_FAIL;
				MQTTState = MQTT_IDLE;
			}
			break;
		}
		case MQTT_PUBCOMP : {
			if(MQTTConnected()) {
				WORD length = 5;
				MQTTBufferTX[length++] = HIBYTE(MQTTClient.MsgId);
				MQTTBufferTX[length++] = LOBYTE(MQTTClient.MsgId);
				if(MQTTWrite(MQTTPUBCOMP, MQTTBufferTX, length - 5)) {
					MQTTPublish_MQTTPublish = MQTT_SUCCESS;
					MQTTState = MQTT_IDLE;
				} else {
					WORD t = getGlobalTime();
					if (t - MQTT_lastActivity > TICK_SECOND * 10) {
						MQTTPublish_MQTTPublish = MQTT_FAIL;
						MQTTState = MQTT_IDLE;
					}
				}
			} else {
				MQTTPublish_MQTTPublish = MQTT_FAIL;
				MQTTState = MQTT_IDLE;
			}
			break;
		}
		case MQTT_UNSUBSCRIBE: {
			if(MQTTConnected()) {
				WORD length = 5;
				nextMsgId++;
				if(nextMsgId == 0) {
					 nextMsgId = 1;
				}
			
				MQTTBufferTX[length++] = HIBYTE(nextMsgId);
				MQTTBufferTX[length++] = LOBYTE(nextMsgId);
				length = MQTTWriteString(MQTTClient.TopicSubscribe.szRAM, MQTTBufferTX,length);
				if (MQTTWrite(MQTTUNSUBSCRIBE | MQTTQOS1, MQTTBufferTX, length - 5)) {
					MQTTState = MQTT_UNSUBSCRIBE_ACK;
				} else {
					WORD t = getGlobalTime();
					if (t - MQTT_lastActivity > TICK_SECOND * 10) {
						MQTTPublish_MQTTUnSubscribe = MQTT_FAIL;
						MQTTState = MQTT_IDLE;
					}
				}
			} else {
				MQTTPublish_MQTTUnSubscribe = MQTT_OPERATION_FAILED;
				MQTTState = MQTT_IDLE;
			}
			break;
		}
		case MQTT_UNSUBSCRIBE_ACK: {
			uint8 mqtt_control_type = 0;
			uint8 mqtt_flags = 0;
			uint16 mqtt_packet_id = 0;
			uint8 *variable_header = NULL;
			uint8 *payload = NULL;
			uint32 length = 0;
			uint8 packetReceived = MQTTReadPacket(&mqtt_control_type, &mqtt_flags, &mqtt_packet_id, &variable_header, &payload, &length);
			if ((packetReceived) && (mqtt_control_type == MQTTUNSUBACK)) {
				if (length >= 2) {
					MQTTPublish_MQTTUnSubscribe = MQTT_SUCCESS;
					MQTTState = MQTT_IDLE;
				}
			} else {
				WORD t = getGlobalTime();
				if (t - MQTT_lastActivity > TICK_SECOND * 10) {
					MQTTPublish_MQTTUnSubscribe = MQTT_OPERATION_FAILED;
					MQTTState = MQTT_IDLE;
				}
			}
			break;
		}
		case MQTT_DISCONNECT: {
			MQTTBufferTX[0] = MQTTDISCONNECT;
			MQTTBufferTX[1] = 0;
			if(TCPIsPutReady(MySocket) >= 2) {
				MQTTPutArray(MQTTBufferTX, 2);
				MQTT_lastActivity = getGlobalTime();
				MQTTState = MQTT_CLOSE;
			} else {
				WORD t = getGlobalTime();
				if (t - MQTT_lastActivity > TICK_SECOND * 10) {
					MQTTPublish_MQTTDisconnect = MQTT_OPERATION_FAILED;
					MQTTState = MQTT_CLOSE;
				}
			}
			break;
		}
		case MQTT_CLOSE: {
			// Close the socket so it can be used by other modules
			TCPDisconnect(MySocket);
			MySocket = INVALID_SOCKET;
			MQTTFlagsConnectedOnce = FALSE;

			MQTTState = MQTT_HOME;
			MQTTPublish_MQTTDisconnect = MQTT_SUCCESS;
			break;
		}
		case MQTT_IDLE: {
			if(MQTTConnected()) {
				WORD t = getGlobalTime();
				if(t - LastPingTick > MQTT_KEEPALIVE_REALTIME * TICK_SECOND) {
					if( !MQTTFlagsPingOutstanding) {
						MQTTState = MQTT_PING;
						MQTT_lastActivity = getGlobalTime();
						MQTTFlagsPingOutstanding = TRUE;
						LastPingTick = getGlobalTime();
						break;
					}
				}
				uint8 mqtt_control_type = 0;
				uint8 mqtt_flags = 0;
				uint16 mqtt_packet_id = 0;
				uint8 *variable_header = NULL;
				uint8 *payload = NULL;
				uint32 length = 0;
				uint8 packetReceived = MQTTReadPacket(&mqtt_control_type, &mqtt_flags, &mqtt_packet_id, &variable_header, &payload, &length);
				if(packetReceived) {
					static uint16 mqtt_packet_id_last = 0;
					MQTT_lastActivity = t;
					switch(mqtt_control_type) {
						case MQTTPUBLISH : {
							mqtt_packet_id_last = mqtt_packet_id;
							if ((mqtt_flags & MQTTQOSMASK) == MQTTQOS0) {
								//Nothing to do.
							} else if ((mqtt_flags & MQTTQOSMASK) == MQTTQOS1) {
								//MQTTPUBACK
								MQTTPubACK(mqtt_packet_id_last);
							} else if ((mqtt_flags & MQTTQOSMASK) == MQTTQOS2) {
								//MQTTPUBREC
								MQTTPubREC(mqtt_packet_id_last);
							}
							if(MQTTClient.m_Callback) {
								uint16 topic_len_callback = (variable_header[0] << 8) + variable_header[1];
								unsigned int len_callback = length - topic_len_callback;
								//void (*m_Callback)(const BYTE *topic, const BYTE *payload, unsigned int len);
								memset(topic_callback, 0x00, MQTT_MAX_PACKET_SIZE);
								memcpy(topic_callback, &variable_header[2], topic_len_callback);
								topic_callback[topic_len_callback] = 0;
								memset(payload_callback, 0x00, MQTT_MAX_PACKET_SIZE);
								memcpy(payload_callback, payload, length);
								payload_callback[length] = 0;
								MQTTClient.m_Callback(topic_callback, payload_callback, len_callback);
							}
							break;
						}
						case MQTTPUBREL : {
							MQTTPubCOMP(mqtt_packet_id_last);
							break;
						}
						case MQTTPINGREQ : {
							MQTT_lastActivity = getGlobalTime();
							MQTTState = MQTT_PING_ACK;
							break;
						}
						case MQTTPINGRESP : {
							MQTTFlagsPingOutstanding = FALSE;
							break;
						}
						default : {
							break;
						}
					}
				}			
			} else {
				MQTTState = MQTT_RECONNECT;
			}
			break;
		}
		case MQTT_RECONNECT: {
			TCPDisconnect(MySocket);
			MySocket = INVALID_SOCKET;
			MQTTFlagsConnectedOnce = FALSE;
			MQTTState = MQTT_BEGIN;
			break;
		}
	}
}

/*****************************************************************************
 Function:
	WORD MQTTPutArray(BYTE* Data, WORD Len)

 Description:
	Writes a series of bytes to the MQTT client.

 Precondition:
	MQTTBeginUsage returned TRUE on a previous call.

 Parameters:
	Data - The data to be written
	Len - How many bytes should be written

 Returns:
	The number of bytes written. If less than Len, then the TX FIFO became
	full before all bytes could be written.
	
 Remarks:
	This function should only be called externally when the MQTT client is
	generating an on-the-fly message. (That is, MQTTSendMail was called
	with MQTTClient.Body set to NULL.)
	
 Internal:
 ***************************************************************************/
WORD MQTTPutArray(BYTE* Data, WORD Len) {
	WORD result = 0;

	result = TCPPutArray(MySocket, Data, Len);
	TCPFlush(MySocket);

	return result;
}

BOOL MQTTWrite(BYTE header, BYTE *buf, WORD length) {
	BYTE lenBuf[4] = {0,0,0,0};
	BYTE llen = 0;
	BYTE digit;
	BYTE pos = 0;
	BYTE rc;
	BYTE len = length;
	int i;

	do {
		digit = len % 128;
		len = len / 128;
		if(len > 0) {
			digit |= 0x80;
		}
		lenBuf[pos++] = digit;
		llen++;
	} while(len>0);

	buf[4 - llen] = header;
	for(i = 0; i < llen; i++) {
		buf[5 - llen + i] = lenBuf[i];
	}
	WORD txlen = length + 1 + llen;
	if(TCPIsPutReady(MySocket) >= txlen) { //length+1+llen
		rc = MQTTPutArray(buf + (4 - llen), txlen);
		MQTT_lastActivity = getGlobalTime();
		return (rc == txlen);
	} else {
		return 0;
	}
}

WORD MQTTWriteString(const BYTE *string, BYTE *buf, WORD pos) {
	const BYTE *idp = string;
	WORD i=0;

	pos += 2;
	while (*idp) {
		buf[pos++] = *idp++;
		i++;
	}
	buf[pos - i - 2] = HIBYTE(i);
	buf[pos - i - 1] = LOBYTE(i);
	return pos;
}

BOOL MQTTConnected(void) {
	BOOL rc = 0;

	if (TCPIsConnected(MySocket)) {
		rc = MQTTClient.bConnected;
	}
	
	return rc;
}

uint8 MQTTReadPacket(uint8 *mqtt_control_type, uint8 *mqtt_flags, uint16 *mqtt_packet_id, uint8 **variable_header, uint8 **payload, uint32 *length) {
	uint8 result;
	result = MQTT_packet_received;

	if (MQTT_packet_received != 0) {
		if (mqtt_control_type != NULL) {
			*mqtt_control_type = MQTTBufferRX[0] & 0xF0;
		}
		if (mqtt_flags != NULL) {
			*mqtt_flags = MQTTBufferRX[0] & 0x0F;
		}
		if (mqtt_packet_id != NULL) {
			*mqtt_packet_id = MQTTBufferRX_Packet_Id;
		}
		if (variable_header != NULL) {
			*variable_header = &MQTTBufferRX[MQTTBufferRX_variable_header_cnt];
		} 
		if (payload != NULL) {
			*payload = &MQTTBufferRX[MQTTBufferRX_payload_cnt];
		}
		if (length != NULL) {
			*length = last_received_packet_length;
		}
	}

	MQTT_packet_received = 0;
	return result;
}

BOOL MQTTSetServer(const BYTE *server, BYTE bSecure, WORD KeepAlive) {
	BOOL result = 0;
	if ((MQTTState == MQTT_IDLE) || (MQTTState == MQTT_BEGIN)) {
		MQTTClient.Server.szRAM = (BYTE *)server;
		MQTTClient.bSecure = bSecure;
		MQTTClient.ServerPort = MQTTClient.bSecure ? MQTT_PORT_SECURE : MQTT_PORT;
		MQTTClient.KeepAlive = KeepAlive;
		result = 1;
	}
	return result;
}

BOOL MQTTSetCallback(void (*m_Callback)(const BYTE *,const BYTE *,unsigned int)) {
	BOOL result = 0;
	if ((MQTTState == MQTT_IDLE) || (MQTTState == MQTT_BEGIN)) {
		MQTTClient.m_Callback = m_Callback;
		result = 1;
	}
	return result;
}

/*****************************************************************************
 Function:
	BOOL MQTTConnect(const BYTE *id, const BYTE *user, const BYTE *pass, const BYTE *willTopic, BYTE willQos, BYTE willRetain, const BYTE *willMessage)

 Summary:
	Connects to a server with given ID etc.

 Description:
	This function starts the state machine that performs the actual
	transmission of the message. Call this function after all the fields
	in MQTTClient have been set.

 Precondition:
	MQTTBeginUsage returned TRUE on a previous call.

 Parameters:
	None

 Returns:
	None
 ***************************************************************************/
BOOL MQTTConnect(const BYTE *id, const BYTE *user, const BYTE *pass, const BYTE *willTopic, BYTE willQos, BYTE willRetain, const BYTE *willMessage) {
	BOOL result = 0;
	if ((MQTTState == MQTT_IDLE) | (MQTTState == MQTT_BEGIN)) {
		MQTTClient.ConnectId.szRAM = (BYTE *)id;
		MQTTClient.Username.szRAM = (BYTE *)user;
		MQTTClient.Password.szRAM = (BYTE *)pass;
		MQTTClient.WillTopic.szRAM = (BYTE *)willTopic;
		MQTTClient.WillQOS = willQos;
		MQTTClient.WillRetain = willRetain;
		MQTTClient.WillMessage.szRAM = (BYTE *)willMessage;
		MQTTPublish_MQTTConnect = MQTT_BUSY;
		result = 1;
	}
	return result;
}

MQTT_RESULTS MQTTPublish_getMQTTConnect(void) {
	MQTT_RESULTS result = MQTT_BUSY;
	result = MQTTPublish_MQTTConnect;
	return result;
}

/*****************************************************************************
 Function:
	void MQTTPing(void)

 Summary:
	Sends a PING message

 Description:
	This function starts the state machine that performs the actual
	transmission of the message. Call this function after all the fields
	in MQTTClient have been set.

 Precondition:
	MQTTBeginUsage returned TRUE on a previous call.

 Parameters:
	None

 Returns:
	None
 ***************************************************************************/
BOOL MQTTPing(void) {
	BOOL result = 0;
	if(MQTTState == MQTT_IDLE) {
		if(MQTTClient.bConnected) {
			MQTTState = MQTT_PING;
			MQTT_lastActivity = getGlobalTime();
			MQTTPublish_MQTTPing = MQTT_BUSY;
			result = 1;
		}
	}
	return result;
}

MQTT_RESULTS MQTTPublish_getMQTTPing(void) {
	MQTT_RESULTS result = MQTT_BUSY;
	result = MQTTPublish_MQTTPing;
	return result;
}

/*****************************************************************************
 Function:
	void MQTTPublish(const BYTE *topic, BYTE *payload, WORD plength, BOOL retained)

 Summary:
	Publishes data for a topic

 Description:
	This function starts the state machine that performs the actual
	transmission of the message. Call this function after all the fields
	in MQTTClient have been set.

 Precondition:
	MQTTBeginUsage returned TRUE on a previous call.

 Parameters:
	None

 Returns:
	None
 ***************************************************************************/
BOOL MQTTPublish(const BYTE *topic, const BYTE *payload, WORD plength, BOOL retained) {
	BOOL result = 0;
	//solo per ROM ovvero per C30!
	if(MQTTState == MQTT_IDLE) {
		if(MQTTClient.bConnected) {
			MQTTClient.Topic.szRAM = (BYTE *)topic;
			MQTTClient.Payload.szRAM = (BYTE *)payload;
			MQTTClient.Plength = plength;
			MQTTClient.Retained = retained;
			MQTTState = MQTT_PUBLISH;
			MQTT_lastActivity = getGlobalTime();
			MQTTPublish_MQTTPublish = MQTT_BUSY;
			result = 1;
		}
	}
	return result;
}

MQTT_RESULTS MQTTPublish_getMQTTPublish(void) {
	MQTT_RESULTS result = MQTT_BUSY;
	result = MQTTPublish_MQTTPublish;
	return result;
}
/*****************************************************************************
 Function:
	void MQTTPubACK(WORD id)

 Summary:
	Replies to Publish if needed

 Description:
	This function starts the state machine that performs the actual
	transmission of the message. Call this function after all the fields
	in MQTTClient have been set.

 Precondition:
	MQTTBeginUsage returned TRUE on a previous call.

 Parameters:
	None

 Returns:
	None
 ***************************************************************************/
BOOL MQTTPubACK(WORD id) {
	if(MQTTState == MQTT_IDLE) {
		if(MQTTClient.bConnected) {
			MQTTClient.MsgId = id;
			MQTTState = MQTT_PUBACK;
			MQTT_lastActivity = getGlobalTime();
			MQTTPublish_MQTTPublish = MQTT_BUSY;
			return 1;
		}
	}
	return 0;
}

MQTT_RESULTS MQTTPublish_getMQTTPubACK(void) {
	MQTT_RESULTS result = MQTT_BUSY;
	result = MQTTPublish_MQTTPublish;
	return result;
}

BOOL MQTTPubREC(WORD id) {
	if(MQTTState == MQTT_IDLE) {
		if(MQTTClient.bConnected) {
			MQTTClient.MsgId = id;
			MQTTState = MQTT_PUBREC;
			MQTT_lastActivity = getGlobalTime();
			MQTTPublish_MQTTPublish = MQTT_BUSY;
			return 1;
		}
	}
	return 0;
}

MQTT_RESULTS MQTTPublish_getMQTTPubREC(void) {
	MQTT_RESULTS result = MQTT_BUSY;
	result = MQTTPublish_MQTTPublish;
	return result;
}

BOOL MQTTPubCOMP(WORD id) {
	if(MQTTState == MQTT_IDLE) {
		if(MQTTClient.bConnected) {
			MQTTClient.MsgId = id;
			MQTTState = MQTT_PUBCOMP;
			MQTT_lastActivity = getGlobalTime();
			MQTTPublish_MQTTPublish = MQTT_BUSY;
			return 1;
		}
	}
	return 0;
}

MQTT_RESULTS MQTTPublish_getMQTTPubOMP(void) {
	MQTT_RESULTS result = MQTT_BUSY;
	result = MQTTPublish_MQTTPublish;
	return result;
}

/*****************************************************************************
 Function:
	void MQTTSubscribe(const char topic, BYTE qos)

 Summary:
	Subscribes to a topic

 Description:
	This function starts the state machine that performs the actual
	transmission of the message. Call this function after all the fields
	in MQTTClient have been set.

 Precondition:
	MQTTBeginUsage returned TRUE on a previous call.

 Parameters:
	None

 Returns:
	None
 ***************************************************************************/
BOOL MQTTSubscribe(const BYTE *topic, BYTE qos) {
	//solo per ROM ovvero per C30!
	if(MQTTState == MQTT_IDLE) {
		if(MQTTClient.bConnected) {
			MQTTClient.TopicSubscribe.szRAM = (BYTE *)topic;
			MQTTClient.QOSSubScribe = qos;
			MQTTState = MQTT_SUBSCRIBE;
			MQTT_lastActivity = getGlobalTime();
			MQTTPublish_MQTTSubscribe = MQTT_BUSY;
			return 1;
		}
	}
	return 0;
}

MQTT_RESULTS MQTTPublish_getMQTTSubscribe(void) {
	MQTT_RESULTS result = MQTT_BUSY;
	result = MQTTPublish_MQTTSubscribe;
	return result;
}

/*****************************************************************************
 Function:
	void MQTTDisconnect()

 Summary:
	Disconnects gracefully (already done in Close anyway)

 Description:

 Precondition:
	MQTTBeginUsage returned TRUE on a previous call.

 Parameters:
	None

 Returns:
	None
 ***************************************************************************/
BOOL MQTTDisconnect(void) {
	BOOL result = 0;
	if(MQTTState == MQTT_IDLE) {
		if(MQTTClient.bConnected) {
			MQTTState = MQTT_DISCONNECT;
			MQTT_lastActivity = getGlobalTime();
			MQTTPublish_MQTTDisconnect = MQTT_BUSY;
			result = 1;
		}
	}
	return result;
}

MQTT_RESULTS MQTTPublish_getMQTTDisconnect(void) {
	MQTT_RESULTS result = MQTT_BUSY;
	result = MQTTPublish_MQTTDisconnect;
	return result;
}

BOOL MQTTIdle(void) {
	if(MQTTState == MQTT_IDLE) {
		return 1;
	}
	return 0;
}

#endif //#if defined(STACK_USE_MQTT_CLIENT)

