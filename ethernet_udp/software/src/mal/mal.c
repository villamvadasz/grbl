#include "mal.h"

volatile unsigned int gieTemp = 0;
volatile unsigned int isrLockCnt = 0;

void __attribute__((noreturn)) SoftReset(void);

void mal_reset(void) {
	SoftReset();
}

ResetReason mal_reset_reason(void) {
	ResetReason resetReason = ResetReason_Unknown;
	if (
				RCONbits.POR &&
				RCONbits.BOR
	) {
		resetReason = ResetReason_Power_on_Reset;
	} else if (
				RCONbits.POR &&
				!RCONbits.BOR
	) {
		resetReason = ResetReason_Brown_out_Reset;
	} else if (
				RCONbits.EXTR &&
				!RCONbits.SLEEP &&
				!RCONbits.IDLE
	) {
		resetReason = ResetReason_MCLR_reset_during_normal_operation;
	} else if (
				RCONbits.SWR
	) {
		resetReason = ResetReason_Software_reset_during_normal_operation;
	} else if (
				RCONbits.EXTR &&
				RCONbits.SLEEP &&
				!RCONbits.IDLE
	) {
		resetReason = ResetReason_MCLR_reset_during_sleep;
	} else if (
				RCONbits.EXTR &&
				!RCONbits.SLEEP &&
				RCONbits.IDLE
	) {
		resetReason = ResetReason_MCLR_reset_during_idle;
	} else if (
				RCONbits.WDTO
				//!RCONbits.SLEEP &&
				//!RCONbits.IDLE
	) {
		resetReason = ResetReason_WDT_Time_out_reset;
	} else if (
				RCONbits.CMR
	) {
		resetReason = ResetReason_Configuration_Word_Mismatch_Reset;
	} else {
		resetReason = ResetReason_Unknown;
	}
	RCON = 0;
	return resetReason;
}
