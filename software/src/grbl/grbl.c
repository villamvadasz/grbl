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
#include "grbl.h"

#include "k_stdtype.h"
#include "mal.h"
#include "c_isr.h"
#include "eep_manager.h"
#include "c_eep_manager.h"

volatile unsigned int dummyPORT1;
volatile unsigned int dummyPORT2;
volatile unsigned int dummyPORT3;
volatile unsigned int dummyPORT4;
volatile unsigned int dummyPORT5;
volatile unsigned int dummyPORT6;
volatile unsigned int dummyPORT7;
volatile unsigned int dummyLAT1;
volatile unsigned int dummyLAT2;
volatile unsigned int dummyLAT3;
volatile unsigned int dummyLAT4;
volatile unsigned int dummyLAT5;
volatile unsigned int dummyLAT6;
volatile unsigned int dummyLAT7;
volatile unsigned int dummyTRIS1;
volatile unsigned int dummyTRIS2;
volatile unsigned int dummyTRIS3;
volatile unsigned int dummyTRIS4;
volatile unsigned int dummyTRIS5;
volatile unsigned int dummyTRIS6;
volatile unsigned int dummyTRIS7;

unsigned int grbl_reset_asyn_execute = 0;

__attribute__(( weak )) void init_serial_grbl(void) {}
__attribute__(( weak )) void do_serial_grbl(void) {}
__attribute__(( weak )) void isr_serial_grbl_1ms(void) {}

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
	
	grbl_reset_asyn_execute = 1;
	init_serial_grbl();
}

void do_grbl(void) {
	do_serial_grbl();
	if (grbl_reset_asyn_execute) {
		{//Do the magic trick of reset here
			unsigned int supress_alarm = 0;
			//itt igazabol nem kell reset hanem eleg ha a Hold bol kihozom a gepet
			//es elotte uritem a buffert
			//TMR4 dolgokat talan majd figyelmet igenyelnek
			
			if (grbl_reset_asyn_execute == 2) {//Soft Reset triggered by command
				supress_alarm = 1;
			}

			eep_manager_ReadAll();

			init_system_exec();
			init_system_alarm();
			init_planner();
			init_nuts_bolts();
			init_motion_control();
			grbl_reset_eep_messages();

			// Initialize system upon power-up.
			settings_init(); // Load Grbl settings from EEPROM
			init_stepper();  // Configure stepper pins and interrupt timers
			init_stepper_enable();
			init_system_control();
			system_init(supress_alarm);   // Configure pinout pins and pin-change interrupt

			init_grbl_eeprom();

			// Reset Grbl primary systems.
			gc_init(); // Set g-code parser to default state
			spindle_init();
			coolant_init();
			limits_init();
			probe_init();
			system_log_sm_plan_reset(5);
			plan_reset(); // Clear block buffer and planner variables
			system_log_st_reset(3);
			st_reset(); // Clear stepper subsystem variables.

			// Sync cleared gcode and planner positions to current system position.
			plan_sync_position();
			gc_sync_position();

			// Print welcome message. Indicates an initialization has occured at power-up or with a reset.
			grlb_notify_new_connection();

			init_protocol_main_loop();
			init_grbl_toolchange();
			init_grbl_running();
			init_grbl_bootloader();
			init_grbl_report();
		}
		grbl_reset_asyn_execute = 0;
	} else {
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
		do_grbl_report();
	}
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
	isr_grbl_report_1ms();
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

void grlb_notify_new_connection(void) {
	// Print welcome message. Indicates an initialization has occured at power-up or with a reset.
	report_init_message();
}
