/*
  motion_control.c - high level interface for issuing motion commands
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
#include <math.h>
#include <string.h>
#include "grbl.h"

#include "k_stdtype.h"

volatile uint8 do_ArcGenerating = 0;

float target_buffer[N_AXIS];
plan_line_data_t pl_data_buffer;
float position_buffer[N_AXIS];
float offset_buffer[N_AXIS];
float radius_buffer;
uint8 axis_0_buffer;
uint8 axis_1_buffer;
uint8 axis_linear_buffer;
uint8 is_clockwise_arc_buffer;
uint16 segments_buffer;
uint16 i_buffer;
float sin_Ti;
float cos_Ti;
float r_axisi;
uint8 count = 0;
float r_axis0;  // Radius vector from center to current location
float r_axis1;
float cos_T;
float sin_T;
float theta_per_segment;
float linear_per_segment;
float center_axis0;
float center_axis1;

void mc_arc_pre(float *target, plan_line_data_t *pl_data, float *position, float *offset, float radius, uint8 axis_0, uint8 axis_1, uint8 axis_linear, uint8 is_clockwise_arc);
uint8 mc_arc_async(float *target, plan_line_data_t *pl_data, float *position, float *offset, float radius, uint8 axis_0, uint8 axis_1, uint8 axis_linear, uint8 is_clockwise_arc);

void init_motion_control(void) {
	do_ArcGenerating = 0;

	memset(target_buffer, 0, sizeof(target_buffer));
	memset(&pl_data_buffer, 0, sizeof(pl_data_buffer));
	memset(position_buffer, 0, sizeof(position_buffer));
	memset(offset_buffer, 0, sizeof(offset_buffer));
	radius_buffer = 0.0f;
	axis_0_buffer = 0;
	axis_1_buffer = 0;
	axis_linear_buffer = 0;
	is_clockwise_arc_buffer = 0;
	segments_buffer = 0;
	i_buffer = 0;
	sin_Ti = 0.0f;
	cos_Ti = 0.0f;
	r_axisi = 0.0f;
	count = 0;
	r_axis0 = 0.0f;
	r_axis1 = 0.0f;
	cos_T = 0.0f;
	sin_T = 0.0f;
	theta_per_segment = 0.0f;
	linear_per_segment = 0.0f;
	center_axis0 = 0.0f;
	center_axis1 = 0.0f;

	init_homing();
}

void do_motion_control(void) {
	if (do_ArcGenerating) {
		if (plan_check_full_buffer()) {
			//buffer is full wait for next call
		} else {
			if (mc_arc_async(target_buffer, &pl_data_buffer, position_buffer, offset_buffer, radius_buffer, axis_0_buffer, axis_1_buffer, axis_linear_buffer, is_clockwise_arc_buffer) == 0) {
				do_ArcGenerating = 0;
			}
		}
	}
	
	do_homing();
}

uint8 isArcGeneratingRunning(void) {
	uint8 result = 0;
	if (do_ArcGenerating != 0) {
		result = 1;
	}
	return result;
}

// Execute linear motion in absolute millimeter coordinates. Feed rate given in millimeters/second
// unless invert_feed_rate is true. Then the feed_rate means that the motion should be completed in
// (1 minute)/feed_rate time.
// NOTE: This is the primary gateway to the grbl planner. All line motions, including arc line
// segments, must pass through this routine before being passed to the planner. The seperation of
// mc_line and plan_buffer_line is done primarily to place non-planner-type functions from being
// in the planner and to let backlash compensation or canned cycle integration simple and direct.
void mc_line(float *target, plan_line_data_t *pl_data) {
	// If enabled, check for soft limit violations. Placed here all line motions are picked up
	// from everywhere in Grbl.
	if (bit_istrue(settings.flags,BITFLAG_SOFT_LIMIT_ENABLE)) {
		// NOTE: Block jog state. Jogging is a special case and soft limits are handled independently.
		if (sys.state != STATE_JOG) { 
			limits_soft_check(target);
		}
	}

	// NOTE: Backlash compensation may be installed here. It will need direction info to track when
	// to insert a backlash line motion(s) before the intended line motion and will require its own
	// plan_check_full_buffer() and check for system abort loop. Also for position reporting
	// backlash steps will need to be also tracked, which will need to be kept at a system level.
	// There are likely some other things that will need to be tracked as well. However, we feel
	// that backlash compensation should NOT be handled by Grbl itself, because there are a myriad
	// of ways to implement it and can be effective or ineffective for different CNC machines. This
	// would be better handled by the interface as a post-processor task, where the original g-code
	// is translated and inserts backlash motions that best suits the machine.
	// NOTE: Perhaps as a middle-ground, all that needs to be sent is a flag or special command that
	// indicates to Grbl what is a backlash compensation motion, so that Grbl executes the move but
	// doesn't update the machine position values. Since the position values used by the g-code
	// parser and planner are separate from the system machine positions, this is doable.

	// Plan and queue motion into planner buffer
	// uint8 plan_status; // Not used in normal operation.
	plan_buffer_line(target, pl_data);
}

// Execute an arc in offset mode format. position == current xyz, target == target xyz,
// offset == offset from current xyz, axis_X defines circle plane in tool space, axis_linear is
// the direction of helical travel, radius == circle radius, isclockwise boolean. Used
// for vector transformation direction.
// The arc is approximated by generating a huge number of tiny, linear segments. The chordal tolerance
// of each segment is configured in settings.arc_tolerance, which is defined to be the maximum normal
// distance from segment to the circle when the end points both lie on the circle.
void mc_arc(float *target, plan_line_data_t *pl_data, float *position, float *offset, float radius, uint8 axis_0, uint8 axis_1, uint8 axis_linear, uint8 is_clockwise_arc) {
	uint32 x = 0;
	for (x = 0; x < N_AXIS; x++) {
		target_buffer[x] = target[x];
		position_buffer[x] = position[x];
		offset_buffer[x] = offset[x];
	}
	pl_data_buffer = *pl_data;
	radius_buffer = radius;
	axis_0_buffer = axis_0;
	axis_1_buffer = axis_1;
	axis_linear_buffer = axis_linear;
	is_clockwise_arc_buffer = is_clockwise_arc;
	
	mc_arc_pre(target_buffer, &pl_data_buffer, position_buffer, offset_buffer, radius_buffer, axis_0_buffer, axis_1_buffer, axis_linear_buffer, is_clockwise_arc_buffer);

	do_ArcGenerating = 1;
}

void mc_arc_pre(float *target, plan_line_data_t *pl_data, float *position, float *offset, float radius, uint8 axis_0, uint8 axis_1, uint8 axis_linear, uint8 is_clockwise_arc) {

	center_axis0 = position[axis_0] + offset[axis_0];
	center_axis1 = position[axis_1] + offset[axis_1];
	r_axis0 = -offset[axis_0];  // Radius vector from center to current location
	r_axis1 = -offset[axis_1];
	float rt_axis0 = target[axis_0] - center_axis0;
	float rt_axis1 = target[axis_1] - center_axis1;

	// CCW angle between position and target from circle center. Only one atan2() trig computation required.
	float angular_travel = atan2(r_axis0*rt_axis1-r_axis1*rt_axis0, r_axis0*rt_axis0+r_axis1*rt_axis1);
	if (is_clockwise_arc) { // Correct atan2 output per direction
		if (angular_travel >= -ARC_ANGULAR_TRAVEL_EPSILON) {
			angular_travel -= 2*M_PI;
		}
	} else {
		if (angular_travel <= ARC_ANGULAR_TRAVEL_EPSILON) {
			angular_travel += 2*M_PI;
		}
	}

	// NOTE: Segment end points are on the arc, which can lead to the arc diameter being smaller by up to
	// (2x) settings.arc_tolerance. For 99% of users, this is just fine. If a different arc segment fit
	// is desired, i.e. least-squares, midpoint on arc, just change the mm_per_arc_segment calculation.
	// For the intended uses of Grbl, this value shouldn't exceed 2000 for the strictest of cases.
	segments_buffer = floor(fabs(0.5*angular_travel*radius) / sqrt(settings.arc_tolerance*(2*radius - settings.arc_tolerance)) );

	if (segments_buffer) {
		// Multiply inverse feed_rate to compensate for the fact that this movement is approximated
		// by a number of discrete segments. The inverse feed_rate should be correct for the sum of
		// all segments.
		if (pl_data->condition & PL_COND_FLAG_INVERSE_TIME) { 
			pl_data->feed_rate *= segments_buffer; 
			pl_data->condition &= ~(PL_COND_FLAG_INVERSE_TIME); // Force as feed absolute mode over arc segments.
		} else if (pl_data->condition_2 & PL_COND_FLAG_PER_REVOLUTION) {
			pl_data->feed_rate = nuts_bolts_tooth_load_rpm_to_feed(pl_data->feed_rate, pl_data->spindle_speed, 1.0f);
			pl_data->condition_2 &= ~(PL_COND_FLAG_PER_REVOLUTION); // Force as feed absolute mode over arc segments.
		}
		theta_per_segment = angular_travel / segments_buffer;
		linear_per_segment = (target[axis_linear] - position[axis_linear])/segments_buffer;

		/* Vector rotation by transformation matrix: r is the original vector, r_T is the rotated vector,
		and phi is the angle of rotation. Solution approach by Jens Geisler.
		r_T = [cos(phi) -sin(phi);
		sin(phi)  cos(phi] * r ;

		For arc generation, the center of the circle is the axis of rotation and the radius vector is
		defined from the circle center to the initial position. Each line segment is formed by successive
		vector rotations. Single precision values can accumulate error greater than tool precision in rare
		cases. So, exact arc path correction is implemented. This approach avoids the problem of too many very
		expensive trig operations [sin(),cos(),tan()] which can take 100-200 usec each to compute.

		Small angle approximation may be used to reduce computation overhead further. A third-order approximation
		(second order sin() has too much error) holds for most, if not, all CNC applications. Note that this
		approximation will begin to accumulate a numerical drift error when theta_per_segment is greater than
		~0.25 rad(14 deg) AND the approximation is successively used without correction several dozen times. This
		scenario is extremely unlikely, since segment lengths and theta_per_segment are automatically generated
		and scaled by the arc tolerance setting. Only a very large arc tolerance setting, unrealistic for CNC
		applications, would cause this numerical drift error. However, it is best to set N_ARC_CORRECTION from a
		low of ~4 to a high of ~20 or so to avoid trig operations while keeping arc generation accurate.

		This approximation also allows mc_arc to immediately insert a line segment into the planner
		without the initial overhead of computing cos() or sin(). By the time the arc needs to be applied
		a correction, the planner should have caught up to the lag caused by the initial mc_arc overhead.
		This is important when there are successive arc motions.
		*/
		// Computes: cos_T = 1 - theta_per_segment^2/2, sin_T = theta_per_segment - theta_per_segment^3/6) in ~52usec
		cos_T = 2.0 - theta_per_segment*theta_per_segment;
		sin_T = theta_per_segment*0.16666667*(cos_T + 4.0);
		cos_T *= 0.5;

		sin_Ti = 0.0f;
		cos_Ti = 0.0f;
		r_axisi = 0.0f;
		count = 0;

		i_buffer = 1;

		//from this point mc_arc_async takes the work
	} else {
		do_ArcGenerating = 0;
		// Ensure last segment arrives at target location.
		mc_line(target, pl_data);  
	}
}
  
uint8 mc_arc_async(float *target, plan_line_data_t *pl_data, float *position, float *offset, float radius, uint8 axis_0, uint8 axis_1, uint8 axis_linear, uint8 is_clockwise_arc) {
	uint8 result = 1;
	if (i_buffer < segments_buffer) {
		if (count < N_ARC_CORRECTION) {
			// Apply vector rotation matrix. ~40 usec
			r_axisi = r_axis0*sin_T + r_axis1*cos_T;
			r_axis0 = r_axis0*cos_T - r_axis1*sin_T;
			r_axis1 = r_axisi;
			count++;
		} else {
			// Arc correction to radius vector. Computed only every N_ARC_CORRECTION increments. ~375 usec
			// Compute exact location by applying transformation matrix from initial radius vector(=-offset).
			cos_Ti = cos(i_buffer*theta_per_segment);
			sin_Ti = sin(i_buffer*theta_per_segment);
			r_axis0 = -offset[axis_0]*cos_Ti + offset[axis_1]*sin_Ti;
			r_axis1 = -offset[axis_0]*sin_Ti - offset[axis_1]*cos_Ti;
			count = 0;
		}
		// Update arc_target location
		position[axis_0] = center_axis0 + r_axis0;
		position[axis_1] = center_axis1 + r_axis1;
		position[axis_linear] += linear_per_segment;

		mc_line(position, pl_data);
		i_buffer++;// Increment (segments_buffer-1).
	} else {
		result = 0;
	    // Ensure last segment arrives at target location.
	    mc_line(target, pl_data);  
	}
	return result;
}

void mc_dwell(float time) {
	//mc_dwell_s(time); //former implementation was using [s]
	mc_dwell_ms(time); //this is better
}


void mc_dwell_ms(float milliseconds) {
	if (milliseconds >= 10000.0) {
		milliseconds = 10000.0;
	} else if (milliseconds < 1.0) {
		milliseconds = 1.0;
	}
	protocol_set_requestSynchMotion();
	protocol_set_requestDwell(milliseconds);
}

// Execute dwell in seconds.
void mc_dwell_s(float seconds) {
	float ms_time = seconds * 1000.0;
	mc_dwell_ms(ms_time);
}

// Former mc_reset now does not reset the uC, instead of it stops all motion and flush all buffers
void mc_stop(void) {
	// Kill spindle and coolant.
	system_log_spindle_off(30);
	spindle_set_state(SPINDLE_DISABLE, 0.0f);
	coolant_stop();

	// Kill steppers only if in any motion state, i.e. cycle, actively holding, or homing.
	// NOTE: If steppers are kept enabled via the step idle delay setting, this also keeps
	// the steppers enabled by avoiding the go_idle call altogether, unless the motion state is
	// violated, by which, all bets are off.
	if ((sys.state & (STATE_CYCLE | STATE_HOMING | STATE_JOG)) || (sys.step_control & (STEP_CONTROL_EXECUTE_HOLD | STEP_CONTROL_EXECUTE_SYS_MOTION))) {
		if (sys.state == STATE_HOMING) {
		} else { 
			system_set_exec_alarm(EXEC_ALARM_ABORT_CYCLE);
		}
		system_log_sm_plan_reset(3);
		plan_reset();
		system_log_st_reset(2);
		st_reset();
		// Sync cleared gcode and planner positions to current system position.
		plan_sync_position();
		gc_sync_position();
		system_set_exec_state_flag(EXEC_CYCLE_STOP); // Flag main program for cycle end
	}
}
