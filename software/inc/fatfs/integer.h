/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _FF_INTEGER
#define _FF_INTEGER

#ifdef _WIN32	/* FatFs development platform */

#include <windows.h>
#include <tchar.h>

#else			/* Embedded platform */

#include "k_stdtype.h"
#include "GenericTypeDefs.h"

/* These types must be 16-bit, 32-bit or larger integer */
//typedef sint16			INT;
//typedef uint16			UINT;

/* These types must be 8-bit integer */
//typedef sint8			CHAR;
//typedef uint8			UCHAR;
//typedef uint8			BYTE;

/* These types must be 16-bit integer */
//typedef sint16			SHORT;
//typedef uint16			USHORT;
//typedef uint16			WORD;
//typedef uint16			WCHAR;

/* These types must be 32-bit integer */
//typedef sint32			LONG;
//typedef uint32			ULONG;
//typedef uint32			DWORD;

#endif

#endif
