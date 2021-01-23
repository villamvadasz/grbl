#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

	#include "k_stdtype.h"

	typedef struct _ExceptionLog {
		unsigned int _excep_code;
		unsigned int _excep_addr;
		unsigned int magicWord;
	} ExceptionLog;

	#pragma GCC diagnostic error "-w"
		extern volatile ExceptionLog exceptionLog[16] __attribute__ ((persistent, address(0xA0000200)));
	#pragma GCC diagnostic pop

	extern void exceptions_clearException(void);
	extern uint32 exceptions_getException(void);

	extern void do_exception_examples(void);

#endif
