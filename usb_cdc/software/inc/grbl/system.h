/*
  system.h - Header for system level commands and real-time processes
  Part of Grbl

  Copyright (c) 2014-2016 Sungeun K. Jeon for Gnea Research LLC

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

#ifndef system_h
#define system_h

#include "nuts_bolts.h"

// Override bit maps. Realtime bitflags to control feed, rapid, spindle, and coolant overrides.
// Spindle/coolant and feed/rapids are separated into two controlling flag variables.
#define EXEC_FEED_OVR_RESET         bit(0)
#define EXEC_FEED_OVR_COARSE_PLUS   bit(1)
#define EXEC_FEED_OVR_COARSE_MINUS  bit(2)
#define EXEC_FEED_OVR_FINE_PLUS     bit(3)
#define EXEC_FEED_OVR_FINE_MINUS    bit(4)
#define EXEC_RAPID_OVR_RESET        bit(5)
#define EXEC_RAPID_OVR_MEDIUM       bit(6)
#define EXEC_RAPID_OVR_LOW          bit(7)
// #define EXEC_RAPID_OVR_EXTRA_LOW   bit(*) // *NOT SUPPORTED*

#define EXEC_SPINDLE_OVR_RESET         bit(0)
#define EXEC_SPINDLE_OVR_COARSE_PLUS   bit(1)
#define EXEC_SPINDLE_OVR_COARSE_MINUS  bit(2)
#define EXEC_SPINDLE_OVR_FINE_PLUS     bit(3)
#define EXEC_SPINDLE_OVR_FINE_MINUS    bit(4)
#define EXEC_SPINDLE_OVR_STOP          bit(5)
#define EXEC_COOLANT_FLOOD_OVR_TOGGLE  bit(6)
#define EXEC_COOLANT_MIST_OVR_TOGGLE   bit(7)

// Define system state bit map. The state variable primarily tracks the individual functions
// of Grbl to manage each without overlapping. It is also used as a messaging flag for
// critical events.
#define STATE_IDLE          0      // Must be zero. No flags.
#define STATE_ALARM         bit(0) // In alarm state. Locks out all g-code processes. Allows settings access.

#define STATE_HOMING        bit(2) // Performing homing cycle
#define STATE_CYCLE         bit(3) // Cycle is running or motions are being executed.
#define STATE_HOLD          bit(4) // Active feed hold
#define STATE_JOG           bit(5) // Jogging mode.
#define STATE_SAFETY_DOOR   bit(6) // Safety door is ajar. Feed holds and de-energizes system.

// Define system suspend flags. Used in various ways to manage suspend states and procedures.
#define SUSPEND_DISABLE           0      // Must be zero.
#define SUSPEND_HOLD_COMPLETE     bit(0) // Indicates initial feed hold is complete.
#define SUSPEND_RESTART_RETRACT   bit(1) // Flag to indicate a retract from a restore parking motion.
#define SUSPEND_RETRACT_COMPLETE  bit(2) // (Safety door only) Indicates retraction and de-energizing is complete.
#define SUSPEND_INITIATE_RESTORE  bit(3) // (Safety door only) Flag to initiate resume procedures from a cycle start.
#define SUSPEND_RESTORE_COMPLETE  bit(4) // (Safety door only) Indicates ready to resume normal operation.
#define SUSPEND_SAFETY_DOOR_AJAR  bit(5) // Tracks safety door state for resuming.
#define SUSPEND_MOTION_CANCEL     bit(6) // Indicates a canceled resume motion. Currently used by probing routine.
#define SUSPEND_JOG_CANCEL        bit(7) // Indicates a jog cancel in process and to reset buffers when complete.

// Define step segment generator state flags.
#define STEP_CONTROL_NORMAL_OP            0  // Must be zero.
#define STEP_CONTROL_END_MOTION           bit(0)
#define STEP_CONTROL_EXECUTE_HOLD         bit(1)
#define STEP_CONTROL_EXECUTE_SYS_MOTION   bit(2)
#define STEP_CONTROL_UPDATE_SPINDLE_PWM   bit(3)

// Define control pin index for Grbl internal use. Pin maps may change, but these values don't.
#define CONTROL_PIN_INDEX_SAFETY_DOOR   bit(0)
#define CONTROL_PIN_INDEX_ABORT         bit(1)
#define CONTROL_PIN_INDEX_FEED_HOLD     bit(2)
#define CONTROL_PIN_INDEX_CYCLE_START   bit(3)

// Define spindle stop override control states.
#define SPINDLE_STOP_OVR_DISABLED       0  // Must be zero.
#define SPINDLE_STOP_OVR_ENABLED        bit(0)
#define SPINDLE_STOP_OVR_INITIATE       bit(1)
#define SPINDLE_STOP_OVR_RESTORE        bit(2)
#define SPINDLE_STOP_OVR_RESTORE_CYCLE  bit(3)


// Define global system variables
typedef struct _system_t{
  uint8 state;               // Tracks the current system state of Grbl.
  uint8 abort_not_supported; // System abort flag. Forces exit back to main loop for reset.             
  uint8 suspend;             // System suspend bitflag variable that manages holds, cancels, and safety door.
  uint8 soft_limit;          // Tracks soft limit errors for the state machine. (boolean)
  uint8 step_control;        // Governs the step segment generator depending on system state.
  uint8 probe_succeeded;     // Tracks if last probing cycle was successful.
  uint8 homing_axis_lock;    // Locks axes when limits engage. Used as an axis motion mask in the stepper ISR.
  uint8 f_override;          // Feed rate override value in percent
  uint8 r_override;          // Rapids override value in percent
  uint8 spindle_speed_ovr;   // Spindle speed value in percent
  uint8 spindle_stop_ovr;    // Tracks spindle stop override states
  uint8 report_ovr_counter;  // Tracks when to add override data to status reports.
  uint8 report_wco_counter;  // Tracks when to add work coordinate offset data to status reports.
  float spindle_speed;
} system_t;
extern system_t sys;

// NOTE: These position variables may need to be declared as volatiles, if problems arise.
extern int32_t sys_position[N_AXIS];      // Real-time machine (aka home) position vector in steps.
extern int32_t sys_probe_position[N_AXIS]; // Last probe position in machine coordinates and steps.

extern volatile uint8 sys_rt_exec_motion_override; // Global realtime executor bitflag variable for motion-based overrides.
extern volatile uint8 sys_rt_exec_accessory_override; // Global realtime executor bitflag variable for spindle/coolant overrides.

// Initialize the serial protocol
void system_init(void);

void do_system(void);
void isr_system_1ms(void);

// Returns if safety door is open or closed, based on pin state.
uint8 system_check_safety_door_ajar();

void system_flag_wco_change(void);

// Returns machine position of axis 'idx'. Must be sent a 'step' array.
float system_convert_axis_steps_to_mpos(int32_t *steps, uint8 idx);

// Updates a machine 'position' array based on the 'step' array sent.
void system_convert_array_steps_to_mpos(float *position, int32_t *steps);

// Checks and reports if target array exceeds machine travel limits.
uint8 system_check_travel_limits(float *target);

// Special handlers for setting and clearing Grbl's real-time execution flags.

extern void system_set_requestSuspendTime(uint16 value);
extern uint8 system_get_requestSuspendTime(void);

extern void system_set_exec_motion_override_flag(uint8 mask);
extern void system_set_exec_accessory_override_flag(uint8 mask);
extern void system_clear_exec_motion_overrides(void);
extern void system_clear_exec_accessory_overrides(void);


#endif
