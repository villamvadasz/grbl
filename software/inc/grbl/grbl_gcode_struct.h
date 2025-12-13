/*
  grbl_gcode.h - rs274/ngc parser.
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

#ifndef gcode_struct_h
#define gcode_struct_h


// NOTE: When this struct is zeroed, the above defines set the defaults for the system.
typedef struct _gc_modal_t{
  uint8 motion;          // {G0,G1,G2,G3,G38.2,G80}
  uint8 feed_rate;       // {G93,G94}
  uint8 units;           // {G20,G21}
  uint8 distance;        // {G90,G91}
  // uint8 distance_arc; // {G91.1} NOTE: Don't track. Only default supported.
  uint8 plane_select;    // {G17,G18,G19}
  // uint8 cutter_comp;  // {G40} NOTE: Don't track. Only default supported.
  uint8 tool_length;     // {G43.1,G49}
  uint8 coord_select;    // {G54,G55,G56,G57,G58,G59}
  // uint8 control;      // {G61} NOTE: Don't track. Only default supported.
  uint8 program_flow;    // {M0,M1,M2,M30}
  uint8 coolant;         // {M7,M8,M9}
  uint8 spindle;         // {M3,M4,M5}
  uint8 override;        // {M56}
} gc_modal_t;

typedef struct _gc_values_t{
  float f;         // Feed
  float ijk[3];    // I,J,K Axis arc offsets
  uint8 l;       // G10 or canned cycles parameters
  int32_t n;       // Line number
  float p;         // G10 or dwell parameters
  // float q;      // G82 peck drilling
  float r;         // Arc radius
  float s;         // Spindle speed
  uint8 t;       // Tool selection
  float xyz[3];    // X,Y,Z Translational axes
} gc_values_t;

typedef struct _parser_state_t{
  gc_modal_t modal;

  float spindle_speed;          // RPM
  float feed_rate;              // Millimeters/min
  uint8 tool_requested;         // Tracks tool number. NOT USED.
  uint8 tool_actual;            // Tracks tool number. NOT USED.
  int32_t line_number;          // Last line number sent

  float position[N_AXIS];       // Where the interpreter considers the tool to be at this point in the code

  float coord_system[N_AXIS];    // Current work coordinate system (G54+). Stores offset from absolute machine
                                 // position in mm. Loaded from EEPROM when called.
  float coord_offset[N_AXIS];    // Retains the G92 coordinate offset (work coordinates) relative to
                                 // machine zero in mm. Non-persistent. Cleared upon reset and boot.
  float tool_length_offset;      // Tracks tool length offset value when enabled.
} parser_state_t;

typedef struct _parser_block_t{
  uint8 non_modal_command;
  gc_modal_t modal;
  gc_values_t values;
} parser_block_t;

#endif
