#include <string.h> 
#include "grbl.h"

#include "k_stdtype.h"
#include "eep_manager.h"

#include "tmr.h"

#define GCODE_POSITION_SAVE_TIME 500

#pragma GCC diagnostic error "-w"
#ifdef __32MZ2048ECG144__
	#define GRBL_EEPROM_SYS_POSITION_ADDRESS_1 0x80000400
	#define GRBL_EEPROM_SYS_POSITION_ADDRESS_2 0x80000410
	#define GRBL_EEPROM_SYS_POSITION_ADDRESS_3 0x80000420
#else
	#define GRBL_EEPROM_SYS_POSITION_ADDRESS_1 0xA0000400
	#define GRBL_EEPROM_SYS_POSITION_ADDRESS_2 0xA0000410
	#define GRBL_EEPROM_SYS_POSITION_ADDRESS_3 0xA0000420
#endif
volatile int32_t grbl_eeprom_sys_position_1[N_AXIS + 1] __attribute__ ((persistent, address(GRBL_EEPROM_SYS_POSITION_ADDRESS_1)));
volatile int32_t grbl_eeprom_sys_position_2[N_AXIS + 1] __attribute__ ((persistent, address(GRBL_EEPROM_SYS_POSITION_ADDRESS_2)));
volatile int32_t grbl_eeprom_sys_position_3[N_AXIS + 1] __attribute__ ((persistent, address(GRBL_EEPROM_SYS_POSITION_ADDRESS_3)));
#pragma GCC diagnostic pop

volatile uint8 do_grbl_eeprom_1ms = 0;
uint8 grbl_eeprom_triggerWriteAll = 0;
uint8 grbl_eeprom_triggerWriteSettings = 0;
uint8 grbl_master_eep_error = 0;
uint8 grbl_backup_eep_error = 0;
uint8 grbl_critical_eep_error = 0;
uint8 master_eep_error_single_shoot = 1;
uint8 backup_eep_error_single_shoot = 1;

uint8 gc_main_eeprom_trigger = 0;
Timer gcodePositionSave;
volatile unsigned char grbl_eeprom_trigger_prev = 1;
volatile unsigned char grbl_eeprom_trigger_current = 0;

unsigned char grbl_eeprom_checkIs(void);

void init_grbl_eeprom(void) {
	initTimer(&gcodePositionSave);

	if (grbl_master_eep_error == 0) {
		//master is ok no need to check backup
	} else {
		//master failed, see for back up
		if (grbl_backup_eep_error) {
			//panic...
			grbl_critical_eep_error = 1;
		} else {
			//load backup
			memcpy(&eep_settings, &eep_settings_backup, sizeof(eep_settings_backup));
		}
	}
	
	grbl_eeprom_restorePositionFromNoInit();
}

void do_grbl_eeprom(void) {
	if (grbl_master_eep_error) {
		if (master_eep_error_single_shoot) {
			master_eep_error_single_shoot = 0;
			system_set_exec_alarm(EXEC_ALARM_EEP_MASTER_READ_FAIL);
		}
	}
	if (grbl_critical_eep_error) {
		if (backup_eep_error_single_shoot) {
			backup_eep_error_single_shoot = 0;
			system_set_exec_alarm(EXEC_ALARM_EEP_BACKUP_READ_FAIL);
		}
	}
	{
		if (do_grbl_eeprom_1ms) {
			do_grbl_eeprom_1ms = 0;
			{
				switch (gc_main_eeprom_trigger) {
					case 0 : {
						if (grbl_eeprom_checkIs()) {
							grbl_eeprom_trigger_current = 1;
						} else {
							grbl_eeprom_trigger_current = 0;
						}
						if ((grbl_eeprom_trigger_prev == 1) && (grbl_eeprom_trigger_current == 0)) {
							gc_main_eeprom_trigger = 1;
							writeTimer(&gcodePositionSave, GCODE_POSITION_SAVE_TIME);
						}
						grbl_eeprom_trigger_prev = grbl_eeprom_trigger_current;
						break;
					}
					case 1 : {
						if (grbl_eeprom_checkIs()) {
							gc_main_eeprom_trigger = 0;
						} else {
							if (readTimer(&gcodePositionSave) == 0) {
								gc_main_eeprom_trigger = 2;
							}
						}
						break;
					}
					case 2 : {
						gc_main_eeprom_trigger = 0;
						{
							uint8 idx = 0;
							for (idx = 0; idx < N_AXIS; idx++) {
								coord_system_eep[idx] = gc_state.coord_system[idx];
							} 
							for (idx = 0; idx < N_AXIS; idx++) {
								coord_offset_eep[idx] = gc_state.coord_offset[idx];
							}
							eep_manager_WriteItem_Trigger(EepManager_Items_GRBL_5);
							eep_manager_WriteItem_Trigger(EepManager_Items_GRBL_6);
							eep_manager_WriteItem_Trigger(EepManager_Items_GRBL_7);
						}
						break;
					}
					default : {
						gc_main_eeprom_trigger = 0;
						break;
					}
				}
			}
			{
				static uint16 doTriggerWriteAllCnt = 0;
				if (grbl_eeprom_triggerWriteAll) {
					doTriggerWriteAllCnt++;
					if (doTriggerWriteAllCnt >= 100) {
						doTriggerWriteAllCnt = 0;
						grbl_eeprom_triggerWriteAll = 0;
						eep_manager_WriteAll_Trigger();
					}
				}
			}
			{
				static uint16 doTriggerWriteSettingsCnt = 0;
				if (grbl_eeprom_triggerWriteSettings) {
					doTriggerWriteSettingsCnt++;
					if (doTriggerWriteSettingsCnt >= 1000) {
						doTriggerWriteSettingsCnt = 0;
						grbl_eeprom_triggerWriteSettings = 0;
						memcpy(&eep_settings_backup, &eep_settings, sizeof(eep_settings_backup));
						eep_manager_WriteItem_Trigger(EepManager_Items_GRBL_Settings);
						eep_manager_WriteItem_Trigger(EepManager_Items_GRBL_Settings_Backup);
					}
				}
			}
		}
	}
}

void isr_grbl_eeprom_1ms(void) {
	do_grbl_eeprom_1ms = 1;
}

void grbl_eeprom_trigger(void) {
	grbl_eeprom_triggerWriteAll = 1;
}

void grbl_eeprom_trigger_settings(void) {
	grbl_eeprom_triggerWriteSettings = 1;
}

void grbl_NotifyUserFailedRead(int item, uint8 type) {
	if (item == EepManager_Items_GRBL_Settings) {
 		//after a new flashing process this is set because eeprom is empty, here an alarm will be triggered but that is ok
		//after second start up the alarm should dissapear since the eeprom becomes written
		grbl_master_eep_error = 1;
	} else if (item == EepManager_Items_GRBL_Settings_Backup) {
		if (type == EEP_MANAGER_LOAD_FAILED) {
			grbl_backup_eep_error = 1;
		}
	}
}

void grbl_reset_eep_messages(void) {
	master_eep_error_single_shoot = 1;
	backup_eep_error_single_shoot = 1;
}

unsigned char grbl_eeprom_checkIs(void) {
	unsigned char result = 0;
	if (
		(plan_check_full_buffer()) ||
		(st_isRunning() != 0) ||
		(isProbing_cycleRunning() != 0) ||
		(protocol_get_requestSynchMotion()) ||
		(protocol_get_requestDwell()) ||
		(isToolChangeRunning()) ||
		(isArcGeneratingRunning() != 0) ||
		(isLimits_go_homeRunning() != 0) ||
		(isMc_homing_cycleRunning() != 0) ||
		(isDelayRunning() != 0)
	) {
		result = 1;
	}
	return result;
}


void grbl_eeprom_storePositionToNoInit(void) {
	//<= RESET
	grbl_eeprom_sys_position_1[0] = sys_position[0];
	grbl_eeprom_sys_position_1[1] = sys_position[1];
	grbl_eeprom_sys_position_1[2] = sys_position[2];
	grbl_eeprom_sys_position_1[3] = 0xDEADBEAF;
	//<= RESET
	grbl_eeprom_sys_position_2[0] = sys_position[0];
	grbl_eeprom_sys_position_2[1] = sys_position[1];
	grbl_eeprom_sys_position_2[2] = sys_position[2];
	grbl_eeprom_sys_position_2[3] = 0xDEADBEAF;
	//<= RESET
	grbl_eeprom_sys_position_3[0] = sys_position[0];
	grbl_eeprom_sys_position_3[1] = sys_position[1];
	grbl_eeprom_sys_position_3[2] = sys_position[2];
	grbl_eeprom_sys_position_3[3] = 0xDEADBEAF;
	//<= RESET
}

void grbl_eeprom_restorePositionFromNoInit(void) {
	if (
		(grbl_eeprom_sys_position_1[3] == 0xDEADBEAF) &&
		(grbl_eeprom_sys_position_2[3] == 0xDEADBEAF) &&
		(grbl_eeprom_sys_position_3[3] == 0xDEADBEAF)
	) {
		if (memcmp(grbl_eeprom_sys_position_1, grbl_eeprom_sys_position_2, sizeof(grbl_eeprom_sys_position_1)) == 0) {
			memcpy(sys_position, grbl_eeprom_sys_position_1, sizeof(grbl_eeprom_sys_position_1));
		} else if (memcmp(grbl_eeprom_sys_position_2, grbl_eeprom_sys_position_3, sizeof(grbl_eeprom_sys_position_1)) == 0) {
			memcpy(sys_position, grbl_eeprom_sys_position_2, sizeof(grbl_eeprom_sys_position_1));
		}
	}

	plan_sync_position();
	gc_sync_position();
}
