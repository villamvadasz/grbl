#ifndef _WGET_FSIO_H_
#define _WGET_FSIO_H_

	#include "k_stdtype.h"
	
	extern void init_wget_fsio(void);
	extern void do_wget_fsio(void);

	extern int wget_fsio_url(char *url, char *file);
	extern int wget_fsio_finished(void);
	extern void wget_fsio_url_cancel(void);

#endif
