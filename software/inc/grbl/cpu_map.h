/*
  cpu_map.h - CPU and pin mapping configuration file
  Part of Grbl

  Copyright (c) 2012-2016 Sungeun K. Jeon for Gnea Research LLC

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

/* The cpu_map.h files serve as a central pin mapping selection file for different
   processor types or alternative pin layouts. This version of Grbl officially supports
   only the Arduino Mega328p. */


#ifndef cpu_map_h
#define cpu_map_h

	#include "c_grbl_config.h"
	#include "mal.h"
	#include "cpu_map_arduino.h"
	#include "cpu_map_cnc_1_0_0.h"
	#include "cpu_map_cnc_1_0_0_debug.h"
	#include "cpu_map_cnc_1_0_0_mz.h"
	#include "cpu_map_cnc_1_0_0_simulator.h"
	#include "cpu_map_cnc_1_0_0_old_grbl.h"
	#include "cpu_map_cnc_1_0_2.h"
	#include "cpu_map_cnc_1_0_2_simulator.h"

#endif
