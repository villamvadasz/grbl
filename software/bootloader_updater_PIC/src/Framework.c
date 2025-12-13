#include <string.h>
#include "c_bootloader.h"
#include "GenericTypeDefs.h"
#include "Framework.h"
#include "BootLoader.h"

#ifdef TRANSPORT_LAYER_USB
	#include "Usb_Tasks.h"
#endif


#ifndef BOOTLOADER_DELAY_TIME
	#define BOOTLOADER_DELAY_TIME (0xFFFFF)
#endif

const UINT8 BootInfo[2] = { MAJOR_VERSION, MINOR_VERSION };
BOOL RunApplication = FALSE;
unsigned int delayRunApplicationEnabled = 1;
unsigned long delayRunApplication = BOOTLOADER_DELAY_TIME;

void FrameWorkInit(UINT val) {
	#ifdef TRANSPORT_LAYER_USB
		UsbInit(val);
	#endif
}

void FrameWorkTask(void) {
	
	HandleCommand_crypto();

	#ifdef TRANSPORT_LAYER_SD_CARD
		HandleCommand_SDCARD();
	#endif
	
	#ifdef TRANSPORT_LAYER_USB
		UsbTasks();
		HandleCommand_USB();
	#endif

	#ifdef BOOTLOADER_DELAYED_AUTOAPPLICATION
		if (delayRunApplicationEnabled) {
			if (!ValidAppPresent()) {
				delayRunApplicationEnabled = 0;
			} else {
				if (delayRunApplication != 0) {
					delayRunApplication--;
				} else {
					delayRunApplicationEnabled = 0;
					RunApplication = TRUE;
				}
			}
		}
	#endif
}

void FrameWorkClose(void) {
	#ifdef TRANSPORT_LAYER_USB
		UsbClose();
	#endif
}

void DisableAutoRunDelay(void) {
	delayRunApplicationEnabled = 0;
}

void RunApplicationSet(void) {
	RunApplication = TRUE;
}

BOOL ExitFirmwareUpgradeMode(void) {
	return RunApplication;
}
