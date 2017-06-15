#include "wakeup.h"
#include "c_wakeup.h"

#include "k_stdtype.h"
#include "mal.h"
#ifdef WAKEUP_USE_LOGGER
	#include "logger.h"
#endif

ResetReason resetReason = ResetReason_Unknown;
unsigned int RCONtemp = 0;

void init_wakeup(void) {
	resetReason = mal_get_reset_reason(&RCONtemp);
}

void deinit_wakeup(void) {
}

void do_wakeup(void) {
	static uint8 singleShootWakeUp = 1;
	if (singleShootWakeUp) {
		singleShootWakeUp = 0;
		
		#ifdef WAKEUP_USE_LOGGER
			putString_logger("\r\n");
			putString_logger("*****************************************\r\n");
			putString_logger("* Device Start Up                       *\r\n");
			putString_logger("*****************************************\r\n");
			putString_logger("\r\n");
		#endif
			
		switch (resetReason) {
			case ResetReason_Unknown : {
				#ifdef WAKEUP_USE_LOGGER
					putString_logger("resetReason: ResetReason_Unknown\r\n");
				#endif
				break;
			}
			case ResetReason_Power_on_Reset : {
				#ifdef WAKEUP_USE_LOGGER
					putString_logger("resetReason: ResetReason_Power_on_Reset\r\n");
				#endif
				break;
			}
			case ResetReason_Brown_out_Reset : {
				#ifdef WAKEUP_USE_LOGGER
					putString_logger("resetReason: ResetReason_Brown_out_Reset\r\n");
				#endif
				break;
			}
			case ResetReason_MCLR_reset_during_normal_operation : {
				#ifdef WAKEUP_USE_LOGGER
					putString_logger("resetReason: ResetReason_MCLR_reset_during_normal_operation\r\n");
				#endif
				break;
			}
			case ResetReason_Software_reset_during_normal_operation : {
				#ifdef WAKEUP_USE_LOGGER
					putString_logger("resetReason: ResetReason_Software_reset_during_normal_operation\r\n");
				#endif
				break;
			}
			case ResetReason_MCLR_reset_during_sleep : {
				#ifdef WAKEUP_USE_LOGGER
					putString_logger("resetReason: ResetReason_MCLR_reset_during_sleep\r\n");
				#endif
				break;
			}
			case ResetReason_MCLR_reset_during_idle : {
				#ifdef WAKEUP_USE_LOGGER
					putString_logger("resetReason: ResetReason_MCLR_reset_during_idle\r\n");
				#endif
				break;
			}
			case ResetReason_WDT_Time_out_reset : {
				#ifdef WAKEUP_USE_LOGGER
					putString_logger("resetReason: ResetReason_WDT_Time_out_reset\r\n");
				#endif
				break;
			}
			case ResetReason_Configuration_Word_Mismatch_Reset : {
				#ifdef WAKEUP_USE_LOGGER
					putString_logger("resetReason: ResetReason_Configuration_Word_Mismatch_Reset\r\n");
				#endif
				break;
			}
			default : {
				#ifdef WAKEUP_USE_LOGGER
					putString_logger("resetReason: Unknown\r\n");
				#endif
				break;
			}
		}
	}
}

void isr_wakeup_1ms(void) {
}
