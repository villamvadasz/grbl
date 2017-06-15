/*
  grbl.h - main Grbl include file
  Part of Grbl

  Copyright (c) 2015-2016 Sungeun K. Jeon for Gnea Research LLC

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef grbl_h
#define grbl_h


	//#include "grbl.h"
	#include "k_stdtype.h"
	#include "cpu_map.h"
	#include "c_grbl_config.h"
	#include "system_exec.h"
	#include "system_control.h"
	#include "system.h"
	#include "system_external_power.h"
	#include "system_alarm.h"
	#include "nuts_bolts.h"
	#include "stepper_enable.h"
	#include "settings.h"
	#include "stepper.h"
	#include "grbl_limits.h"
	#include "report.h"
	#include "planner.h"
	#include "grbl_gcode.h"
	#include "coolant_control.h"
	#include "spindle_control.h"
	#include "grbl_probe.h"
	#include "grbl_eeprom.h"
	#include "grbl_serial.h"
	#include "grbl_running.h"
	#include "grbl_print.h"
	#include "jog.h"
	#include "motion_control.h"
	#include "grbl_protocol.h"
	#include "grbl_toolchange.h"
	#include "grbl_bootloader.h"
	
	
	// Grbl versioning system
	#define GRBL_VERSION "1.1f"
	#define GRBL_VERSION_BUILD "20170131"
	
	extern unsigned int grbl_reset_asyn_execute;

	extern void set_grbl_tmr0_trigger(uint16 trigger);
	extern void set_grbl_tmr0_trigger_Step_Pulse_Delay(uint16 trigger);
	extern void set_grbl_tmr1_period(uint16 period);
	extern void set_grbl_tmr1_enabled(void);
	extern void set_grbl_tmr1_disabled(void);

	extern void grbl_NotifyUserFailedRead(int item, uint8 type);
	extern void grbl_reset_eep_messages(void);

	extern void do_grbl(void);
	extern void init_grbl(void);
	extern void isr_grbl_1ms(void);
	extern void isr_grbl_tmr_stepper(void);//tmr3
	extern void isr_grbl_tmr_stepper_delay(void);//tmr4
	extern void isr_grbl_cn(void);
	
	extern void grlb_notify_new_connection(void);


#endif
