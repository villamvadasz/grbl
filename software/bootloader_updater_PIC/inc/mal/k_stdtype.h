#ifndef _K_STDTYPE_H_
#define _K_STDTYPE_H_

	#include <stdint.h>
	#include <stdbool.h> 

	typedef char sint8;
	typedef unsigned char uint8;
	typedef short sint16;
	typedef unsigned short uint16;
	typedef int sint32;
	typedef unsigned int uint32;
	typedef long long sint64;
	typedef unsigned long long uint64;

	typedef uint8	BYTE;
	typedef uint16	WORD;
	typedef signed int		  INT;
	typedef signed char		 INT8;
	typedef signed short int	INT16;
	typedef signed long int	 INT32;
	typedef unsigned int		UINT;
	typedef unsigned char	   UINT8;
	typedef unsigned short int  UINT16;
	typedef unsigned long int   UINT32;	 /* other name for 32-bit integer */

	#if defined(__GNUC__)
	#define __EXTENSION __extension__
	#else
	#define __EXTENSION
	#endif
	/* Alternate definitions */
	typedef void					VOID;

	typedef char					CHAR8;
	typedef unsigned char		   UCHAR8;

	//typedef unsigned char		   BYTE;						   /* 8-bit unsigned  */
	//typedef unsigned short int	  WORD;						   /* 16-bit unsigned */
	typedef unsigned long		   DWORD;						  /* 32-bit unsigned */
	/* MPLAB C Compiler for PIC18 does not support 64-bit integers */
	__EXTENSION
	typedef unsigned long long	  QWORD;						  /* 64-bit unsigned */
	typedef signed char			 CHAR;						   /* 8-bit signed	*/
	typedef signed short int		SHORT;						  /* 16-bit signed   */
	typedef signed long			 LONG;						   /* 32-bit signed   */
	/* MPLAB C Compiler for PIC18 does not support 64-bit integers */
	__EXTENSION
	typedef signed long long		LONGLONG;					   /* 64-bit signed   */
	/* INT is processor specific in length may vary in size */

	/* MPLAB C Compiler for PIC18 does not support 64-bit integers */
	#if !defined(__18CXX)
	__EXTENSION typedef signed long long	INT64;
	#endif

	/* UINT is processor specific in length may vary in size */
	/* 24-bit type only available on C18 */
	#if defined(__18CXX)
	typedef unsigned short long UINT24;
	#endif
	/* MPLAB C Compiler for PIC18 does not support 64-bit integers */
	#if !defined(__18CXX)
	__EXTENSION typedef unsigned long long  UINT64;
	#endif

	typedef union _uint32_to_uint8 {
		uint32 var_uint32;
		uint16 var_uint16[2];
		uint8 var_uint8[4];
	} uint32_to_uint8;

#endif
