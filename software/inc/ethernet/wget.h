#ifndef _WGET_H_
#define _WGET_H_

	#include "k_stdtype.h"
	
	extern void init_wget(void);
	extern void do_wget(void);

	extern int wget_url(char *str);
	extern int wget_getByte(void);
	extern void wget_url_cancel(void);

#endif
