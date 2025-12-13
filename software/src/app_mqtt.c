#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "TCPIPConfig.h"
#ifdef STACK_USE_MQTT_CLIENT
#include "app_mqtt.h"
#include "mal.h"
#include "version.h"

#include "k_stdtype.h"

#include "MQTTclient.h"
#include "MQTT.h"
#include "grbl_eep_export.h"
#include "tmr.h"
#include "TCPIP.h"

//#include "grbl_print.h"

volatile unsigned int do_app_mqtt_1000ms = 1;

char app_mqtt_msgBF[256];
char app_mqtt_serverAddr[] = "asdf1234.no-ip.org";
char app_mqtt_Username[] = "asdf1234";
char app_mqtt_Password[] = "asdf1234";
char app_mqtt_id[] = "grbl_cnc";
char app_mqtt_topic[] = "/grbl_cnc/0x12345678/send____";

BYTE app_mqtt_qos = 1;
char app_mqtt_topic_subscribe[] = "/grbl_cnc/0x12345678/receive____";
BYTE app_mqtt_qos_subscribe = 0;
Timer app_mqtt_delay = 0;

void init_app_mqtt(void) {
	init_timer(&app_mqtt_delay);
}

void do_app_mqtt(void) {
	if (do_app_mqtt_1000ms) {
		do_app_mqtt_1000ms = 0;
		
		{
			static unsigned int app_mqtt_state = 0;
			switch (app_mqtt_state) {
				case 0 : {
					//printPgmString(("\r\nConnect MQTT\r\n"));
					sprintf(app_mqtt_topic, "/grbl_cnc/0x%08x/send", VERSION_APP_SERIAL_NUMBER);
					sprintf(app_mqtt_topic_subscribe, "/grbl_cnc/0x%08x/receive", VERSION_APP_SERIAL_NUMBER);

					MqttClient_SetServerAuth((BYTE *)app_mqtt_id, (BYTE*) app_mqtt_Username, (BYTE*) app_mqtt_Password, (BYTE *)app_mqtt_serverAddr);
					MqttClient_SetServer((BYTE *)app_mqtt_id, (BYTE *)app_mqtt_serverAddr, FALSE);
					write_timer(&app_mqtt_delay, 1500);
					app_mqtt_state = 1;
					break;
				}
				case 1 : {
					if (read_timer(&app_mqtt_delay) == 0) {
						MqttClient_QueueSubscribe((const BYTE *)app_mqtt_topic_subscribe, (BYTE)app_mqtt_qos_subscribe);
						write_timer(&app_mqtt_delay, 1500);
						app_mqtt_state = 2;
					}
					break;
				}
				case 2 : {
					if (read_timer(&app_mqtt_delay) == 0) {
						extern BYTE SerializedMACAddress[6];

						sprintf((char *)app_mqtt_msgBF,"{\"grbl_serial_number\":\"0x%08x\",\"serial_number\":\"0x%08x\",\"running_hour\":%u,\"startup_cnt\":%u,\"spindle_running_minutes\":%u,\"MAC\":%02x-%02x-%02x-%02x-%02x-%02x},\"IP\":%u.%u.%u.%u}", 
							grbl_serial_number, 
							VERSION_APP_SERIAL_NUMBER, 
							grbl_running_running_hours, 
							grbl_running_startup_cnt, 
							grbl_running_spindle_running_minutes,
							SerializedMACAddress[0],
							SerializedMACAddress[1],
							SerializedMACAddress[2],
							SerializedMACAddress[3],
							SerializedMACAddress[4],
							SerializedMACAddress[5],
							AppConfig.MyIPAddr.v[0],
							AppConfig.MyIPAddr.v[1],
							AppConfig.MyIPAddr.v[2],
							AppConfig.MyIPAddr.v[3]
							);
						MqttClient_QueueMsg((BYTE *)app_mqtt_msgBF, (BYTE *)app_mqtt_topic);
						write_timer(&app_mqtt_delay, 15000);
						app_mqtt_state = 3;
					}
					break;
				}
				case 3 : {
					if (read_timer(&app_mqtt_delay) == 0) {
						//printPgmString(("\r\nDisconnect MQTT\r\n"));
						MqttClient_ClearServer();
						app_mqtt_state = 4;
					}
					break;
				}
				default : {
					break;
				}
			}
		}
    }
}

void isr_app_mqtt_1ms(void) {
    static uint32 do_app_mqtt_1000ms_cnt = 0;
    do_app_mqtt_1000ms_cnt++;
    if (do_app_mqtt_1000ms_cnt >= 1000) {
        do_app_mqtt_1000ms_cnt = 0;
        do_app_mqtt_1000ms = 1;
    }
}

void app_mqttclient_callback(const BYTE *topic, const BYTE *payload, unsigned int len) {
    while (1) {
        unsigned int x = 0;
        unsigned char foundSpace = 0;
        BYTE *payload_ovr = (BYTE *)payload;
        for (x = 0; x < len; x++) {
            if (payload[x] == ' ') {
                foundSpace = x;
                break;
            }
        }
        if (foundSpace) {
            for (x = foundSpace; x < (len - 1); x++){
                payload_ovr[x] = payload_ovr[x + 1];
            }
            payload_ovr[len] = 0;//To ensure null terminated string
            len = len - 1;
        } else {
            break;
        }
    }
    
    if (strcmp((const char *)topic, (const char *)app_mqtt_topic_subscribe) == 0) {
        if (strncmp((const char *)&payload[2], (const char *)"topic", strlen((const char *)"topic")) == 0) {
            if (strncmp((const char *)&payload[10], (const char *)"switch", strlen((const char *)"switch")) == 0) {
                if (strncmp((const char *)&payload[19], (const char *)"state", strlen((const char *)"state")) == 0) {
                    if (strncmp((const char *)&payload[27], (const char *)"on", strlen((const char *)"on")) == 0) {
                    }
                    if (strncmp((const char *)&payload[27], (const char *)"off", strlen((const char *)"off")) == 0) {
                    }
                }
            }
        }
    }
}
#endif