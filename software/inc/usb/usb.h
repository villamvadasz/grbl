#ifndef _USB_H_
#define _USB_H_
//DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: All necessary USB Library headers
// *****************************************************************************
// *****************************************************************************

#include "GenericTypeDefs.h"
#include "usb_compiler.h"

#include "usb_config.h"             // Must be defined by the application

#include "usb_common.h"         // Common USB library definitions
#include "usb_ch9.h"            // USB device framework definitions

#if defined( USB_SUPPORT_DEVICE )
    #include "usb_device.h"     // USB Device abstraction layer interface
#endif

#if defined( USB_SUPPORT_HOST )
    #include "usb_host.h"       // USB Host abstraction layer interface
#endif

#if defined ( USB_SUPPORT_OTG )
    #include "usb_otg.h"
#endif

#include "usb_hal.h"            // Hardware Abstraction Layer interface

// *****************************************************************************
// *****************************************************************************
// Section: MCHPFSUSB Firmware Version
// *****************************************************************************
// *****************************************************************************

#define USB_MAJOR_VER   2       // Firmware version, major release number.
#define USB_MINOR_VER   9       // Firmware version, minor release number.
#define USB_DOT_VER     0       // Firmware version, dot release number.


extern void init_usb(void);
extern void do_usb(void);
extern void isr_usb(void);
extern void isr_usb_1ms(void);
extern void deinit_usb(void);


#endif // _USB_H_
/*************************************************************************
 * EOF
 */

