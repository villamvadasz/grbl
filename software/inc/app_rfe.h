#ifndef _APP_RFE_H_
#define _APP_RFE_H_

	#define APP_RFE_PACKET_RAW_SIZE 16

	extern unsigned char app_rfe_last_raw_packet[APP_RFE_PACKET_RAW_SIZE];

	extern void app_rfe_add_packet(unsigned char *packet, unsigned int len);
	extern unsigned char app_rfe_isFeatureActive(unsigned int featureNr);

	extern void init_app_rfe(void);
	extern void do_app_rfe(void);
	extern void isr_app_rfe_1ms(void);

#endif
