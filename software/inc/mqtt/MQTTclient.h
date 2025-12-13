#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_
	
	extern void MqttClient_SetServerAuth(BYTE* Id, BYTE* Username, BYTE* Password, BYTE* serverAddr);
	extern void MqttClient_SetServer(BYTE* Id, BYTE* serverAddr, BYTE bSecure);
	extern void MqttClient_ClearServer(void);
	extern void MqttClient_QueueSubscribe(const BYTE *topic, BYTE qos);
	extern void MqttClient_QueueMsg(BYTE* Msg, BYTE* Topic);

	extern void MQTTClientInit(void);
	extern void MQTTClientTask(void);
	extern void isr_mqttclient_1ms(void);

#endif
