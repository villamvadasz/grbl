#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "app_rfe.h"

#include "tmr.h"
#include "aes_128.h"
#include "grbl.h"
#include "eep_manager.h"
#include "rtc.h"

#define APP_RFE_DEFAULT_FULL_FEATURE_TIME 1*60*60*1000	//1h

typedef struct _TokenStruct {
	uint32 ntp_time;
	uint32 serial_number;
	uint32 packet_version;
	uint32 feature_enabled;
} TokenStruct;

uint32 do_app_rfe_1ms = 0;
Timer app_rfe_timer;
uint32 app_mqtt_feature_enabled = 0;
uint32 app_mqtt_last_validated_feature_enabled = 0;
unsigned char app_rfe_last_raw_packet[APP_RFE_PACKET_RAW_SIZE] = {0};
TokenStruct token_actual_debug;

uint8 app_rfe_add_packet_trigger = 0;
unsigned char app_rfe_last_raw_packet_trigger[APP_RFE_PACKET_RAW_SIZE] = {0};

static uint32 app_rfe_validate_packet(unsigned char *packet, unsigned int len);

void init_app_rfe(void) {
	init_timer(&app_rfe_timer);
	write_timer(&app_rfe_timer, APP_RFE_DEFAULT_FULL_FEATURE_TIME);
}

void do_app_rfe(void) {
	static unsigned char do_app_rfe_single_shoot = 1;
	if (do_app_rfe_1ms) {
		do_app_rfe_1ms = 0;

		if (aes_key_loading_finished()) {
			if (do_app_rfe_single_shoot) {
				do_app_rfe_single_shoot = 0;
				app_mqtt_last_validated_feature_enabled = app_rfe_validate_packet(app_rfe_last_raw_packet, APP_RFE_PACKET_RAW_SIZE);
			}
			if (app_rfe_add_packet_trigger) {
				app_rfe_add_packet_trigger = 0;
				{
					uint32 result = app_rfe_validate_packet(app_rfe_last_raw_packet_trigger, APP_RFE_PACKET_RAW_SIZE);
					if (result != 0) {
						memcpy(app_rfe_last_raw_packet, app_rfe_last_raw_packet_trigger, APP_RFE_PACKET_RAW_SIZE);
						grbl_eeprom_trigger();
						//eep_manager_WriteItem_Trigger(EepManager_Items_RFE);

						app_mqtt_last_validated_feature_enabled = result;
					}
				}
			}
		}
		{
			if (read_timer(&app_rfe_timer) != 0) {
				//After Reset features are all enabled 1 hour long
				app_mqtt_feature_enabled = ~0;
			} else {
				//Otherwise use the last validated one
				app_mqtt_feature_enabled = app_mqtt_last_validated_feature_enabled;
			}
		}
	}
}

void isr_app_rfe_1ms(void) {
	do_app_rfe_1ms = 1;
}

void app_rfe_add_packet(unsigned char *packet, unsigned int len) {
	if ((packet != NULL) && (len == APP_RFE_PACKET_RAW_SIZE)) {
		app_rfe_add_packet_trigger = 1;
		memcpy(app_rfe_last_raw_packet_trigger, packet, len);
	}
}

unsigned char app_rfe_isFeatureActive(unsigned int featureNr) {
	unsigned char result = 0;
	if ((app_mqtt_feature_enabled >> featureNr) & 0x1) {
		result = 1;
	}
	return result;
}

static uint32 app_rfe_validate_packet(unsigned char *packet, unsigned int len) {
	unsigned int result = 0;
	if ((packet != NULL) && (len == APP_RFE_PACKET_RAW_SIZE)) {
		TokenStruct token_actual;
		unsigned char token_raw_current_encrypted[APP_RFE_PACKET_RAW_SIZE] = {0};
		unsigned char token_raw_current_decrypted[APP_RFE_PACKET_RAW_SIZE] = {0};

		memcpy(token_raw_current_encrypted, packet, len);

		{
			AES_KeySlot keyslot = AES_KEY7;
			unsigned char *dataIn = token_raw_current_encrypted;
			unsigned int sizeIn = APP_RFE_PACKET_RAW_SIZE;
			unsigned char *dataOut = token_raw_current_decrypted;
			int result = aes_decrypt_cbc(keyslot, dataIn, sizeIn, dataOut);
			if (result == 1) {
				memcpy(&token_actual, token_raw_current_decrypted, APP_RFE_PACKET_RAW_SIZE);
				memcpy(&token_actual_debug, token_raw_current_decrypted, APP_RFE_PACKET_RAW_SIZE);

				if (token_actual.packet_version != 0x00000001) {
					//Ignore packet
				} else {
					if (token_actual.serial_number != grbl_serial_number) {
						//Ignore packet
					} else {
						TimeDate ptr;
						uint32 ntp_seconds = 0;
						rtc_getTime(&ptr);//UTC
						rtc_DateToNtp(&ptr, &ntp_seconds);
						if (token_actual.ntp_time < ntp_seconds) {
						} else {
							result = token_actual.feature_enabled;
						}
					}
				}
			}
		}
	}
	return result;
}
