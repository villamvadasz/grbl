#include <string.h> 
#include "k_stdtype.h"
#include "grbl_eeprom.h"
#include "eep_manager.h"

#include "settings.h"
#include "system_alarm.h"

volatile uint8 do_grbl_eeprom_1ms = 0;
uint8 grbl_eeprom_triggerWriteAll = 0;
uint8 grbl_eeprom_triggerWriteSettings = 0;
uint8 grbl_eeprom_triggerWriteItems = 0;
uint8 grbl_master_eep_error = 0;
uint8 grbl_backup_eep_error = 0;
uint8 grbl_critical_eep_error = 0;
uint8 master_eep_error_single_shoot = 1;
uint8 backup_eep_error_single_shoot = 1;

void init_grbl_eeprom(void) {
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
			{
				static uint16 doTriggerWriteItemsCnt = 0;
				if (grbl_eeprom_triggerWriteItems) {
					doTriggerWriteItemsCnt++;
					if (doTriggerWriteItemsCnt >= 1000) {
						doTriggerWriteItemsCnt = 0;
						grbl_eeprom_triggerWriteItems = 0;
						eep_manager_WriteItem_Trigger(EepManager_Items_GRBL_5);
						eep_manager_WriteItem_Trigger(EepManager_Items_GRBL_6);
						eep_manager_WriteItem_Trigger(EepManager_Items_GRBL_7);
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

void grbl_eeprom_trigger_items(void) {
	grbl_eeprom_triggerWriteItems = 1;
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
