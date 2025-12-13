#include "ethernet.h"

#define THIS_IS_STACK_APPLICATION
#include "TCPIP.h"
#include "c_eep_manager.h"
#include "eep_manager.h"
#include "mal.h"

APP_CONFIG AppConfig;
BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
uint8 ethernet_is_initialized_value = 0;

extern void isr_ethernet_tick_1ms(void);
static void InitAppConfig(void);
static void deInitAppConfig(void);

void init_ethernet(void) {
	#ifdef ETHERNET_USE_ASYNC_INIT
		//Do nothing
	#else
		ethernet_is_initialized_value = 0;
		while (1) {
			unsigned int result = init_ethernet_Asynch();
			if (result != 0) {
				break;
			}
		}
		ethernet_is_initialized_value = 1;
	#endif
}

unsigned int init_ethernet_Asynch(void) {
	unsigned int result = 0;
	static unsigned int do_ethernet_sm = 0;
	switch (do_ethernet_sm) {
		case 0 : {
			init_enc28j60();
			init_tcp();
			init_udp();
			init_berkeleyapi();
			InitAppConfig();
			do_ethernet_sm = 1;
			break;
		}
		case  1 : {
			unsigned int StackInit_Async_result = StackInit_Async();
			if (StackInit_Async_result != 0) {
				do_ethernet_sm = 2;
			}
			break;
		}
		default : {
			result = 1;
			do_ethernet_sm = 0;
			break;
		}
	}
	return result;		
}

__attribute__(( weak )) unsigned int ethernet_callout_wait_for_other_device(void) {return 0;}

void do_ethernet(void) {
	if (ethernet_is_initialized_value == 0) {
		extern unsigned int ethernet_callout_wait_for_other_device(void);
		if (ethernet_callout_wait_for_other_device() == 0) {
			unsigned int init_ethernet_Asynch_result = init_ethernet_Asynch();
			if (init_ethernet_Asynch_result != 0) {
				ethernet_is_initialized_value = 1;
			}
		}
	} else {
		do_enc28j60();
		do_tcp();
		do_udp();
		do_berkeleyapi();
		StackTask();
		StackApplications();
	}
}

void isr_ethernet_1ms(void) {
	isr_enc28j60_1ms();
	isr_tcp_1ms();
	isr_udp_1ms();
	isr_berkeleyapi_1ms();
	isr_Stack_1ms();
}

void deinit_ethernet(void) {
	deinit_enc28j60();
	deinit_tcp();
	deinit_udp();
	deinit_berkeleyapi();
	deInitAppConfig();
	StackdeInit();
	ethernet_is_initialized_value = 0;
}

void ethernet_set_mac_address(unsigned char *str) {
	if (str != NULL) {
		SerializedMACAddress[0] = hexStringToChar(&str[0]);
		SerializedMACAddress[1] = hexStringToChar(&str[0 + 3]);
		SerializedMACAddress[2] = hexStringToChar(&str[0 + 3 + 3]);
		SerializedMACAddress[3] = hexStringToChar(&str[0 + 3 + 3 + 3]);
		SerializedMACAddress[4] = hexStringToChar(&str[0 + 3 + 3 + 3 + 3]);
		SerializedMACAddress[5] = hexStringToChar(&str[0 + 3 + 3 + 3 + 3 + 3]);
		eep_manager_WriteItem_Trigger(EepManager_Items_MAC_Address);
	}
}

unsigned int ethernet_is_initialized(void) {
	unsigned int result = 0;
	result = ethernet_is_initialized_value;
	return result;
}

static void InitAppConfig(void)
{
	// Start out zeroing all AppConfig bytes to ensure all fields are 
	// deterministic for checksum generation
	memset((void*)&AppConfig, 0x00, sizeof(AppConfig));
	
	AppConfig.Flags.bIsDHCPEnabled = TRUE;
	AppConfig.Flags.bInConfigMode = TRUE;
	memcpypgm2ram((void*)&AppConfig.MyMACAddr, (ROM void*)SerializedMACAddress, sizeof(AppConfig.MyMACAddr));
	AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
	AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
	AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
	AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
	AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
	AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
	AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;
	
	// Load the default NetBIOS Host Name
	size_t len = sizeof(MY_DEFAULT_HOST_NAME);
	if (len >= 16) {
		len = 16;
	}
	memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, len);
	FormatNetBIOSName(AppConfig.NetBIOSName);
}

static void deInitAppConfig(void) {
}
