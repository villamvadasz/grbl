#include "ethernet.h"

#define THIS_IS_STACK_APPLICATION
#include "TCPIP.h"
#include "c_eep_manager.h"
#include "eep_manager.h"

APP_CONFIG AppConfig;
BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};

extern void isr_ethernet_tick_1ms(void);
static void InitAppConfig(void);
unsigned char hexStringToChar(unsigned char *str);

void init_ethernet(void) {
	init_enc28j60();
	init_tcp();
	init_udp();
	init_berkeleyapi();
	InitAppConfig();
	StackInit();
}

void do_ethernet(void) {
	do_enc28j60();
	do_tcp();
	do_udp();
	do_berkeleyapi();
	StackTask();
	StackApplications();
}

void isr_ethernet_1ms(void) {
	isr_enc28j60_1ms();
	isr_tcp_1ms();
	isr_udp_1ms();
	isr_berkeleyapi_1ms();
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

unsigned char hexStringToChar(unsigned char *str) {
	unsigned char result = 0;
	if (str != NULL) {
		unsigned int x = 0;
		for (x = 0; x < 2; x++) {
			result <<= 4;
			if ((str[x] >= '0') && (str[x] <= '9')) {
				result += str[x] - '0' + 0x00;
			} else if ((str[x] >= 'A') && (str[x] <= 'F')) {
				result += str[x] - 'A' + 0x0A;
			} else if ((str[x] >= 'a') && (str[x] <= 'f')) {
				result += str[x] - 'a' + 0x0A;
			}
		}
	}
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
	memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, 16);
	FormatNetBIOSName(AppConfig.NetBIOSName);
}