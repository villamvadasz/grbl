/*
  spindle_control.c - spindle control methods
  Part of Grbl

  Copyright (c) 2012-2016 Sungeun K. Jeon for Gnea Research LLC
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

/*M3 S20000 spindle on at 20000rpm*/ 
/*M5 off*/ 
/*M4 S20000 spindle on at 20000rpm counterclock wise*/ 
#include <math.h>
#include "grbl.h"

#include "k_stdtype.h"
#include "pwm.h"


#ifndef VARIABLE_SPINDLE_OC
	float pwm_dummy_value = 0.0f;
#endif
#ifdef SPINDLE_FAKE_SPINDLE_RUNNING
	uint8 spindle_control_fake_spindle_run = 0;
#endif

float spindle_compute_pwm_value(float rpm);

void spindle_init(void) {
	#ifndef VARIABLE_SPINDLE_OC
		pwm_dummy_value = 0.0f;
	#endif
	#ifdef SPINDLE_FAKE_SPINDLE_RUNNING
		spindle_control_fake_spindle_run = 0;
	#endif

	{
		#ifdef VARIABLE_SPINDLE_OC
			extern float maxFreq;
			setPwmFreq(VARIABLE_SPINDLE_OC, maxFreq / 4.0);
		#endif
	}

	system_log_spindle_off(20);
	spindle_set_state(SPINDLE_DISABLE, 0.0f);

	#ifdef SPINDLE_PWM_DDR
		SPINDLE_PWM_DDR = 0;
	#endif
	#ifdef SPINDLE_ENABLE_DDR
		SPINDLE_ENABLE_DDR = 0;
	#endif
	#ifdef SPINDLE_DIRECTION_DDR
		SPINDLE_DIRECTION_DDR = 0;
	#endif

}

void do_spindle(void) {
}

uint8 spindle_get_state(void) {
	uint8 result = SPINDLE_STATE_DISABLE;
	float pwm_value = 0.0;
	#ifdef VARIABLE_SPINDLE_OC
		pwm_value = getPwmDuty(VARIABLE_SPINDLE_OC);
	#else
		pwm_value = pwm_dummy_value;
	#endif
	#ifdef SPINDLE_INVERT_GRADIENT
		pwm_value = 100.0 - pwm_value;
	#endif
	if (pwm_value > 0.0) { // Check if PWM is enabled.
		#ifdef SPINDLE_DIRECTION_PORT
			if (SPINDLE_DIRECTION_PORT) {
				result = SPINDLE_STATE_CCW;
			} else {
				result = SPINDLE_STATE_CW;
			}
		#else
			result = SPINDLE_STATE_CW;
		#endif
	}
	#ifdef SPINDLE_ENABLE_PORT
		#ifdef INVERT_SPINDLE_ENABLE_PIN
			if (SPINDLE_ENABLE_PORT == 0) {
				result = SPINDLE_STATE_DISABLE;
			}
		#else
			if (SPINDLE_ENABLE_PORT != 0) {
				result = SPINDLE_STATE_DISABLE;
			}
		#endif
	#endif
	
	#ifdef SPINDLE_FAKE_SPINDLE_RUNNING
		if (spindle_control_fake_spindle_run) {
			result = SPINDLE_STATE_CW;
		}
	#endif
	
	return result;	
}

// Immediately sets spindle running state with direction and spindle rpm via PWM, if enabled.
// Called by g-code parser spindle_sync(), parking retract and restore, g-code program end,
// sleep, and spindle stop override.
void spindle_set_state(uint8 state, float rpm) {
	uint8 localEnableDisable = 0;
	float pwm_value = 0;
	uint8 internal_spindle_var = 0;
	
	if (isfinite(rpm)) {
		//value is ok
	} else {
		state = SPINDLE_DISABLE;
		rpm = 0.0f;
	}
	#ifdef SPINDLE_FAKE_SPINDLE_RUNNING
		//Fake that spindle is running so that eeprom is not touched.
		//Good for engraving with not running spindle.
		spindle_control_fake_spindle_run = 0;
		if ((rpm > 0.5f) && (rpm < 1.5f)) {
			rpm = 0.0f;
			spindle_control_fake_spindle_run = 1;
		}
	#endif

	#ifdef SPINDLE_ENABLE_OFF_WITH_ZERO_SPEED
		if ((state == SPINDLE_DISABLE) || (rpm == 0.0f)) { // Halt or set spindle direction and rpm.
			internal_spindle_var = 1;
		} else {
			internal_spindle_var = 0;
		}
	#else
		if (state == SPINDLE_DISABLE) { // Halt or set spindle direction and rpm.
			internal_spindle_var = 1;
		} else {
			internal_spindle_var = 0;
		}
	#endif
	
	if (internal_spindle_var) {
		pwm_value = spindle_compute_pwm_value(0.0f);
		sys.spindle_speed = 0.0;
		localEnableDisable = 0;
	} else {
		pwm_value = spindle_compute_pwm_value(rpm);
		// NOTE: Assumes all calls to this function is when Grbl is not moving or must remain off.
		if (settings.flags & BITFLAG_LASER_MODE) {
			if (state == SPINDLE_ENABLE_CCW) {
				rpm = 0.0;// TODO: May need to be rpm_min*(100/MAX_SPINDLE_SPEED_OVERRIDE);
			} 
		}
		localEnableDisable = 1;
	}
	sys.report_ovr_counter = 0; // Set to report change immediately

	#ifdef VARIABLE_SPINDLE_OC
		setPwmDuty(VARIABLE_SPINDLE_OC, pwm_value);
	#else
		pwm_dummy_value = pwm_value;
	#endif

	#ifdef SPINDLE_DIRECTION_PORT
		if (state != SPINDLE_ENABLE_LASTW) {
			if (state == SPINDLE_ENABLE_CW) {
				SPINDLE_DIRECTION_PORT = 0;
			} else {
				SPINDLE_DIRECTION_PORT = 1;
			}
		}
	#endif

	if (localEnableDisable == 0) {
		#ifdef SPINDLE_ENABLE_PORT
			#ifdef INVERT_SPINDLE_ENABLE_PIN
				SPINDLE_ENABLE_PORT = 1;
			#else
				SPINDLE_ENABLE_PORT = 0;
			#endif
		#endif
	} else {
		#ifdef SPINDLE_ENABLE_PORT
			#ifdef INVERT_SPINDLE_ENABLE_PIN
				SPINDLE_ENABLE_PORT = 0;
			#else
				SPINDLE_ENABLE_PORT = 1;
			#endif
		#endif
	}
}

float spindle_compute_pwm_value(float rpm) {
	float result = 0.0f;
	float pwm_value = 0.0f;
	
	if (isfinite(rpm)) {
		//value is ok
	} else {
		rpm = 0.0f;
	}

	rpm *= (0.010f * sys.spindle_speed_ovr); // Scale by spindle speed override value.
	// Calculate PWM register value based on rpm max/min settings and programmed rpm.
	if ((settings.rpm_min >= settings.rpm_max) || (rpm >= settings.rpm_max)) {
		// No PWM range possible. Set simple on/off spindle control pin state.
		sys.spindle_speed = settings.rpm_max;
		pwm_value = 100.0f;
	} else if (rpm <= settings.rpm_min) {
		if (rpm == 0.0f) { // S0 disables spindle
			sys.spindle_speed = 0.0f;
			pwm_value = 0.0f;
		} else { // Set minimum PWM output
			sys.spindle_speed = settings.rpm_min;
			pwm_value = 0.0f;
		}
	} else { 
		float pwm_gradient = 100.0 / (settings.rpm_max - settings.rpm_min);
		// Compute intermediate PWM value with linear spindle speed model.
		// NOTE: A nonlinear model could be installed here, if required.
		sys.spindle_speed = rpm;
		
		if (settings.spindle_algorithm == 1) {
			float A = settings.spindle_a;
			float B = settings.spindle_b;
			float rpm1 = rpm;
			pwm_value += (A*rpm1);
			pwm_value += B;
		} else if (settings.spindle_algorithm == 2) {
			float A = settings.spindle_a;
			float B = settings.spindle_b;
			float C = settings.spindle_c;
			float rpm1 = rpm;
			float rpm2 = rpm1 * rpm;
			pwm_value += (A*rpm2);
			pwm_value += (B*rpm1);
			pwm_value += C;
		} else if (settings.spindle_algorithm == 3) {
			float A = settings.spindle_a;
			float B = settings.spindle_b;
			float C = settings.spindle_c;
			float D = settings.spindle_d;
			float rpm1 = rpm;
			float rpm2 = rpm1 * rpm;
			float rpm3 = rpm2 * rpm;
			pwm_value = (A*rpm3);
			pwm_value += (B*rpm2);
			pwm_value += (C*rpm1);
			pwm_value += D;
		} else {//0
			pwm_value = ((rpm - settings.rpm_min) * pwm_gradient);
		}
		if (pwm_value >= 100.0) {
			pwm_value = 100.0;
		} else if (pwm_value <= 0.0) {
			pwm_value = 0.0;
		}
	}
	#ifdef SPINDLE_INVERT_GRADIENT
		pwm_value = 100.0f - pwm_value;
	#endif
	
	if (isfinite(pwm_value)) {
		//value is ok can be used later
	} else {
		pwm_value = 0.0f;
	}

	result = pwm_value;
	
	return result;
}
