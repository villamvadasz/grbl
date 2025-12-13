/*********************************************************************
 *
 *				  MQTT (Internet of Things Protocol) Client
 *									Module for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:		mqtt.h
 * Dependencies:	TCP.h
 * Processor:	   PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:		Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:		 Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights reserved.
 * Copyright (C) 2013,2014 Cyberdyne.  All rights reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *	  digital signal controller product ("Device") which is
 *	  integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 *
 * Author			   Date	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder	 3/03/06	Original
 * Dario Greggio				30/9/14	mqtt client start
 * D. Guz						   20/01/15  Fix, cleanup, testing
 ********************************************************************/
#ifndef __MQTT_H
#define __MQTT_H

    #include "k_stdtype.h"
    #include "GenericTypeDefs.h"

/****************************************************************************
  Section:
	Data Type Definitions
  ***************************************************************************/
typedef enum _MQTT_RESULTS {
	MQTT_SUCCESS = (0x0000u),	// successfully 
	MQTT_BUSY = (0x0001u),	// successfully 
	MQTT_FAIL =	(0xffffu),	// error 
	MQTT_RESOLVE_ERROR = (0x8000u),	// DNS lookup for MQTT server failed
	MQTT_CONNECT_ERROR = (0x8001u),	// Connection to MQTT server failed
	MQTT_BAD_PROTOCOL =	 (0x8101u),	// connect: bad protocol
	MQTT_IDENT_REJECTED = (0x8102u),	// connect: id rejected
	MQTT_SERVER_UNAVAILABLE = (0x8103u),	// connect: server unavailable
	MQTT_BAD_USER_PASW = (0x8104u),	// connect: bad user or password
	MQTT_UNAUTHORIZED = (0x8105u),	// connect: unauthorized
	MQTT_OPERATION_FAILED = (0x8201u),	// publish, subscribe error
} MQTT_RESULTS;
	
	// MQTT_MAX_PACKET_SIZE : Maximum packet size
	#define MQTT_MAX_PACKET_SIZE 256
	
	// MQTT_KEEPALIVE : keepAlive interval in Seconds
	#define MQTT_KEEPALIVE_REALTIME 4
	#define MQTT_KEEPALIVE_SHORT 15
	#define MQTT_KEEPALIVE_LONG 120
	
	#define MQTTPROTOCOLVERSION 3
	#define MQTTCONNECT		1 << 4 // Client request to connect to Server
	#define MQTTCONNACK		2 << 4 // Connect Acknowledgment
	#define MQTTPUBLISH		3 << 4 // Publish message
	#define MQTTPUBACK		4 << 4 // Publish Acknowledgment
	#define MQTTPUBREC		5 << 4 // Publish Received (assured delivery part 1)
	#define MQTTPUBREL		6 << 4 // Publish Release (assured delivery part 2)
	#define MQTTPUBCOMP		7 << 4 // Publish Complete (assured delivery part 3)
	#define MQTTSUBSCRIBE	8 << 4 // Client Subscribe request
	#define MQTTSUBACK		9 << 4 // Subscribe Acknowledgment
	#define MQTTUNSUBSCRIBE 10 << 4 // Client Unsubscribe request
	#define MQTTUNSUBACK	11 << 4 // Unsubscribe Acknowledgment
	#define MQTTPINGREQ		12 << 4 // PING Request
	#define MQTTPINGRESP	13 << 4 // PING Response
	#define MQTTDISCONNECT	14 << 4 // Client is Disconnecting
	#define MQTTReserved	15 << 4 // Reserved
	
	#define MQTTQOSMASK		(3 << 1)
	#define MQTTQOS0		(0 << 1)
	#define MQTTQOS1		(1 << 1)
	#define MQTTQOS2		(2 << 1)
		
/****************************************************************************
  Function:
	  typedef struct MQTT_POINTERS
	
  Summary:
	Configures the MQTT client to send a message
	
  Description:
	This structure of pointers configures the MQTT Client to send an e-mail
	message. Initially, all pointers will be null.
	
		
  Parameters:
	Server -		the MQTT server to receive the message(s)
	User		 -	  the user name to use when logging into the POP3 server,
					if any is required
	Pass		 -	  the password to supply when logging in, if any is required
	bSecure -	   Port (method) to use
	ServerPort -	(WORD value) Indicates the port on which to connect to the
					remote MQTT server.

  Remarks:

  ***************************************************************************/

	typedef struct {
		union {
			BYTE *szRAM;
		} Server;
		union	{
			BYTE *szRAM;
		} Username;
		union	{
			BYTE *szRAM;
		} Password;
		union	{
			BYTE *szRAM;
		} ConnectId;
		union	{
			BYTE *szRAM;
		} Topic;
		union	{
			BYTE *szRAM;
		} TopicSubscribe;
		union	{
			BYTE *szRAM;
		} Payload;
		union	{
			BYTE *szRAM;
		} WillTopic;
		union	{
			BYTE *szRAM;
		} WillMessage;
	
		WORD Plength;
		BYTE Retained;
		BYTE WillQOS;
		BYTE WillRetain;
	
		BYTE Ver;
		WORD ServerPort;
		WORD MsgId;
		WORD KeepAlive;
		BYTE QOS;
		BYTE QOSSubScribe;
		BYTE bSecure;
		BYTE bConnected;
		BYTE bAvailable;
		FILE *Stream;
		void (*m_Callback)(const BYTE *topic, const BYTE *payload, unsigned int len);
	
	} MQTT_POINTERS;

	
	
	/****************************************************************************
	  Section:
		MQTT Function Prototypes
	  ***************************************************************************/
	
	extern BOOL MQTTBeginUsage(void);
	extern WORD MQTTEndUsage(void);
	extern void MQTTTask(void);
	
	extern BOOL MQTTSetServer(const BYTE *server, BYTE bSecure, WORD KeepAlive);
	extern BOOL MQTTSetCallback(void (*m_Callback)(const BYTE *,const BYTE *,unsigned int));
	extern BOOL MQTTSetPayload(BYTE *szRAM);
	extern BOOL MQTTConnect(const BYTE *id, const BYTE *user, const BYTE *pass, const BYTE *willTopic, BYTE willQos, BYTE willRetain, const BYTE *willMessage);
	extern MQTT_RESULTS MQTTPublish_getMQTTConnect(void);
	extern BOOL MQTTIdle(void);
	extern BOOL MQTTPublish(const BYTE *topic, const BYTE *payload, WORD plength, BOOL retained);
	extern MQTT_RESULTS MQTTPublish_getMQTTPublish(void);
	extern BOOL MQTTPubACK(WORD);
	extern MQTT_RESULTS MQTTPublish_getMQTTPubACK(void);
	extern BOOL MQTTPubREC(WORD);
	extern MQTT_RESULTS MQTTPublish_getMQTTPubREC(void);
	extern BOOL MQTTPubCOMP(WORD);
	extern MQTT_RESULTS MQTTPublish_getMQTTPubCOMP(void);
	extern BOOL MQTTSubscribe(const BYTE *topic, BYTE qos);
	extern MQTT_RESULTS MQTTPublish_getMQTTSubscribe(void);
	extern BOOL MQTTPing(void);
	extern MQTT_RESULTS MQTTPublish_getMQTTPing(void);
	extern BOOL MQTTDisconnect(void);
	extern MQTT_RESULTS MQTTPublish_getMQTTDisconnect(void);
	
	extern BOOL MQTTWrite(BYTE , BYTE *, WORD );
	extern WORD MQTTWriteString(const BYTE *, BYTE *, WORD );
	extern BOOL MQTTPut(BYTE c);
	extern WORD MQTTPutArray(BYTE *Data, WORD Len);
	extern BOOL MQTTConnected(void);
	
	#define HIBYTE(a)	((a>>8)&0xFF)
	#define LOBYTE(a)	((a)&0xFF)
	#define MAKEWORD(a,b) ((a<<8)|(b))
	
	extern BYTE *GetAsJSONValue(BYTE *buf,const BYTE *n,double v);

#endif
