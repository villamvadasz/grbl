#include "usb.h"
#include "c_usb.h"

#include "tmr.h"
#include "sys_module.h"
#include <xc.h>
#include <sys/attribs.h>
#include "usb_app.h"
#include "system_definitions.h"

Timer usb_delay_timer;

void init_usb(void) {
    init_timer(&usb_delay_timer);
    SYS_Initialize ( NULL ); 
}

void do_usb(void) {
	SYS_Tasks ( ); 
}

void isr_usb(void) {
    DRV_USBHS_Tasks_ISR(sysObj.drvUSBObject);
}

void isr_usb_dma(void) {
    DRV_USBHS_Tasks_ISR_USBDMA(sysObj.drvUSBObject);
}

void isr_usb_1ms(void) {
}
