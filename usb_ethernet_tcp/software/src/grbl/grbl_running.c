#include "grbl.h"

#include "k_stdtype.h"
#include "eep_manager.h"
#include "c_eep_manager.h"

uint32 grbl_running_running_minutes = 0;
uint32 grbl_running_running_hours = 0;
uint32 grbl_running_startup_cnt = 0;
uint32 grbl_running_spindle_running_minutes = 0;
uint32 grbl_running_running_minutes_eep_trigger = 0;
uint32 grbl_running_spindle_running_minutes_eep_trigger = 0;
uint32 grbl_running_running_hours_eep_trigger = 0;

volatile uint8 do_grbl_running_1s = 0;
uint8 do_grbl_running_1min = 0;
uint8 do_grbl_running_1h = 0;

void init_grbl_running(void) {
}

void do_grbl_running(void) {
	static unsigned char appSingleShoot = 1;
	if (appSingleShoot) {
		appSingleShoot = 0;
		grbl_running_startup_cnt++;
		eep_manager_WriteItem_Trigger(EepManager_Items_StartUpCnt);
	}
	if (do_grbl_running_1s) {
		do_grbl_running_1s = 0;
		{
			static uint32 do_grbl_running_1h_cnt = 0;
			do_grbl_running_1h_cnt++;
			if (do_grbl_running_1h_cnt >= 3600) {
				do_grbl_running_1h_cnt = 0;
				do_grbl_running_1h = 1;
			}
		}
		{
			static uint32 do_grbl_running_1min_cnt = 0;
			do_grbl_running_1min_cnt++;
			if (do_grbl_running_1min_cnt >= 60) {
				do_grbl_running_1min_cnt = 0;
				do_grbl_running_1min = 1;
			}
		}
	}
	if (do_grbl_running_1min) {
		do_grbl_running_1min = 0;
		{
			grbl_running_running_minutes++;
			grbl_running_running_minutes_eep_trigger = 1;
		}
		{
			if (spindle_get_state() != SPINDLE_STATE_DISABLE) {
				grbl_running_spindle_running_minutes++;
				grbl_running_spindle_running_minutes_eep_trigger = 1;
			}
		}
	}
	if (do_grbl_running_1h) {
		do_grbl_running_1h = 0;
		{
			grbl_running_running_hours++;
			grbl_running_running_hours_eep_trigger = 1;
		}
	}
	if (grbl_eeprom_checkIs() == 0) {
		if (grbl_running_running_minutes_eep_trigger) {
			grbl_running_running_minutes_eep_trigger = 0;
			eep_manager_WriteItem_Trigger(EepManager_Items_RunningMinutes);
		}
		if (grbl_running_spindle_running_minutes_eep_trigger) {
			grbl_running_spindle_running_minutes_eep_trigger = 0;
			eep_manager_WriteItem_Trigger(EepManager_Items_SpindleRunningMinutes);
		}
		if (grbl_running_running_hours_eep_trigger) {
			grbl_running_running_hours_eep_trigger = 0;
			eep_manager_WriteItem_Trigger(EepManager_Items_RunningHours);
		}
	}
}

void isr_grbl_running_1ms(void) {
	static unsigned int do_grbl_running_1s_cnt = 0;
	do_grbl_running_1s_cnt ++;
	if (do_grbl_running_1s_cnt >= 1000) {
		do_grbl_running_1s_cnt = 0;
		do_grbl_running_1s = 1;
	}
}
