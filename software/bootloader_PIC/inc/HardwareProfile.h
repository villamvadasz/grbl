/********************************************************************
 FileName:      HardwareProfile.h
 Dependencies:  See INCLUDES section
 Processor:     PIC32
 Hardware:      The code is natively intended to be used on the 
                  following hardware platforms: 
                  Explorer 16
                  USB Starter Kit
                  Ethernet Starter Kit
                  The firmware may be modified for use on other 
                  platforms by editing this file (HardwareProfile.h)
 Compiler:  	Microchip C32 (for PIC32)
 Company:       Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the �Company�) for its PIC� Microcontroller is intended and
 supplied to you, the Company�s customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************/


#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H


//   Part number defining Macro
#if   (((__PIC32_FEATURE_SET__ >= 100) && (__PIC32_FEATURE_SET__ <= 299)))
    #define __PIC32MX1XX_2XX__
#elif (((__PIC32_FEATURE_SET__ >= 300) && (__PIC32_FEATURE_SET__ <= 799)))
    #define __PIC32MX3XX_7XX__
#else
    #error("Controller not supported")
#endif

// Demo board hardware profiles
#if defined(DEMO_BOARD_EXPLORER_16)
	#include "HardwareProfile_PIC32MX_PIM_Explorer_16.h"
#elif defined(DEMO_BOARD_USB_STARTER_KIT)
	#include "HardwareProfile_PIC32MX_USB_StarterKit.h"
#elif defined(DEMO_BOARD_ETH_STARTER_KIT)
	#include "HardwareProfile_PIC32MX_ETH_StarterKit.h"	
#elif defined(DEMO_BOARD_OLIMEX)
	#include "HardwareProfile_PIC32MX_Olimex.h"	
#else 
		/* Note ****: User has to define board type depending on the development board. 
	To do this, in the MPLAB IDE navigate to menu Project->Build Options->Project.
	Select "MPLAB PIC32 C Compiler" tab. Select categories as "General" from the dropdown list box.
	Click ADD button and define the DEMO_BOARD under "Preprocessor Macros".*/
	
	#error ("Demo board is either not defined or not defined properly. \
			 Supported values for this macro are BOARD_EXPLORER_16/ BOARD_USB_STARTER_KIT.");
#endif



// Transport layer specific hardware profiles
#if defined(TRANSPORT_LAYER_ETH) // Following configurations are required only if transport layer is Ethernet (Ethernet Bootloader).
	#if defined(ETH_MAC_ENC28J60)
		#include "HardwareProfile_PIC32MX_ENC28J60.h"		
	#elif defined(ETH_MAC_ENC624J600)
		#include "HardwareProfile_PIC32MX_ENC624J600.h"
	#elif defined(ETH_INTERNAL_MAC)
		#include "HardwareProfile_PIC32MX_InternalMAC.h"
	#else			
		#error("Either the MAC is not defined or the MAC is not supported")
	#endif						
#elif defined(TRANSPORT_LAYER_SD_CARD)	
	#include "HardwareProfile_PIC32MX_SD_PICtail.h"
#elif defined(TRANSPORT_LAYER_USB_HOST)
	
	// ******************* MDD File System Required Definitions ********************
	// Select your MDD File System interface type
	// This library currently only supports a single physical interface layer
	// In this example we are going to use the USB so we only need the USB definition
	// *****************************************************************************
	
	#define USE_USB_INTERFACE               // USB host MSD library
	
#elif defined(TRANSPORT_LAYER_USB)
	// Nothing specific.
#elif defined(TRANSPORT_LAYER_UART)
    // Nothing specific.
#else
	#error ("TRANSPORT_LAYER is either not defined or not defined properly");
#endif




#if defined(__PIC32MX3XX_7XX__)

	#if !defined(__32MX440F256H__)
	// Comment this line once you add the right linker script to the workspace.
		#if !defined(__32MX470F512H__)
			#if !defined(__32MX460F512L__)
				#if !defined(__32MX470F512L__)
					#error("If you are compiling this project for a part number other than PIC32MX795F512L, remove the existing linker script file from \
					the workspace and make sure to add right linker script file for the chosen part from the location ..Firmware\Bootloader\linker_scripts. Later comment this #error.");
				#endif
			#endif
		#endif
	#endif
#endif



#endif  //HARDWARE_PROFILE_H
