#ifndef _CPU_MAP_ARDUINO_H_
#define _CPU_MAP_ARDUINO_H_

	#ifdef DEFAULTS_PIC32_MX_ARDUINO
		#define CPU_MAP_PIC32_MX

		extern volatile unsigned int dummyPORT;
		extern volatile unsigned int dummyLAT;
		extern volatile unsigned int dummyTRIS;

		#define STEP_X_DDR TRISDbits.TRISD4
		#define STEP_Y_DDR TRISDbits.TRISD5
		#define STEP_Z_DDR TRISDbits.TRISD6
		#define STEP_X_PORT LATDbits.LATD4
		#define STEP_Y_PORT LATDbits.LATD5
		#define STEP_Z_PORT LATDbits.LATD6

		#define DIRECTION_X_DDR TRISDbits.TRISD7
		#define DIRECTION_Y_DDR TRISDbits.TRISD8
		#define DIRECTION_Z_DDR TRISDbits.TRISD11
		#define DIRECTION_X_PORT LATDbits.LATD7
		#define DIRECTION_Y_PORT LATDbits.LATD8
		#define DIRECTION_Z_PORT LATDbits.LATD11

		#define STEPPERS_DISABLE_DDR TRISBbits.TRISB13
		#define STEPPERS_DISABLE_PORT LATBbits.LATB13

		#define LIMIT_X_DDR TRISBbits.TRISB14
		#define LIMIT_Y_DDR TRISGbits.TRISG9
		#define LIMIT_Z_DDR TRISGbits.TRISG8
		#define LIMIT_X_PIN PORTBbits.RB14
		#define LIMIT_Y_PIN PORTGbits.RG9
		#define LIMIT_Z_PIN PORTGbits.RG8

		//#define LIMIT_X_PIN_PUE N/A
		//#define LIMIT_Y_PIN_PUE N/A
		//#define LIMIT_Z_PIN_PUE N/A

		//#define LIMIT_X_PIN_CN N/A
		//#define LIMIT_Y_PIN_CN N/A
		//#define LIMIT_Z_PIN_CN N/A

		#define SPINDLE_ENABLE_DDR TRISGbits.TRISG7
		#define SPINDLE_ENABLE_PORT LATGbits.LATG7

		#define SPINDLE_DIRECTION_DDR TRISGbits.TRISG6
		#define SPINDLE_DIRECTION_PORT LATGbits.LATG6

		//#define SPINDLE_PWM_DDR N/A

		//#define VARIABLE_SPINDLE_OC 

		//#define COOLANT_FLOOD_DDR N/A
		//#define COOLANT_FLOOD_PORT N/A
		//#define COOLANT_MIST_DDR N/A
		//#define COOLANT_MIST_PORT N/A

		//#define CONTROL_ABORT_DDR N/A
		//#define CONTROL_FEED_HOLD_DDR N/A
		//#define CONTROL_CYCLE_START_DDR N/A
		//#define CONTROL_SAFETY_DOOR_DDR N/A

		//#define CONTROL_ABORT_PIN N/A
		//#define CONTROL_FEED_HOLD_PIN N/A
		//#define CONTROL_CYCLE_START_PIN N/A
		//#define CONTROL_SAFETY_DOOR_PIN N/A

		//#define CONTROL_ABORT_PIN_PUE N/A
		//#define CONTROL_FEED_HOLD_PIN_PUE N/A
		//#define CONTROL_CYCLE_START_PIN_PUE N/A
		//#define CONTROL_SAFETY_DOOR_PIN_PUE N/A

		//#define CONTROL_ABORT_PIN_CN N/A
		//#define CONTROL_FEED_HOLD_PIN_CN N/A
		//#define CONTROL_CYCLE_START_PIN_CN N/A
		//#define CONTROL_SAFETY_DOOR_PIN_CN N/A

		#define PROBE_TRIS TRISDbits.TRISD2
		#define PROBE_PIN PORTDbits.RD2
		//#define PROBE_PIN_PUE N/A
		//#define PROBE_PIN_CN N/A






		// Define the homing cycle patterns with bitmasks. The homing cycle first performs a search mode
		// to quickly engage the limit switches, followed by a slower locate mode, and finished by a short
		// pull-off motion to disengage the limit switches. The following HOMING_CYCLE_x defines are executed
		// in order starting with suffix 0 and completes the homing routine for the specified-axes only. If
		// an axis is omitted from the defines, it will not home, nor will the system update its position.
		// Meaning that this allows for users with non-standard cartesian machines, such as a lathe (x then z,
		// with no y), to configure the homing cycle behavior to their needs.
		// NOTE: The homing cycle is designed to allow sharing of limit pins, if the axes are not in the same
		// cycle, but this requires some pin settings changes in cpu_map.h file. For example, the default homing
		// cycle can share the Z limit pin with either X or Y limit pins, since they are on different cycles.
		// By sharing a pin, this frees up a precious IO pin for other purposes. In theory, all axes limit pins
		// may be reduced to one pin, if all axes are homed with seperate cycles, or vice versa, all three axes
		// on separate pin, but homed in one cycle. Also, it should be noted that the function of hard limits
		// will not be affected by pin sharing.
		// NOTE: Defaults are set for a traditional 3-axis CNC machine. Z-axis first to clear, followed by X & Y.
		#define HOMING_CYCLE_0 (1<<Z_AXIS)                // REQUIRED: First move Z to clear workspace.
		#define HOMING_CYCLE_1 (1<<Y_AXIS)
		#define HOMING_CYCLE_2 (1<<X_AXIS)
		//#define HOMING_IS_JUST_ZEROING_MACHINE_POS




		// Grbl generic default settings. Should work across different machines.
		#define DEFAULT_X_STEPS_PER_MM 400.0
		#define DEFAULT_Y_STEPS_PER_MM 400.0
		#define DEFAULT_Z_STEPS_PER_MM 400.0
		#define DEFAULT_X_MAX_RATE 400.0 // mm/min
		#define DEFAULT_Y_MAX_RATE 400.0 // mm/min
		#define DEFAULT_Z_MAX_RATE 400.0 // mm/min
		#define DEFAULT_X_ACCELERATION (25.0*60*60) // 25*60*60 mm/min^2 = 25 mm/sec^2
		#define DEFAULT_Y_ACCELERATION (25.0*60*60) // 25*60*60 mm/min^2 = 25 mm/sec^2
		#define DEFAULT_Z_ACCELERATION (25.0*60*60) // 25*60*60 mm/min^2 = 25 mm/sec^2
		#define DEFAULT_X_MAX_TRAVEL 118.0 // mm NOTE: Must be a positive value.
		#define DEFAULT_Y_MAX_TRAVEL 40.0 // mm NOTE: Must be a positive value.
		#define DEFAULT_Z_MAX_TRAVEL 56.0 // mm NOTE: Must be a positive value.






		#define DEFAULT_SPINDLE_RPM_MAX 30000.0 // rpm
		#define DEFAULT_SPINDLE_RPM_MIN 4000.0 // rpm
		#define SPINDLE_INVERT_GRADIENT
		#define DEFAULT_STEP_PULSE_MICROSECONDS 10
		#define DEFAULT_STEPPING_INVERT_MASK 0
		#define DEFAULT_DIRECTION_INVERT_MASK 0x4
		#define DEFAULT_STEPPER_IDLE_LOCK_TIME 25 // msec (0-254, 255 keeps steppers enabled)
		#define DEFAULT_STATUS_REPORT_MASK 1 // MPos enabled
		#define DEFAULT_JUNCTION_DEVIATION 0.01 // mm
		#define DEFAULT_ARC_TOLERANCE 0.002 // mm
		#define DEFAULT_REPORT_INCHES 0 // false
		#define DEFAULT_EMPTY 0 // false
		#define DEFAULT_INVERT_LIMIT_PINS 1 // false
		#define DEFAULT_SOFT_LIMIT_ENABLE 0 // false
		#define DEFAULT_HARD_LIMIT_ENABLE 1  // false
		#define DEFAULT_INVERT_PROBE_PIN 1 // true
		#define DEFAULT_LASER_MODE 0 // false
		#define DEFAULT_HOMING_ENABLE 1  // true
		#define DEFAULT_HOMING_DIR_MASK 3 // move positive dir
		#define DEFAULT_HOMING_FEED_RATE 50.0 // mm/min
		#define DEFAULT_HOMING_SEEK_RATE 200.0 // mm/min
		#define DEFAULT_HOMING_DEBOUNCE_DELAY 250 // msec (0-65k)
		#define DEFAULT_HOMING_PULLOFF 0.5 // mm
		#define DEFAULT_AUTOMATIC_TOOL_CHANGE 0

		// Alters the behavior of the spindle enable pin with the option . By default,
		// Grbl will not disable the enable pin if spindle speed is zero and M3/4 is active, but still sets the PWM 
		// output to zero. This allows the users to know if the spindle is active and use it as an additional control
		// input. However, in some use cases, user may want the enable pin to disable with a zero spindle speed and 
		// re-enable when spindle speed is greater than zero. This option does that.
		#define SPINDLE_ENABLE_OFF_WITH_ZERO_SPEED // Default disabled. Uncomment to enable.

		// Inverts the spindle enable pin from low-disabled/high-enabled to low-enabled/high-disabled. Useful
		// for some pre-built electronic boards.
		// #define INVERT_SPINDLE_ENABLE_PIN // Default disabled. Uncomment to enable.

		// Inverts the selected coolant pin from low-disabled/high-enabled to low-enabled/high-disabled. Useful
		// for some pre-built electronic boards.
		// #define INVERT_COOLANT_FLOOD_PIN // Default disabled. Uncomment to enable.
		// #define INVERT_COOLANT_MIST_PIN // Default disabled. Note: Enable M7 mist coolant in config.h

		#define STEPPERS_ALWAYS_ENABLED
































	#endif

#endif
