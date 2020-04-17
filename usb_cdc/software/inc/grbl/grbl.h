/*
  grbl.h - main Grbl include file
  Part of Grbl

  Copyright (c) 2015-2016 Sungeun K. Jeon for Gnea Research LLC

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

#ifndef grbl_h
#define grbl_h

#include "c_grbl_config.h"

// Grbl versioning system
#define GRBL_VERSION "1.1f"
#define GRBL_VERSION_BUILD "20170131"

extern void set_grbl_tmr0_trigger(uint16 trigger);
extern void set_grbl_tmr0_trigger_Step_Pulse_Delay(uint16 trigger);
extern void set_grbl_tmr1_period(uint16 period);
extern void set_grbl_tmr1_enabled(void);
extern void set_grbl_tmr1_disabled(void);
extern void grbl_eeprom_trigger(void);
extern void grbl_eeprom_trigger_settings(void);
extern void grbl_eeprom_trigger_items(void);

extern void grbl_NotifyUserFailedRead(int item, uint8 type);
extern void grbl_reset_eep_messages(void);

extern void do_grbl(void);
extern void init_grbl(void);
extern void isr_grbl_1ms(void);
extern void isr_grbl_tmr_stepper(void);//tmr3
extern void isr_grbl_tmr_stepper_delay(void);//tmr4
extern void isr_grbl_cn(void);

// ---------------------------------------------------------------------------------------

#endif
