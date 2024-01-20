/*********************************************************************
 *
 *                  PIC32 Boot Loader
 *
 *********************************************************************
 * FileName:        EthernetTasks.c
 * Dependencies:
 * Processor:       PIC32
 *
 * Compiler:        MPLAB C32
 *                  MPLAB IDE
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC32 Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * $Id:  $
 * $Name: $
 *
 **********************************************************************/
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "Bootloader.h"
#include "Framework.h"
#include <stdlib.h>
#include "my_plib.h"
// Include all headers for any enabled TCPIP Stack functions
#include "TCPIP.h"


UDP_PORT         serverPort = UDP_BTL_PORT;    // server port
UDP_SOCKET       udpSkt;        // communication handle

APP_CONFIG AppConfig;

UINT8 txData[200];
UINT8 rxData[FRAMEWORK_BUFF_SIZE];
BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};

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

/********************************************************************
* Function: 	ethernetInit()
*
* Precondition: 
*
* Input: 		peripheral bus clock (Not used).
*
* Output:		None.
*
* Side Effects:	None.
*
* Overview: 	Initializes the IP address, TCPIP stack, creates UDP socket.
*
*			
* Note:		 	None.
********************************************************************/

void ethernetInit(UINT pbClk)
{
#if   (((__PIC32_FEATURE_SET__ >= 100) && (__PIC32_FEATURE_SET__ <= 299)))
		#error("TODO: For PIC32MX1xx/PIC32MX2xx devices, user should take care of mapping the ports to required pins");
		/* Example Code (if SPI2 is the interface)
		PPSInput(3,SDI2,RPn); // SDI2 mapping, where RPn = RPA2, RPB6....
		
		PPSOutput(2,RPn,SDO2);// SDO2 mapping, where RPn = RPA1, RPB5....

		//Do not forget to switch-off corrresponding "analog selection".
		ANSELx = 0;
		*/
#endif
	// Enable multi-vectored interrupts
	INTEnableSystemMultiVectoredInt();
	
	InitAppConfig();

	// Initialize the tick.
	TickInit();
	
	// Initialize MAC, PHY and Stack.
	StackInit();
	
	// UDP open
	udpSkt = UDPOpen(serverPort, 0, 0);	 
} 

/********************************************************************
* Function: 	ethernetTasks()
*
* Precondition: 
*
* Input: 		None.
*
* Output:		None.
*
* Side Effects:	None.
*
* Overview:     Receive packets from UDP and send packets on UDP.
*
*			
* Note:		 	None.
********************************************************************/
void ethernetTasks(void)
{
	static uint32_t dwLastIP = 0;
	INT nBytes = 0;
	UINT txLen = 0;
	INT avlBytes = 0;
	
	StackTask();
	// This tasks invokes each of the core stack application tasks
	StackApplications();
	// Check if bootloader has something to send out to PC. 
	txLen = FRAMEWORK_GetTransmitFrame(txData);
	
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
				FRAMEWORK_BuildRxFrame(rxData, nBytes);	
		  		
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


/********************************************************************
* Function: 	disableInterrupt()
*
* Precondition: 
*
* Input: 		None.
*
* Output:		None.
*
* Side Effects:	None.
*
* Overview:     Disables all interrupts used by ethernet.
*
*			
* Note:		 	None.
********************************************************************/
void disableInterrupt(void)
{
	
	IEC0bits.T1IE = 0;	
	
}	
	
//-----------------------------------------------------------------------------------------
