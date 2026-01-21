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

/* @file
 *
 * This file contains internal settings used to build ITTIA DB. These settings
 * mainly describe the software environment and should not affect the user
 * application.
 *
 * OSlib provides support for the following operating systems and platforms:
 *
 * - OS_WIN32    - any Win32 based system, including NT, 9x, CE, etc.
 *   - OS_WINNT  - Windows NT target (should support windows 9x as well)
 *   - OS_WINCE  - Windows CE target
 * - OS_POSIX    - any POSIX based system, various kinds of unixes, linux, etc.
 *    OS_LINUX   - Linux target
 * - OS_VXWORKS  - VxWorks 5.x & 6.x
 * -  OS_VXWORKS_55 - VxWorks 5.5
 * -  OS_VXWORKS_6x - VxWorks 6.x (using vxWorks 6.8)
 * -  OS_VXWORKS_7x - VxWorks 7.x
 * - OS_UCOS_II  - Micrium uC/OS-II
 * - OS_UCOS_III - Micrium uC/OS-III
 *
 * Compilers:
 * - OS_MSC      - MS Visual C++
 * - OS_GNUC     - GNU C/C++
 *   - OS_MINGW  - GNU MinGW
 *   - OS_CYGWIN - GNU Cygwin
 * - OS_DIAB     - Wind River DIAB
 *
 */

#ifndef OS_CONFIG_H
#define OS_CONFIG_H

#include "ittia/os/os_env.h"

/* Allow GNU extensions */
#ifndef _GNU_SOURCE
#	define _GNU_SOURCE
#endif
/* request largefile support on Linux */
#ifndef _LARGEFILE64_SOURCE
#	define _LARGEFILE64_SOURCE
#endif

/* Make educated guesses about various environment settings. */

#ifdef OS_MSC
#	if !defined(_MT) && !defined(DB_DISABLE_THREADS)
#		define DB_DISABLE_THREADS
#	endif
#
#	ifdef _CPPRTTI
#		define HAVE_RTTI 1
#	endif
#
#	ifdef _CPPUNWIND
#		define HAVE_EXCEPTIONS 1
#	endif
#
#   ifdef _UNICODE
#       define DB_UNICODE
#   endif
#
#endif

#ifdef OS_MINGW
#
#	define HAVE_EXCEPTIONS 1
#
#endif

#ifdef OS_DIAB
#
#	ifdef __RTTI
#		define HAVE_RTTI 1
#	endif
#
#	ifdef __EXCEPTIONS
#		define HAVE_EXCEPTIONS 1
#	endif
#
#   ifdef __wchar_t
#       define DB_UNICODE
#   endif
#
#endif

#if defined(_POSIX_DEBUG)
#
#	define HAVE_GENERIC_ATOMIC
#	define HAVE_POSIX_THREADS
#	define HAVE_POSIX_MUTEX
#   define HAVE_POSIX_SEM
#   define HAVE_POSIX_FASTLOCK
#   define HAVE_POSIX_BARRIER
#   define HAVE_POSIX_TLS
#   define HAVE_POSIX_RWLOCK
#   define HAVE_POSIX_CONDVAR
#	define HAVE_POSIX_TIME
#	define HAVE_POSIX_FILE
#	define HAVE_POSIX_ERROR
#   define HAVE_POSIX_SOCKET
#
#	ifndef DB_DEBUG
#		error "debug code"
#	endif
#   define HAVE_PTHREAD_H
#   define HAVE_SEMAPHORE_H
#   define HAVE_SYS_TIMEB_H
#   define HAVE__FTIME
#
#elif defined(OS_MAPUSOFT)
#
#   define HAVE_POSIX_FILE
#   define HAVE_POSIX_ERROR
#   define HAVE_POSIX_THREADS
// CrossOS mutexes do not pass PlatformLayer tests.
#   define HAVE_GENERIC_MUTEX
#   define HAVE_POSIX_SEM
#   define HAVE_POSIX_TLS
// POSIX condition variables are not compatible with generic mutexes.
#   define HAVE_GENERIC_CONDVAR
#   define HAVE_POSIX_TIME
#   define HAVE_GENERIC_ATOMIC
#   undef  HAVE_POSIX_SOCKET
#   ifdef HAVE_PTHREAD_SPIN_LOCK
#       define HAVE_POSIX_FASTLOCK
#   else
#       define HAVE_GENERIC_FASTLOCK
#   endif
// CrossOS POSIX barriers do not pass PlatformLayer tests.
#   define HAVE_GENERIC_BARRIER
// CrossOS POSIX rwlocks do not pass PlatformLayer tests.
#   define HAVE_GENERIC_RWLOCK
#   ifdef HAVE_SHM_OPEN
#       define HAVE_POSIX_SHM
#   endif
#
#   ifdef OS_VXWORKS
#       define HAVE_POSIX_SOCKET
#   endif
#
#	define DBXX_USE_BUILTIN_MALLOC
#
#elif defined(OS_UCOS_II)
#
#   define HAVE_UCOS_II_THREADS
#   if defined(OS_TLS_TBL_SIZE) && (OS_TLS_TBL_SIZE > 0u)
#       define HAVE_UCOS_II_TLS
#   else
#       define HAVE_GENERIC_TLS
#   endif
#   define HAVE_UCOS_II_TIME
#   define HAVE_GENERIC_ATOMIC
#   define HAVE_GENERIC_BARRIER
#   define HAVE_GENERIC_CONDVAR
#   define HAVE_GENERIC_FASTLOCK
#   define HAVE_GENERIC_RWLOCK
#
#elif defined(OS_UCOS_III)
#
#   define HAVE_UCOS_III_THREADS
#   if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
#       define HAVE_UCOS_III_TLS
#   else
#       define HAVE_GENERIC_TLS
#   endif
#   define HAVE_UCOS_III_TIME
#   define HAVE_GENERIC_ATOMIC
#   define HAVE_GENERIC_BARRIER
#   define HAVE_GENERIC_CONDVAR
#   define HAVE_GENERIC_FASTLOCK
#   define HAVE_GENERIC_RWLOCK
#
#elif defined(OS_FREERTOS)
#
#   define HAVE_FREERTOS_THREADS
#   define HAVE_GENERIC_TLS
#   define HAVE_GENERIC_RWLOCK
#   define HAVE_GENERIC_BARRIER
#   define HAVE_GENERIC_CONDVAR
#   define HAVE_GENERIC_FASTLOCK
#ifdef OS_WIN32
#   define HAVE_WIN32_ATOMIC
#else
#   define HAVE_GENERIC_ATOMIC
#   define GENERIC_ATOMIC_USE_MUTEX
#endif
#
#elif defined(OS_THREADX)
#
#   define HAVE_THREADX_THREADS
#   define HAVE_GENERIC_TLS
#   define HAVE_GENERIC_RWLOCK
#   define HAVE_GENERIC_BARRIER
#   define HAVE_GENERIC_CONDVAR
#   define HAVE_GENERIC_FASTLOCK
#ifdef OS_WIN32
#   define HAVE_WIN32_ATOMIC
#else
#   define HAVE_GENERIC_ATOMIC
#   define GENERIC_ATOMIC_USE_MUTEX
#endif
#
#   ifdef OS_FILEX
#       define HAVE_FILEX_FILE
#   endif
#
#elif defined(OS_WIN32)
#
#	define HAVE_WIN32_FILE
#	define HAVE_WIN32_THREADS
#	define HAVE_WIN32_ATOMIC
#	define HAVE_WIN32_TIME
#	define HAVE_WIN32_SOCKET
#	define HAVE_WIN32_SHM
#	define HAVE_GENERIC_BARRIER
#	define HAVE_GENERIC_CONDVAR
#   define HAVE_GENERIC_RWLOCK
#
#elif defined(OS_LINUX)
#
#	define HAVE_POSIX_FILE
#	define HAVE_POSIX_ERROR
#	define HAVE_POSIX_THREADS
#   define HAVE_POSIX_MUTEX
#   define HAVE_POSIX_SEM
#   define HAVE_POSIX_TLS
#   define HAVE_POSIX_CONDVAR
#	define HAVE_POSIX_TIME
#	define HAVE_GENERIC_ATOMIC
#   define HAVE_POSIX_SOCKET
#   ifdef HAVE_PTHREAD_SPIN_LOCK
#       define HAVE_POSIX_FASTLOCK
#   else
#       define HAVE_GENERIC_FASTLOCK
#   endif
#   ifdef HAVE_PTHREAD_BARRIER_WAIT
#       define HAVE_POSIX_BARRIER
#   else
#       define HAVE_GENERIC_BARRIER
#   endif
#   ifdef HAVE_PTHREAD_RWLOCK_WRLOCK
#       define HAVE_POSIX_RWLOCK
#   else
#       define HAVE_GENERIC_RWLOCK
#   endif
#   ifdef HAVE_SHM_OPEN
#       define HAVE_POSIX_SHM
#   endif
#
#elif defined(OS_INTEGRITY)
#
#   define HAVE_POSIX_TIME
#   define HAVE_POSIX_FILE
#
#   define HAVE_GENERIC_ATOMIC
#
#   define HAVE_POSIX_THREADS
#   define HAVE_POSIX_MUTEX
#   define HAVE_POSIX_SEM
#   define HAVE_POSIX_TLS
#   define HAVE_POSIX_CONDVAR
#   define HAVE_GENERIC_FASTLOCK
#   define HAVE_GENERIC_BARRIER
#   define HAVE_POSIX_RWLOCK
#
#   define HAVE_POSIX_SOCKET
#
#elif defined(OS_VXWORKS)
#
#	define HAVE_POSIX_FILE
#	define HAVE_POSIX_ERROR
#	define HAVE_POSIX_THREADS
#   ifdef OS_VXWORKS_7x
#       define HAVE_GENERIC_TLS
#   else
#       define HAVE_POSIX_TLS
#   endif
#   define HAVE_POSIX_TIME
#   define HAVE_VXWORKS_ATOMIC
#   define HAVE_GENERIC_BARRIER
#   define HAVE_VXWORKS_RWLOCK
#   define HAVE_GENERIC_FASTLOCK

#   ifdef OS_VXWORKS_55
#       define HAVE_VXWORKS_MUTEX
#       define HAVE_VXWORKS_SEM
#       define HAVE_VXWORKS_CONDVAR
#   else
#       define HAVE_POSIX_MUTEX
#       define HAVE_POSIX_SOCKET
#       define HAVE_POSIX_SEM
#       define HAVE_POSIX_CONDVAR
#       ifdef OS_VXWORKS_RTP
#           define HAVE_POSIX_SHM
#       endif
#   endif

/* #	define HAVE_GENERIC_ATOMIC */
/* #	define HAVE_GENERIC_RWLOCK */
/* #	define HAVE_GENERIC_BARRIER */
/* #	define HAVE_GENERIC_CONDVAR */

/*#define GENERIC_ATOMIC_USE_MUTEX*/

#   if defined(__GNUC__) && !defined(HAVE_ALLOCA)
        /* diab have an alloca() definition in dcc.h */
#       define	alloca		__builtin_alloca
#       define HAVE_ALLOCA
#   endif

/*#	define HAVE_STDINT_H*/
#elif defined(OS_QNX)
#
#	define HAVE_POSIX_FILE
#	define HAVE_POSIX_ERROR
#	define HAVE_POSIX_THREADS
#   define HAVE_POSIX_TLS
#   define HAVE_POSIX_MUTEX
#   define HAVE_POSIX_FASTLOCK
#   define HAVE_POSIX_BARRIER
#   define HAVE_POSIX_RWLOCK
#   define HAVE_POSIX_CONDVAR
#   define HAVE_POSIX_SEM
#	define HAVE_POSIX_TIME
#   define HAVE_POSIX_SOCKET
#	define HAVE_GENERIC_ATOMIC
#	define GENERIC_ATOMIC_USE_FASTLOCK
#   ifdef HAVE_SHM_OPEN
#       define HAVE_POSIX_SHM
#   endif
#

#elif defined(OS_OTHER)
#
#   define HAVE_GENERIC_THREADS
#   define HAVE_GENERIC_MUTEX
#   define HAVE_GENERIC_SEM
#   define HAVE_GENERIC_TLS
#   define HAVE_GENERIC_RWLOCK
#   define HAVE_GENERIC_BARRIER
#   define HAVE_GENERIC_CONDVAR
#   define HAVE_GENERIC_FASTLOCK
#   define HAVE_GENERIC_ATOMIC
#   define GENERIC_ATOMIC_USE_MUTEX
#
#elif !defined(OS_OTHER)
#
#error unknown OS
#
#endif

#if !defined(DB_DISABLE_THREADS) \
    && (defined(HAVE_WIN32_THREADS) \
        || defined(HAVE_POSIX_THREADS) \
        || defined(HAVE_FREERTOS_THREADS) \
        || defined(HAVE_THREADX_THREADS) \
        || defined(HAVE_UCOS_II_THREADS) \
        || defined(HAVE_UCOS_III_THREADS))
#	define HAVE_THREADS
#endif

#if defined(DB_DISABLE_THREADS) && defined(HAVE_THREADS)
#	error "threads misconfigured"
#endif

/* Processor architecture settings */
/* GHS defines __ARM. */
#if defined(ARM) || defined(__ARM) || defined(__ARM__) || defined(__arm__) \
	|| (defined(__IAR_SYSTEMS_ICC__) && defined(__ICCARM__) && __ICCARM__ == 1) \
	|| defined(__ADSPBLACKFIN__)

/* GHS defines __ghs_alignment as the number of bytes in the most aligned type. */

/** Defined when a pointer must be aligned to the size of its type */
#   define DB_POINTER_ALIGNMENT
#endif

/* Miscellaneous settings */
#ifdef _MSC_VER
#if 0
/* debugging */
#   pragma warning(push, 4)
#   pragma warning(disable: 4127) // conditional expression is constant
#   pragma warning(disable: 4512) // assignment operator could not be generated
#   pragma warning(disable: 4511) // copy constructor could not be generated
#   pragma warning(disable: 4097) // typedef-name used as synonym for class-name
#endif
#	/* Turn off some annoying compiler warnings. */
#	pragma warning(disable: 4514) // 'function': unreferenced inline function has been removed
#	pragma warning(disable: 4355) // 'this': used in base member initializer list
#	pragma warning(disable: 4250) // 'class1': inherits 'class2::member' via dominance
#	pragma warning(disable: 4786) // 'identifier': identifier was truncated to 'number' characters in the debug information
#   pragma warning(disable: 4996) // 'function': was declared deprecated
#   pragma warning(disable: 4351) // new behavior: elements of array 'identifier' will be default initialized
#
#ifdef DB_DEBUG
#	pragma warning(1: 4100) // enable checking for unused parameters
#   pragma warning(1: 4239) // nonstandard extension used : 'token' : conversion from 'type' to 'type'
#   pragma warning(1: 4189) // local variable is initialized but not referenced
#   pragma warning(1: 4061) // enumerate 'identifier' in switch of enum 'identifier' is not explicitly handled by a case label
#endif
///#   pragma warning(1: 4514) // unreferenced inline/local function has been removed
//#   pragma warning(1: 4710) // 'function' : function not inlined
//#   pragma warning(1: 4711) // function 'function' selected for inline expansion
//#   pragma warning(1: 4714) // 'function' : function marked as __forceinline not inlined
#
#endif

#if !defined(_DB_NOVTABLE) && defined(OS_MSC)
#define _DB_NOVTABLE __declspec(novtable)
#endif

#if !defined(_DB_NOVTABLE)
#define _DB_NOVTABLE
#endif

#ifndef db_interface_decl
#	define db_interface_decl struct
#endif

#ifndef db_interface
/** The keyword @c db_interface is used to declare an abstract class which
 * contains only pure virtual functions.
 */
#	define db_interface db_interface_decl _DB_NOVTABLE
#endif

#if defined(__cplusplus)
	/* ignore the C rules with C++ compiler */
#	undef C_INLINE_SPECIFIER
#	define C_INLINE_SPECIFIER		inline
#elif defined(DISABLE_C_INLINES)
	/* disable inlines in C completly */
#	undef  C_INLINE_SPECIFIER
#	define C_INLINE_SPECIFIER		/**/
#elif defined(C_INLINE_SPECIFIER)
	/* user-defined configuration */
#elif defined(OS_MSC) || defined(OS_ARMCC)
#	define C_INLINE_SPECIFIER		__inline
#elif defined(OS_GNUC)
/* The __inline__ keyword works despite -ansi */
#	define C_INLINE_SPECIFIER		__inline__
#elif defined(OS_IAR) || defined (OS_ANALOG_DEVICES) || defined(OS_GHS)
#	define C_INLINE_SPECIFIER		inline
#elif defined(OS_DIAB)
#	define C_INLINE_SPECIFIER		inline
#else
#	error "Check if your compiler supports inline and define C_INLINE_SPECIFIER or DISABLE_C_INLINES"
#endif

#ifdef __cplusplus
#
#	define C_HEADER_BEGIN extern "C" {
#	define C_HEADER_END   }
#	define C_LINKAGE      extern "C"
#	define C_INLINE_DECL  C_INLINE_SPECIFIER
#
#else
#
#	define C_HEADER_BEGIN
#	define C_HEADER_END
#	define C_LINKAGE
#	define C_INLINE_DECL  static C_INLINE_SPECIFIER /* target C99 static inline with failing back to plain static functions */
#
#endif


#if defined(OS_MSC) && (!defined(_MANAGED) || _MANAGED != 1)
#
#	define DB_FASTCALL __fastcall
#	define DB_CDECL    __cdecl
#
#elif defined(OS_GNUC) && __GNUC__ > 0x0300
#
#	define DB_FASTCALL __attribute__((__fastcall__))
#	define DB_CDECL    __attribute__((__cdecl__))
#
#elif defined(OS_GNUC)
#
#	define DB_FASTCALL  /* __attribute__((__stdcall__)) */
#	define DB_CDECL     /* __attribute__((__cdecl__)) */
#
#else
#
#	define DB_FASTCALL  /*NOTHING*/
#	define DB_CDECL     /*NOTHING*/
#
#endif

#if defined(OS_WIN32) && defined(OS_MSC)
#
#	define DB_EXPORT __declspec(dllexport)
#	define DB_IMPORT __declspec(dllimport)
#
#elif defined(OS_WIN32) && defined(__GNUC__)
#
#	define DB_EXPORT __declspec(dllexport)
#	define DB_IMPORT __declspec(dllimport)
#
#else
#	define DB_EXPORT /*NOTHING*/
#	define DB_IMPORT /*NOTHING*/
#endif

#if defined(DBDLL) /* database compiled as shared library */
#
#	ifdef DBDLL_EXPORTS
#
#		define DBDLL_API DB_EXPORT
#
#	else /* DBDLL_EXPORTS */
#
#		define DBDLL_API DB_IMPORT
#
#	endif /* DBDLL_EXPORTS */
#
#else /* static library */
#
#	define DBDLL_API
#
#endif /* defined(DBDLL) */

#define DB_CALLBACK DB_FASTCALL
#define DB_DECL     DB_FASTCALL

/* Compiler data types, */
#include "ittia/os/std/types.h"

#define UINT16_HIGH(x)   ((uint8_t)((uint16_t)(x) >> 8))
#define UINT16_LOW(x)    ((uint8_t)((uint16_t)(x)))
#define UINT16_MAKE(h,l) (((uint16_t)(uint8_t)(h) << 8) | (uint16_t)(uint8_t)(l))

#define UINT32_HIGH(x)   ((int16_t)((uint32_t)(x) >> 16))
#define UINT32_LOW(x)    ((int16_t)((uint32_t)(x)))
#define UINT32_MAKE(h,l) (((uint32_t)(uint16_t)(h) << 16) | (uint32_t)(uint16_t)(l))

typedef float float32_t;
typedef double float64_t;

#ifdef HAVE_INT64
typedef int64_t db_AlignType;
#else
typedef double db_AlignType;
#endif

#ifdef UINT64_MAX
#define UINT64_HIGH(x)   ((int32_t)((uint64_t)(x) >> 32))
#define UINT64_LOW(x)    ((int32_t)((uint64_t)(x)))
#define UINT64_MAKE(h,l) (((uint64_t)(uint32_t)(h) << 32) | (uint64_t)(uint32_t)(l))
#endif

#ifdef OS_MSC
#	define DB_UINT2DBL(x)   ((double)(int64_t)(x))
#else
#	define DB_UINT2DBL(x)   (x)
#endif

#ifdef DB_DEBUG
#	define DB_TRACE
#endif

#ifdef DB_UNICODE
#	include <wchar.h>
#endif

typedef char db_fname_t;
#define DB_FNAME_C(x) x

/** An address that can be used in static casts for size and offset calculations. */
#ifdef OS_IAR
#define DB_STATIC_CAST_ADDRESS 0
#else
#define DB_STATIC_CAST_ADDRESS 1
#endif

#if defined(OS_IAR) || defined(OS_ARMCC)
#define DB_STATIC_FOR_TEMPLATE
#else
#define DB_STATIC_FOR_TEMPLATE static
#endif

/* define standard types */
#include <stddef.h>

typedef size_t db_size_t;
#ifdef SSIZE_MAX
#define DB_SSIZE_MAX SSIZE_MAX
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#ifdef OS_LINUX
#include <linux/limits.h>
#endif
#endif

#if defined(OS_WIN32) || defined(HAVE_LSEEK64) || defined(HAVE__LSEEK64)
#	define HAVE_LARGE_FILE
#endif

#if defined(OS_WIN32)

#include <basetsd.h>
typedef int64_t db_off_t;
typedef int32_t db_ssize_t;
#ifndef DB_SSIZE_MAX
#define DB_SSIZE_MAX INT32_MAX
#endif

#elif defined(OS_POSIX)

/** TODO: sys/types.h is needed for off_t, but should have already been
 * included from a different file. */
#include <sys/types.h>
#ifdef HAVE_LARGE_FILE
typedef off64_t   db_off_t;
#else
/* 32-bit file */
typedef off_t   db_off_t;
#endif

typedef ssize_t db_ssize_t;
#ifndef DB_SSIZE_MAX
#define DB_SSIZE_MAX LONG_MAX
#endif

#elif defined(OS_VXWORKS)

#include <sys/types.h>
typedef off_t   db_off_t;
typedef ssize_t db_ssize_t;

#else

typedef int64_t db_off_t;
typedef int32_t db_ssize_t;
#ifndef DB_SSIZE_MAX
#define DB_SSIZE_MAX INT32_MAX
#endif

#endif

/* Type used for counting */
#if defined(OS_WIN32) || (defined(OS_VXWORKS) && defined(__LP64__))
typedef long db_counter_t;
#else
typedef int db_counter_t;
#endif

/* MAX PATH LENGTH. OS_MAX_PATH is defined in os_file.h. */
#if !defined(OS_MAX_PATH)
#ifdef OS_WIN32
/* MAX_PATH is defined in windows.h. */
#define OS_MAX_PATH_ MAX_PATH
#elif defined(PATH_MAX)
/* PATH_MAX is defined in limits.h. */
#define OS_MAX_PATH_ PATH_MAX
#elif defined(OS_INTEGRITY)
#define OS_MAX_PATH_ (PJFS_PATH_MAX > MAXPATHLEN ? PJFS_PATH_MAX : MAXPATHLEN)
#elif defined(HAVE_WIN32_FILE) || defined(HAVE_POSIX_FILE)
/* Either MAX_PATH or PATH_MAX should be defined on these platforms. */
#error OS_MAX_PATH is not defined
#else
/* Use a small default path length for other file systems. */
#define OS_MAX_PATH_ 128
#endif
#endif

typedef uint8_t db_bool_t;
#define DB_FALSE    ((db_bool_t)0)
#define DB_TRUE     ((db_bool_t)1)

#endif /* OS_CONFIG_H */
