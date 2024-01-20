#include "grbl.h"

#include "k_stdtype.h"
#include "mal.h"
#include "bootloader_interface.h"
#include "tmr.h"
#include "eep_manager.h"

volatile uint8 do_grbl_bootloader_1ms = 0;
uint8 grbl_bootloader_triggered = 0;
Timer grbl_bootloader_timer;
uint8 grbl_bootloader_triggered_value = 0;
uint8 grbl_bootloader_state = 0;

void init_grbl_bootloader(void) {
	grbl_bootloader_state = 0;
	init_timer(&grbl_bootloader_timer);
}

void do_grbl_bootloader(void) {
	if (do_grbl_bootloader_1ms) {
		do_grbl_bootloader_1ms = 0;
		{
			switch (grbl_bootloader_state) {
				case 0 : {
					if (grbl_bootloader_triggered) {
						write_timer(&grbl_bootloader_timer, 5000);
						eep_manager_WriteAll_Trigger();
						grbl_bootloader_state = 1;
					}
					break;
				}
				case 1 : {
					if ((read_timer(&grbl_bootloader_timer) == 0) || (eep_manager_IsBusy() == 0)) {
						if (grbl_bootloader_triggered_value) {
							grbl_bootloader_triggered_value = 0;
							bootloader_interface_setRequest();
						} else {
						}
						bootloader_reset();
						grbl_bootloader_triggered = 0;
						grbl_bootloader_state = 0;
					}
					break;
				}
				default : {
					break;
				}
			}
		}
	}
}

void isr_grbl_bootloader_1ms(void) {
	do_grbl_bootloader_1ms = 1;
}

void grbl_bootloader_trigger(unsigned int value) {
	grbl_bootloader_triggered = 1;
	grbl_bootloader_triggered_value = value;
}
