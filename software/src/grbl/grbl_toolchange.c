#include "grbl.h"

#include "tmr.h"

#define GRBL_TIMEOUT_FAILSAFE_TIMEOUT_AUTO   600000
#define GRBL_TIMEOUT_FAILSAFE_TIMEOUT_MANUAL 6000000

typedef enum _GRBL_Toolchange_SM_States {
	GRBL_TOOLCHANGE_SM_IDLE = 0,
	GRBL_TOOLCHANGE_SM_PREPARE,
	GRBL_TOOLCHANGE_SM_MOVE_1,
	GRBL_TOOLCHANGE_SM_MOVE_2,
	GRBL_TOOLCHANGE_SM_MOVE_3,
	GRBL_TOOLCHANGE_SM_FINISHED,
	GRBL_TOOLCHANGE_SM_PREPARE_MANUAL,
	GRBL_TOOLCHANGE_SM_FAILSAFE,
	
} GRBL_Toolchange_SM_States;

GRBL_Toolchange_SM_States GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_IDLE;
uint8 grbl_toolchange_isRunning = 0;
uint16 grbl_toolchange_toolNumber = 0;
uint8 grbl_toolchange_load_or_load_and_change = 0;
volatile uint8 do_grbl_toolchange_1ms = 0;
Timer grbl_toolchange_failsafe_timeout;

void init_grbl_toolchange(void) {
	GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_IDLE;
	grbl_toolchange_isRunning = 0;
	grbl_toolchange_toolNumber = 0;
	grbl_toolchange_load_or_load_and_change = 0;
	do_grbl_toolchange_1ms = 0;
	init_timer(&grbl_toolchange_failsafe_timeout);
}

void do_grbl_toolchange(void) {
	if (do_grbl_toolchange_1ms) {
		do_grbl_toolchange_1ms = 0;
		{
			if (//Failsafe event, state machine running and timer is elapsed
				(GRBL_Toolchange_SM_State != GRBL_TOOLCHANGE_SM_IDLE) &&
				(read_timer(&grbl_toolchange_failsafe_timeout) == 0)
			) {
				GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_FAILSAFE;
			}
			
			switch (GRBL_Toolchange_SM_State) {
				case GRBL_TOOLCHANGE_SM_IDLE : {
					if (grbl_toolchange_isRunning) {
						grbl_toolchange_isRunning = 0;
						if (settings.grbl_parameter_automatic_tool_change) {
							uint32 newValue = GRBL_TIMEOUT_FAILSAFE_TIMEOUT_AUTO;
							write_timer(&grbl_toolchange_failsafe_timeout, newValue);
							GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_PREPARE;
							//Here some kind of toolholder holds the tool, where we have to move to and execute a change
							//Off cours asyncronously implemented in a state machine
						} else {
							uint32 newValue = GRBL_TIMEOUT_FAILSAFE_TIMEOUT_MANUAL;
							write_timer(&grbl_toolchange_failsafe_timeout, newValue);
							GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_PREPARE_MANUAL;
							//Stop spindle, stop move.
							//User will change the tool and do a Z null
							//Wait for user to signalise start
						}
					}
					break;
				}
				case GRBL_TOOLCHANGE_SM_PREPARE : {
					GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_MOVE_1;
					break;
				}
				case GRBL_TOOLCHANGE_SM_MOVE_1 : {
					GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_MOVE_2;
					break;
				}
				case GRBL_TOOLCHANGE_SM_MOVE_2 : {
					GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_MOVE_3;
					break;
				}
				case GRBL_TOOLCHANGE_SM_MOVE_3 : {
					GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_FINISHED;
					break;
				}
				case GRBL_TOOLCHANGE_SM_FINISHED : {
					grbl_toolchange_isRunning = 0;
					GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_IDLE;
					break;
				}
				
				case GRBL_TOOLCHANGE_SM_PREPARE_MANUAL : {
					grbl_toolchange_isRunning = 0;
					GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_IDLE;
					break;
				}
				
				case GRBL_TOOLCHANGE_SM_FAILSAFE : {
					//Emergency button?
					grbl_toolchange_isRunning = 0;
					GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_IDLE;
					break;
				}
				default : {
					grbl_toolchange_isRunning = 0;
					GRBL_Toolchange_SM_State = GRBL_TOOLCHANGE_SM_IDLE;
					break;
				}
			}
		}
	}
}

void isr_grbl_toolchange_1ms(void) {
	do_grbl_toolchange_1ms = 1;
}

void setToolChangeRunning(uint16 toolNumber, uint8 load_or_load_and_change) { 
	grbl_toolchange_toolNumber = toolNumber;
	grbl_toolchange_load_or_load_and_change = load_or_load_and_change;
	protocol_set_requestSynchMotion();
	grbl_toolchange_isRunning = 1;
}

void updateToolChangeRunning(void) {
	grbl_toolchange_isRunning = 0;
}

uint8 isToolChangeRunning(void) {
	uint8 result = 0;
	result = grbl_toolchange_isRunning;
	return result;
}
