/**************************************************************************/
/*                                                                        */
/*      Copyright (c) 2005-2023 by ITTIA L.L.C. All rights reserved.      */
/*                                                                        */
/*  This software is copyrighted by and is the sole property of ITTIA     */
/*  L.L.C.  All rights, title, ownership, or other interests in the       */
/*  software remain the property of ITTIA L.L.C.  This software may only  */
/*  be used in accordance with the corresponding license agreement.  Any  */
/*  unauthorized use, duplication, transmission, distribution, or         */
/*  disclosure of this software is expressly forbidden.                   */
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */
/*  written consent of ITTIA L.L.C.                                       */
/*                                                                        */
/*  ITTIA L.L.C. reserves the right to modify this software without       */
/*  notice.                                                               */
/*                                                                        */
/*  info@ittia.com                                                        */
/*  https://www.ittia.com                                                 */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

#ifndef OS_STD_TYPES_H
#define OS_STD_TYPES_H

/* define std types and supporting constants for ITTIA DB.
 * @todo: investigate Single UNIX spec's inttypes.h
 */

#include "ittia/os/os_env.h"

#ifdef HAVE_STDBOOL_H
#   include <stdbool.h>
#endif

#ifdef HAVE_STDINT_H
#	ifndef __STDC_CONSTANT_MACROS
#		define __STDC_CONSTANT_MACROS
#	endif
#	ifndef __STDC_LIMIT_MACROS
#		define __STDC_LIMIT_MACROS
#	endif
#	include <stdint.h>

/* Work-around for bug in Rowley C Compiler's stdint.h. */
#if defined(OS_CROSSWORKS) && !defined(UINT8_C) && defined(UINT8)
#define UINT8_C UINT8
#endif

/* UINT64_C in VxWorks stdint.h does not work correctly with gcc. */
#if defined(OS_VXWORKS) && defined(OS_GNUC)
#	undef INT64_C
#	undef UINT64_C
#	define INT64_C(x)    x##LL
#	define UINT64_C(x)   x##ULL
#endif

#else

/* when the environment doesn't provides these types we try to
 * guess correct ones
 */

/* signed 8 bits integer */
#ifndef OS_VXWORKS_55
#ifndef OS_MINGW
typedef signed char      int8_t;
typedef unsigned char    uint8_t;
typedef short int        int16_t;
typedef unsigned short   uint16_t;
typedef signed int       int32_t;
typedef unsigned int     uint32_t;
#endif
#endif

#	define INT8_C(x)     ((int8_t)  +(x))
#	define INT16_C(x)    ((int16_t) +(x))
#	define INT32_C(x)    ((int32_t) x##L)

#	define UINT8_C(x)    ((uint8_t)  +(x##U))
#	define UINT16_C(x)   ((uint16_t) +(x##U))
#	define UINT32_C(x)   ((uint32_t) x##UL)

#	define INT8_MIN      SCHAR_MIN
#	define INT8_MAX      SCHAR_MAX
#	define INT16_MIN     SHRT_MIN
#	define INT16_MAX     SHRT_MAX
#	define INT32_MIN     LONG_MIN
#	define INT32_MAX     LONG_MAX

#	define UINT8_MAX     UINT8_C(0xff)
#	define UINT16_MAX    UINT16_C(0xffff)
#	define UINT32_MAX    UINT32_C(0xffffffff)

/* 64 bits integers */

#ifdef OS_MSC

#	include <limits.h>

	typedef signed __int64     int64_t;
	typedef unsigned __int64   uint64_t;

#	define INT64_C(x)    x##I64
#	define UINT64_C(x)   x##UI64

#	define INT64_MIN   _I64_MIN
#	define INT64_MAX   _I64_MAX
#	define UINT64_MAX  _UI64_MAX

#endif

#ifdef OS_VXWORKS

#	include <limits.h>

#	define INT64_C(x)    x##LL
#	define UINT64_C(x)   x##ULL

#   define INT64_MIN LONG_LONG_MIN
#   define INT64_MAX LONG_LONG_MAX
#   define UINT64_MAX ULONG_LONG_MAX
	typedef int intptr_t;
	typedef unsigned int uintptr_t;

#endif


#if defined(OS_GNUC) || defined(OS_DIAB)

#	include <limits.h>

	typedef signed long long     int64_t;
	typedef unsigned long long   uint64_t;

#	define INT64_C(x)    x##LL
#	define UINT64_C(x)   x##ULL

#	define INT64_MIN     LLONG_MIN
#	define INT64_MAX     LLONG_MAX
#	define UINT64_MAX    ULLONG_MAX

#endif

#ifdef OS_WIN32

#include <basetsd.h>

	typedef INT_PTR intptr_t;
	typedef UINT_PTR uintptr_t;
#endif

#endif /* HAVE_STDINT_H */

/* cleanup environment */
#if defined(INT64_MAX) || defined(_I64_MIN) || defined(LONG_LONG_MIN)
#	define HAVE_INT64
#endif

#if !defined(__cplusplus) && !defined(HAVE_STDBOOL_H)
typedef int bool;
#define true 1
#define false 0
#endif

/* check */
#ifdef OS_MSC
#	define HAVE_LONGLONG
	typedef __int64 		longlong_t;
	typedef unsigned __int64 	ulonglong_t;
#endif

#if defined(OS_GNUC) || defined(OS_IAR) || defined(OS_ARMCC) || defined(OS_GHS) || defined(OS_DIAB)
#	define HAVE_LONGLONG
	typedef long long		longlong_t;
	typedef unsigned long long	ulonglong_t;
#endif

/* Verify correct size for standard types. Do not instantiate. */
union x_verify_std_types_x
{
	char   int8_t_incorrect        [sizeof(  int8_t) == 1];
	char   uint8_t_incorrect       [sizeof( uint8_t) == 1];
	char   int16_t_incorrect       [sizeof( int16_t) == 2];
	char   uint16_t_incorrect      [sizeof(uint16_t) == 2];
	char   int32_t_incorrect       [sizeof( int32_t) == 4];
	char   uint32_t_incorrect      [sizeof(uint32_t) == 4];
#ifdef HAVE_INT64
	char   int64_t_incorrect       [sizeof( int64_t) == 8];
	char   uint64_t_incorrect      [sizeof(uint64_t) == 8];
#endif
};

#endif /* OS_STD_TYPES_H */
