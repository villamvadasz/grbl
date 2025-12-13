#include <string.h>
#include "c_bootloader.h"
#include "Framework.h"
#include "fixedmemoryaddress.h"
#include "StackTsk.h"
#include "UDP.h"
#include "Helpers.h"
#include "Announce.h"
#include "Tick.h"

UDP_PORT         serverPort = UDP_BTL_PORT;    // server port
UDP_SOCKET       udpSkt;        // communication handle
UINT8 txData[200];
UINT8 rxData[FRAMEWORK_BUFF_SIZE];
BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
APP_CONFIG AppConfig;

static void InitAppConfig(void);

void ethernetInit(UINT pbClk) {
	INTEnableSystemMultiVectoredInt();
	InitAppConfig();
	TickInit();
	StackInit();
	udpSkt = UDPOpen(serverPort, 0, 0);	
}

void HandleCommand_Ethernet(void) {
	static uint32_t dwLastIP = 0;
	INT nBytes = 0;
	UINT txLen = 0;
	INT avlBytes = 0;
	
	// Valid frame received, process the command.
	HandleCommand_general();

	StackTask();
	// This tasks invokes each of the core stack application tasks
	StackApplications();
	// Check if bootloader has something to send out to PC. 
	txLen = GetTransmitFrame(txData);
	
	if(txLen)
	{
	
		while(nBytes < txLen)
		{
			StackTask();
			nBytes = UDPIsPutReady(udpSkt);
		}	
		
		nBytes = UDPPutArray( (UINT8*)&txData, txLen);
		// Make sure all bytes are transmitted.
		// This assert must be caught during debug phase.
		ASSERT(nBytes==txLen);
		
		UDPFlush();	
	} 
	else   
	{	
		avlBytes = UDPIsGetReady(udpSkt);
		
		if(avlBytes >= sizeof(rxData))
		{
			// PC application is pumping more data than device can handle.
			while(1);
		}	

		if(avlBytes) 
		{
						
		 	nBytes = UDPGetArray((BYTE*)&rxData, avlBytes ); 		  		
	  		if(nBytes)
	  		{
		  		// Pass the bytes to frame work.
				BuildRxFrame(rxData, nBytes);	
		  	}					 
		}			
	}	

	
	// If the local IP address has changed (ex: due to DHCP lease change)
	// write the new IP address to the LCD display, UART, and Announce
	// service
	if (dwLastIP != AppConfig.MyIPAddr.Val) {
		dwLastIP = AppConfig.MyIPAddr.Val;

#if defined(STACK_USE_UART)
		putrsUART((ROM char *) "\r\nNew IP Address: ");
#endif

		//DisplayIPValue(AppConfig.MyIPAddr);

#if defined(STACK_USE_UART)
		putrsUART((ROM char *) "\r\n");
#endif

#if defined(STACK_USE_ANNOUNCE)
		AnnounceIP();
#endif

	}
}

void ethernetClose(void) {
	IEC0bits.T1IE = 0;	
}

static void InitAppConfig(void)
{
	// Start out zeroing all AppConfig bytes to ensure all fields are 
	// deterministic for checksum generation
	memset((void*)&AppConfig, 0x00, sizeof(AppConfig));
	
	AppConfig.Flags.bIsDHCPEnabled = TRUE;
	AppConfig.Flags.bInConfigMode = TRUE;

	{
		extern void bootloader_get_shared_mac(unsigned char *data);
		bootloader_get_shared_mac(SerializedMACAddress);
	}
	
	memcpypgm2ram((void*)&AppConfig.MyMACAddr, (ROM void*)SerializedMACAddress, sizeof(AppConfig.MyMACAddr));
	//AppConfig.MyMACAddr.v[0] = MY_DEFAULT_MAC_BYTE1;
	//AppConfig.MyMACAddr.v[1] = MY_DEFAULT_MAC_BYTE2;
	//AppConfig.MyMACAddr.v[2] = MY_DEFAULT_MAC_BYTE3;
	//AppConfig.MyMACAddr.v[3] = MY_DEFAULT_MAC_BYTE4;

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
	// Initialize the MAC address.
}
