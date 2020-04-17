/*
  main.c - An embedded CNC Controller with rs274/ngc (g-code) support
  Part of Grbl

  Copyright (c) 2011-2016 Sungeun K. Jeon for Gnea Research LLC
  Copyright (c) 2009-2011 Simen Svale Skogsrud

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
#include <string.h>
#include "k_stdtype.h"
#include "grbl.h"

#include "c_isr.h"
#include "eep_manager.h"
#include "c_eep_manager.h"
#include "stepper_enable.h"
#include "nuts_bolts.h"
#include "system.h"
#include "settings.h"
#include "system_exec.h"
#include "system_alarm.h"
#include "system_control.h"
#include "grbl_eeprom.h"
#include "stepper.h"
#include "gcode.h"
#include "spindle_control.h"
#include "coolant_control.h"
#include "limits_grbl.h"
#include "grbl_probe.h"
#include "planner.h"
#include "report.h"
#include "motion_control.h"
#include "grbl_toolchange.h"
#include "mal.h"
#include "grbl_running.h"
#include "grbl_bootloader.h"
#include "grbl_serial.h"

volatile unsigned int dummyPORT;
volatile unsigned int dummyLAT;
volatile unsigned int dummyTRIS;

void init_grbl(void) {
	TMR4 = 0;
	PR4 = 0xFFFF;

	TMR3 = 0;
	PR3 = 0xFFFF;
	#if TIMER_PRESACLER == 4
		T3CONbits.TCKPS = 0b010; //Prescaler is 4
		T4CONbits.TCKPS = 0b010; //Prescaler is 4
	#else
		#error TODO implement
	#endif
	T3CONbits.ON = 0; //Will be enabled by application
	T4CONbits.ON = 0; //Will be enabled by application

	IPC3bits.T3IP = ISR_IPLV_TMR3;
	IPC3bits.T3IS = 0;
	clear_isr(IFS0CLR, _IFS0_T3IF_POSITION);
	IEC0bits.T3IE = 1;

	IPC4bits.T4IP = ISR_IPLV_TMR4;
	IPC4bits.T4IS = 0;
	clear_isr(IFS0CLR, _IFS0_T4IF_POSITION);
	
	IEC0bits.T4IE = 1;

	// Initialize system upon power-up.
	settings_init(); // Load Grbl settings from EEPROM
	init_stepper();  // Configure stepper pins and interrupt timers
	init_stepper_enable();
	init_system_control();
	system_init();   // Configure pinout pins and pin-change interrupt

	//memset(sys_position,0,sizeof(sys_position)); // Clear machine position.
	//sei(); // Enable interrupts

	init_grbl_eeprom();

	// Reset Grbl primary systems.
	gc_init(); // Set g-code parser to default state
	spindle_init();
	coolant_init();
	limits_init();
	probe_init();
	plan_reset(); // Clear block buffer and planner variables
	st_reset(); // Clear stepper subsystem variables.

	// Sync cleared gcode and planner positions to current system position.
	plan_sync_position();
	gc_sync_position();

	// Print welcome message. Indicates an initialization has occured at power-up or with a reset.
	report_init_message();

	init_protocol_main_loop();
	init_grbl_toolchange();
	init_grbl_running();
	init_grbl_bootloader();
	init_serial_grbl();
}

void do_grbl(void) {
	do_grbl_eeprom();
	gc_main();
	do_protocol_main_loop();
	do_motion_control();
	do_limits();
	do_nuts_bolts();
	do_system();
	do_spindle();
	do_stepper();
	do_stepper_enable();
	do_system_control();
	do_probe();
	do_grbl_toolchange();
	do_grbl_running();
	do_grbl_bootloader();
	do_serial_grbl();
}

void isr_grbl_1ms(void) {
	isr_grbl_eeprom_1ms();
	isr_stepper_enable_1ms();
	isr_nuts_bolts_1ms();
	isr_limits_1ms();
	isr_system_1ms();
	isr_system_control_1ms();
	isr_protocol_main_loop_1ms();
	isr_probe_1ms();
	isr_grbl_toolchange_1ms();
	isr_grbl_running_1ms();
	isr_grbl_bootloader_1ms();
	isr_serial_grbl_1ms();
}

void isr_grbl_tmr_stepper_delay(void) {//tmr4
	TIMER0_OVF_vect();

	#ifdef STEP_PULSE_DELAY
		TIMER0_COMPA_vect();
	#endif	
	T4CONbits.ON = 0;
}

void set_grbl_tmr0_trigger(uint16 trigger) {
	TMR4 = 0;
	PR4 = trigger;
	T4CONbits.ON = 1;
}

void set_grbl_tmr0_trigger_Step_Pulse_Delay(uint16 trigger) {
	TMR4 = 0;
	PR4 = trigger;
	T4CONbits.ON = 1;
}

void isr_grbl_tmr_stepper(void) {//tmr3
	TIMER1_COMPA_vect();
}

void set_grbl_tmr1_period(uint16 period) {
	T3CONbits.ON = 0;
	PR3 = period;
	TMR3 = 0;
	T3CONbits.ON = 1;
}

void set_grbl_tmr1_enabled(void) {
	T3CONbits.ON = 1;
}

void set_grbl_tmr1_disabled(void) {
	T3CONbits.ON = 0;
}

void isr_grbl_cn(void) {
	CONTROL_INT_vect();
	LIMIT_INT_vect();
}
