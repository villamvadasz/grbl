/*********************************************************************
 *
 * MQTT Client Demonstrations
 *		- MQTT Client (publish, subscribe)
 *
 *********************************************************************
 * FileName:		MQTTClient.c
 * Dependencies:	TCP/IP stack
 * Processor:		PIC32
 * Compiler:		Microchip C32 v1.05 or higher
 *
 * Author				Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * E. Wood	 			4/26/08		Moved from MainDemo.c
 * D. Greggio				9/30/14		wrote example
 * D. Guz 			20/01/15	Fix, cleanup, testing
 ********************************************************************/

#define __MQTTDEMO_C

#include "tcpip.h"

#if defined(STACK_USE_MQTT_CLIENT)

#include "MQTTclient.h"
#include "MQTT.h"
#include "k_stdtype.h"
#include "wifi.h"
#include "tmr.h"

__attribute__(( weak )) 	extern uint8 wifi_is_connected() { return 1;}

#define TICK_MILLISECONDS 1
#define TICK_SECOND 1000

#define REQ_TIMEOUT_CYCLES 5

typedef enum _MQTTClientState_States	{
	MQTTCLIENT_HOME = 0,
	MQTTCLIENT_BEGIN,
	MQTTCLIENT_CONNECT_WAIT,
	MQTTCLIENT_DECIDE,
	MQTTCLIENT_SUBSCRIBE,
	MQTTCLIENT_SUBSCRIBE_WAIT,
	MQTTCLIENT_PUBLISH,
	MQTTCLIENT_PUBLISH_WAIT,
	MQTTCLIENT_DISCONNET,
	MQTTCLIENT_DISCONNET_WAIT,
	MQTTCLIENT_DONE
} MQTTClientState_States;

typedef struct _MqttClientRequestsType {
	BYTE* Mqtt_Request_DevId;
	BYTE* Mqtt_Request_ServerAddr;
	BYTE* Mqtt_Request_MsgBuff;
	int Mqtt_Request_MsgBuffLen;
	BYTE* Mqtt_Request_Username;
	BYTE* Mqtt_Request_Password;
	BYTE* Mqtt_Request_Topic;
	BYTE Mqtt_Request_QOS;
	BYTE* Mqtt_Request_Subscribe_Topic;
	BYTE Mqtt_Request_Subscribe_QOS;
	BYTE Mqtt_Request_bSecure;
} MqttClientRequestsType;

WORD MQTTClient_lastActivity = 0;
uint8 PendingRequestStartUp = 0;
uint8 PendingRequestsMessage = 0;
uint8 PendingRequestsSubscribe = 0;
uint8 PendingRequestShutDown = 0;
MqttClientRequestsType MqttClientRequests;
void (*MqttClientRequestCallbacks)();
MQTTClientState_States MQTTClientState = MQTTCLIENT_HOME;
volatile uint32 do_mqttclient_1ms = 0;

void MqttQueueMsg(BYTE* Msg, BYTE* Topic, BYTE* Id, BYTE* serverAddr);
void SetCredForRequest(BYTE* Username, BYTE* Password);
void MqttClient_Callback(const BYTE *topic,const BYTE *payload,unsigned int len);

__attribute__(( weak )) void app_mqttclient_callback(const BYTE *topic,const BYTE *payload,unsigned int len) {}

void MQTTClientInit(void) {
}


/*****************************************************************************
 Function:
	void MQTTDemo(void)

 Summary:
	Synchronous state machine for currently executed request.

 Precondition:
	The MQTT client is initialized.

 Parameters:
	None

 Returns:
 	None
 ***************************************************************************/
void MQTTClientTask(void) {
	if (do_mqttclient_1ms) {
		do_mqttclient_1ms = 0;
		
		switch(MQTTClientState) {
			case MQTTCLIENT_HOME : {
				if (wifi_is_connected()) {
					if (PendingRequestStartUp) {
						PendingRequestStartUp = 0;
						MQTTClient_lastActivity = getGlobalTime();
						MQTTClientState = MQTTCLIENT_BEGIN;
					}
				}
				break;
			}
			case MQTTCLIENT_BEGIN : {
				if(MQTTBeginUsage()) {
					MQTTConnect(MqttClientRequests.Mqtt_Request_DevId, MqttClientRequests.Mqtt_Request_Username, MqttClientRequests.Mqtt_Request_Password, NULL, 0, 1, NULL);
					MQTTSetServer(MqttClientRequests.Mqtt_Request_ServerAddr, MqttClientRequests.Mqtt_Request_bSecure, MQTT_KEEPALIVE_LONG);
					MQTTSetCallback(MqttClient_Callback);
					MQTTClientState = MQTTCLIENT_CONNECT_WAIT;
				} else {
					WORD t = getGlobalTime();
					if (t - MQTTClient_lastActivity > TICK_SECOND * REQ_TIMEOUT_CYCLES) {
						MQTTClientState = MQTTCLIENT_DONE;
					}
				}
				break;
			}
			case MQTTCLIENT_CONNECT_WAIT : {
				if(MQTTConnected()) {
					MQTTClientState = MQTTCLIENT_DECIDE;
				} else {
					WORD t = getGlobalTime();
					if (t - MQTTClient_lastActivity > TICK_SECOND * REQ_TIMEOUT_CYCLES) {
						MQTTClientState = MQTTCLIENT_DONE;
					}
				}
				break;
			}
			case MQTTCLIENT_DECIDE : {
				if(MQTTIdle()) {
					if (PendingRequestsSubscribe > 0) {
						MQTTClientState = MQTTCLIENT_SUBSCRIBE;
					} else if(PendingRequestsMessage > 0)	{
						MQTTClientState = MQTTCLIENT_PUBLISH;
					} else if (PendingRequestShutDown > 0) {
						MQTTClient_lastActivity = getGlobalTime();
						MQTTClientState = MQTTCLIENT_DISCONNET;
					}
				}
				break;
			}
			case MQTTCLIENT_SUBSCRIBE : {
				if (MQTTIdle()) {
					MQTTSubscribe(MqttClientRequests.Mqtt_Request_Subscribe_Topic, MqttClientRequests.Mqtt_Request_Subscribe_QOS);
					MQTTClient_lastActivity = getGlobalTime();
					MQTTClientState = MQTTCLIENT_SUBSCRIBE_WAIT;
				}
				break;
			}
			case MQTTCLIENT_SUBSCRIBE_WAIT : {
				PendingRequestsSubscribe = 0;
				if(MQTTPublish_getMQTTSubscribe() == MQTT_BUSY) {
					WORD t = getGlobalTime();
					if (t - MQTTClient_lastActivity > TICK_SECOND * REQ_TIMEOUT_CYCLES) {
						MQTTClientState = MQTTCLIENT_DONE;
					}
				} else if(MQTTPublish_getMQTTSubscribe() == MQTT_SUCCESS) {
					MQTTClientState = MQTTCLIENT_DECIDE;
				} else {
					MQTTClientState = MQTTCLIENT_DECIDE;
				}
				break;
			}

			case MQTTCLIENT_PUBLISH : {
				if (MQTTIdle()) {
					MQTTPublish(MqttClientRequests.Mqtt_Request_Topic, MqttClientRequests.Mqtt_Request_MsgBuff, MqttClientRequests.Mqtt_Request_MsgBuffLen, 0);
					MQTTClient_lastActivity = getGlobalTime();
					MQTTClientState = MQTTCLIENT_PUBLISH_WAIT;
				}
				break;
			}
			case MQTTCLIENT_PUBLISH_WAIT : {
				PendingRequestsMessage = 0;
				if(MQTTPublish_getMQTTPublish() == MQTT_BUSY) {
					WORD t = getGlobalTime();
					if (t - MQTTClient_lastActivity > TICK_SECOND * REQ_TIMEOUT_CYCLES) {
						MQTTClientState = MQTTCLIENT_DONE;
					}
				} else if(MQTTPublish_getMQTTPublish() == MQTT_SUCCESS) {
					MQTTClientState = MQTTCLIENT_DECIDE;
				} else {
					MQTTClientState = MQTTCLIENT_DECIDE;
				}
				break;
			}
			case MQTTCLIENT_DISCONNET : {
				if (MQTTIdle()) {
					MQTTDisconnect();
					MQTTClient_lastActivity = getGlobalTime();
					MQTTClientState = MQTTCLIENT_DISCONNET_WAIT;
				}
				break;
			}
			case MQTTCLIENT_DISCONNET_WAIT : {
				if(MQTTPublish_getMQTTPublish() == MQTT_BUSY) {
					WORD t = getGlobalTime();
					if (t - MQTTClient_lastActivity > TICK_SECOND * REQ_TIMEOUT_CYCLES) {
						MQTTClientState = MQTTCLIENT_DONE;
					}
				} else if (MQTTPublish_getMQTTDisconnect() == MQTT_SUCCESS) {
						MQTTClientState = MQTTCLIENT_DONE;
				} else {
					
				}
				break;
			}
			case MQTTCLIENT_DONE : {
				MQTTEndUsage();
				MQTTClientState = MQTTCLIENT_HOME;
				PendingRequestsMessage = 0;
				PendingRequestsSubscribe = 0;
				break;
			}
		}
	}
}


void isr_mqttclient_1ms(void) {
	do_mqttclient_1ms = 1;
}

/***********	Send message without credentials	************/
void MqttClient_SetServer(BYTE* Id, BYTE* serverAddr, BYTE bSecure) {
	if (PendingRequestStartUp == 0) {
		MqttClientRequests.Mqtt_Request_DevId = Id;
		MqttClientRequests.Mqtt_Request_ServerAddr = serverAddr;
		MqttClientRequests.Mqtt_Request_bSecure = bSecure;
		PendingRequestStartUp = 1;
	}
}

void MqttClient_ClearServer(void) {
	if (PendingRequestShutDown == 0) {
		PendingRequestShutDown = 1;
	}
}

void MqttClient_SetServerAuth(BYTE* Id, BYTE* Username, BYTE* Password, BYTE* serverAddr) {
	MqttClientRequests.Mqtt_Request_DevId = Id;
	MqttClientRequests.Mqtt_Request_ServerAddr = serverAddr;
	MqttClientRequests.Mqtt_Request_Username = Username;
	MqttClientRequests.Mqtt_Request_Password = Password;
}

void MqttClient_QueueSubscribe(const BYTE *topic, BYTE qos) {
	if (PendingRequestsSubscribe == 0) {
		MqttClientRequests.Mqtt_Request_Subscribe_Topic = (BYTE *)topic;
		MqttClientRequests.Mqtt_Request_Subscribe_QOS = qos;
		PendingRequestsSubscribe = 1;
	}
}

void MqttClient_QueueMsg(BYTE* Msg, BYTE* Topic) {
	if (PendingRequestsMessage == 0) {
		MqttClientRequests.Mqtt_Request_Topic = Topic;
		MqttClientRequests.Mqtt_Request_MsgBuff = Msg;
		MqttClientRequests.Mqtt_Request_MsgBuffLen = strlen((char *)Msg);
		MqttClientRequests.Mqtt_Request_QOS = 0;
		PendingRequestsMessage = 1;
	}
}

void MqttClient_Callback(const BYTE *topic, const BYTE *payload, unsigned int len) {
	app_mqttclient_callback(topic, payload, len);
}

#endif //#if defined(STACK_USE_MQTT_CLIENT)
