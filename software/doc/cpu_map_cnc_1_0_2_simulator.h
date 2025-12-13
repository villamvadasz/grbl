#ifndef _CPU_MAP_CNC_1_0_2_SIMULATOR_H_
#define _CPU_MAP_CNC_1_0_2_SIMULATOR_H_

	#ifdef DEFAULTS_PIC32_MX_CNC_1_0_2_SIMULATOR
		#define CPU_MAP_PIC32_MX

		extern volatile unsigned int dummyPORT;
		extern volatile unsigned int dummyLAT;
		extern volatile unsigned int dummyTRIS;

		extern volatile unsigned int dummyENDSWITCH_X_PORT;
		extern volatile unsigned int dummyENDSWITCH_X_LAT;
		extern volatile unsigned int dummyENDSWITCH_X_TRIS;

		extern volatile unsigned int dummyENDSWITCH_Y_PORT;
		extern volatile unsigned int dummyENDSWITCH_Y_LAT;
		extern volatile unsigned int dummyENDSWITCH_Y_TRIS;

		extern volatile unsigned int dummyENDSWITCH_Z_PORT;
		extern volatile unsigned int dummyENDSWITCH_Z_LAT;
		extern volatile unsigned int dummyENDSWITCH_Z_TRIS;

		//#define STEP_X_DDR N/A
		//#define STEP_Y_DDR N/A
		//#define STEP_Z_DDR N/A
		//#define STEP_X_PORT N/A
		//#define STEP_Y_PORT N/A
		//#define STEP_Z_PORT N/A

		//#define DIRECTION_X_DDR N/A
		//#define DIRECTION_Y_DDR N/A
		//#define DIRECTION_Z_DDR N/A
		//#define DIRECTION_X_PORT N/A
		//#define DIRECTION_Y_PORT N/A
		//#define DIRECTION_Z_PORT N/A

		//#define STEPPERS_DISABLE_DDR N/A
		//#define STEPPERS_DISABLE_PORT N/A

		#define GRBL_SIMULATE_ENDSWITCH

		#define LIMIT_X_DDR dummyENDSWITCH_X_TRIS
		#define LIMIT_Y_DDR dummyENDSWITCH_Y_TRIS
		#define LIMIT_Z_DDR dummyENDSWITCH_Z_TRIS
		#define LIMIT_X_PIN dummyENDSWITCH_X_PORT
		#define LIMIT_Y_PIN dummyENDSWITCH_Y_PORT
		#define LIMIT_Z_PIN dummyENDSWITCH_Z_PORT

		//#define LIMIT_X_PIN_PUE N/A
		//#define LIMIT_Y_PIN_PUE N/A
		//#define LIMIT_Z_PIN_PUE N/A

		//#define LIMIT_X_PIN_CN N/A
		//#define LIMIT_Y_PIN_CN N/A
		//#define LIMIT_Z_PIN_CN N/A

		//#define SPINDLE_ENABLE_DDR N/A
		//#define SPINDLE_ENABLE_PORT N/A

		//#define SPINDLE_DIRECTION_DDR N/A
		//#define SPINDLE_DIRECTION_PORT N/A

		//#define SPINDLE_PWM_DDR N/A

		#define VARIABLE_SPINDLE_OC 2

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

		//#define PROBE_TRIS N/A
		//#define PROBE_PIN N/A
		//#define PROBE_PIN_PUE N/A
		//#define PROBE_PIN_CN N/A

		//#define PROBE_NEGATE_TRIS N/A
		//#define PROBE_NEGATE_PIN N/A
		//#define PROBE_NEGATE_PIN_PUE N/A
		//#define PROBE_NEGATE_PIN_CN N/A

		//#define EXTERNAL_POWER_TRIS N/A
		//#define EXTERNAL_POWER_PORT N/A
		//#define EXTERNAL_POWER_PORT_PUE N/A
		//#define EXTERNAL_POWER_PORT_CN N/A

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
		#define DEFAULT_X_STEPS_PER_MM 320.0
		#define DEFAULT_Y_STEPS_PER_MM 320.0
		#define DEFAULT_Z_STEPS_PER_MM 320.0
		#define DEFAULT_X_MAX_RATE 10000.0 // mm/min
		#define DEFAULT_Y_MAX_RATE 10000.0 // mm/min
		#define DEFAULT_Z_MAX_RATE 10000.0 // mm/min
		#define DEFAULT_X_ACCELERATION (150.0*60*60) // 1500*60*60 mm/min^2 = 1500 mm/sec^2
		#define DEFAULT_Y_ACCELERATION (150.0*60*60) // 1500*60*60 mm/min^2 = 1500 mm/sec^2
		#define DEFAULT_Z_ACCELERATION (150.0*60*60) // 1500*60*60 mm/min^2 = 1500 mm/sec^2
		#define DEFAULT_X_MAX_TRAVEL 2000.0 // mm NOTE: Must be a positive value.
		#define DEFAULT_Y_MAX_TRAVEL 1000.0 // mm NOTE: Must be a positive value.
		#define DEFAULT_Z_MAX_TRAVEL 500.0 // mm NOTE: Must be a positive value.

		#define DEFAULT_SPINDLE_ALGORITHM 3
		#define DEFAULT_SPINDLE_A (-0.000000000008)
		#define DEFAULT_SPINDLE_B (0.0000003)
		#define DEFAULT_SPINDLE_C (-0.0007)
		#define DEFAULT_SPINDLE_D (1.1373)

		#define DEFAULT_SPINDLE_RPM_MAX 24000.0 // rpm
		#define DEFAULT_SPINDLE_RPM_MIN 0.0 // rpm
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
		#define DEFAULT_HOMING_ENABLE 1  // false
		#define DEFAULT_HOMING_DIR_MASK 3 // move positive dir
		#define DEFAULT_HOMING_FEED_RATE 25.0 // mm/min
		#define DEFAULT_HOMING_SEEK_RATE 500.0 // mm/min
		#define DEFAULT_HOMING_DEBOUNCE_DELAY 250 // msec (0-65k)
		#define DEFAULT_HOMING_PULLOFF 2.0 // mm
		#define DEFAULT_AUTOMATIC_TOOL_CHANGE 0

		// Alters the behavior of the spindle enable pin with the option . By default,
		// Grbl will not disable the enable pin if spindle speed is zero and M3/4 is active, but still sets the PWM 
		// output to zero. This allows the users to know if the spindle is active and use it as an additional control
		// input. However, in some use cases, user may want the enable pin to disable with a zero spindle speed and 
		// re-enable when spindle speed is greater than zero. This option does that.
		#define SPINDLE_ENABLE_OFF_WITH_ZERO_SPEED // Default disabled. Uncomment to enable.

		//#define SPINDLE_FAKE_SPINDLE_RUNNING

		// Inverts the spindle enable pin from low-disabled/high-enabled to low-enabled/high-disabled. Useful
		// for some pre-built electronic boards.
		// #define INVERT_SPINDLE_ENABLE_PIN // Default disabled. Uncomment to enable.

		// Inverts the selected coolant pin from low-disabled/high-enabled to low-enabled/high-disabled. Useful
		// for some pre-built electronic boards.
		// #define INVERT_COOLANT_FLOOD_PIN // Default disabled. Uncomment to enable.
		// #define INVERT_COOLANT_MIST_PIN // Default disabled. Note: Enable M7 mist coolant in config.h

		#define STEPPERS_ALWAYS_ENABLED

		extern volatile unsigned int dummyPORT1;
		extern volatile unsigned int dummyPORT2;
		extern volatile unsigned int dummyPORT3;
		extern volatile unsigned int dummyPORT4;
		extern volatile unsigned int dummyPORT5;
		extern volatile unsigned int dummyPORT6;
		extern volatile unsigned int dummyPORT7;
		extern volatile unsigned int dummyLAT1;
		extern volatile unsigned int dummyLAT2;
		extern volatile unsigned int dummyLAT3;
		extern volatile unsigned int dummyLAT4;
		extern volatile unsigned int dummyLAT5;
		extern volatile unsigned int dummyLAT6;
		extern volatile unsigned int dummyLAT7;
		extern volatile unsigned int dummyTRIS1;
		extern volatile unsigned int dummyTRIS2;
		extern volatile unsigned int dummyTRIS3;
		extern volatile unsigned int dummyTRIS4;
		extern volatile unsigned int dummyTRIS5;
		extern volatile unsigned int dummyTRIS6;
		extern volatile unsigned int dummyTRIS7;


















	#endif

#endif
