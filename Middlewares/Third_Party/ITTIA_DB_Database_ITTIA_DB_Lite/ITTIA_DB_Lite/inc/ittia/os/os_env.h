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
 * Define characteristics of the environment, including operating system,
 * available headers, and processor architecture.
 */

#ifndef OS_ENV_H
#define OS_ENV_H

/* Make educated guesses about the characteristics of the build environment. */

#include "ittia_conf.h"

#ifdef HAVE_CONFIG_H
#include "ittia/os/auto-config.h"
#endif

#ifdef OS_OTHER

/* For a custom platform, list available headers and standard functions here,
 * or in the compiler flags. */

#define HAVE_TOUPPER
#define HAVE_TOLOWER
#define HAVE_STRLEN
#define HAVE_STRCMP
#define HAVE_STRNCMP
#define HAVE_STRNCPY

#else

/* Determine OS or platform */

#ifndef OS_WIN32
#	if defined(WIN32) \
        || defined(_WIN32) \
        || defined(__WIN32__) \
        || defined(_WIN32_WCE)
#		define OS_WIN32 1
#	endif
#endif

#ifndef OS_POSIX
#	if defined(unix)
#		define OS_POSIX 1
#	endif
#endif

/* === Distinguish target operating system. === */

#ifndef OS_VXWORKS
#	if defined(OS_VXWORKS_55) \
    || defined(OS_VXWORKS_6x) \
    || defined(OS_VXWORKS_7x) \
    || defined(__VXWORKS__) \
    || defined(__vxworks)
    /* guess target vxWorks version and properties */
#       include <version.h>
#       define OS_VXWORKS 1
#	    define OS_POSIX 1
#       define OS_VXWORKS_VERSION (_WRS_VXWORKS_MAJOR * 100 +  _WRS_VXWORKS_MINOR)
#
#       undef OS_VXWORKS_55
#       undef OS_VXWORKS_6x
#       undef OS_VXWORKS_7x
#       ifdef _WRS_KERNEL
#           define OS_VXWORKS_KERNEL
#       elif defined(__RTP__)
#           define OS_VXWORKS_RTP
#       else
#           error "Unknown vxWorks compilation configuration (user/rtp)"
#       endif
#
#       if (_WRS_VXWORKS_MAJOR) == 5 && (_WRS_VXWORKS_MINOR == 5)
#           define OS_VXWORKS_55    _WRS_VXWORKS_MINOR
#       elif (_WRS_VXWORKS_MAJOR) == 6 && (_WRS_VXWORKS_MINOR >= 6) && (_WRS_VXWORKS_MINOR <= 9)
#           define OS_VXWORKS_6x    _WRS_VXWORKS_MINOR
#       elif (_WRS_VXWORKS_MAJOR) == 7
#           define OS_VXWORKS_7x    _WRS_VXWORKS_MINOR
#       else
#           error "untested vxWorks version"
#	    endif
#   endif
#endif

#ifdef OS_WIN32
#	ifdef _WIN32_WCE
#		define OS_WINCE 1
#	else
#		define OS_WINNT 1
#	endif
#endif

#ifndef OS_LINUX
#	if defined(linux) || defined(__linux) || defined(__linux__)
#		define OS_POSIX 1
#		define OS_LINUX 1
#	endif
#endif

#ifndef OS_ANDROID
#   if defined(ANDROID)
#       define OS_ANDROID 1
#   endif
#endif

#ifndef OS_QNX
#	if defined(__QNX__) && defined(__QNXNTO__)
#		define OS_POSIX 1
#		define OS_QNX 1
#	endif
#endif

#ifndef OS_WICED
#   if defined(WICED_VERSION)
#       define OS_WICED 1
#   endif
#endif

#ifndef OS_THREADX
#   if defined(WICED_VERSION) && defined(RTOS_ThreadX)
#       define OS_THREADX 1
#   endif
#endif

#ifndef OS_FILEX
#   if defined(WICED_VERSION) && defined(USING_FILEX)
#       define OS_FILEX 1
#   endif
#endif

#if !defined(OS_POSIX) \
    && (defined(OS_LINUX) \
       || defined(OS_INTEGRITY) \
       || defined(OS_QNX) \
       || defined(OS_VXWORKS))
#   define OS_POSIX 1
#endif /* OS_POSIX */

#endif

#ifndef OS_UCOS_II
/* There is no canonical define for uC/OS-II. Instead, define OS_UCOS_II
 * in CPPFLAGS or equivalent. */
#endif

/* Detect which compiler is being used. */

#ifdef _MSC_VER
#	define OS_MSC 1
#endif

#ifdef __GNUC__
#	define OS_GNUC 1
#endif

#ifdef __MINGW32__
#	define OS_MINGW 1
#endif

#ifdef __CYGWIN__
#	define OS_CYGWIN 1
#endif

#ifdef __ghs__
#	define OS_GHS 1
#endif

#if defined(_DIAB_TOOL) || (defined(_VX_TOOL) && _VX_TOOL == diab) || defined(__DCC__)
#	define OS_DIAB 1
#endif

#ifdef __IAR_SYSTEMS_ICC__
#	define OS_IAR 1
#endif

#ifdef __ARMCC_VERSION
#	define OS_ARMCC 1
#endif

#ifdef _ADI_COMPILER
#   define OS_ANALOG_DEVICES 1
#endif

#if defined(__CROSSWORKS) || defined(__CROSSWORKS_ARM) || defined(__CROSSWORKS_AVR) || defined(__CROSSWORKS_MAXQ)
#	define OS_CROSSWORKS
#endif

#ifndef HAVE_CONFIG_H

/* no config file, perform static configuration */

/* MapuSoft POSIX interface functions are not detected by configure because
 * their actual names have a "PX_" prefix. */
#ifdef OS_MAPUSOFT
#
#   define HAVE_USLEEP         1
#   define HAVE_NANOSLEEP      1
#   define HAVE_CLOCK_GETTIME  1
#   define HAVE_MMAP           1
#   define HAVE_SLEEP          1
#   define HAVE_SYSCONF        1
#   define HAVE_WAITPID        1
#   define HAVE_SCHED_YIELD    1

#   define HAVE_FLOOR 1
#   define HAVE_FTIME 1
#   define HAVE_FTRUNCATE 1
#   define HAVE_GETPAGESIZE 1
#   define HAVE_GETPID 1
#   define HAVE_GETPPID 1
#   undef  HAVE_GETTIMEOFDAY
#   undef  HAVE_LSEEK64
#   define HAVE_MBTOWC 1
#   define HAVE_MEMMOVE 1
#   define HAVE_MEMSET 1
#   define HAVE_OPEN 1
#   define HAVE_PTHREAD_BARRIER_WAIT 1
#   define HAVE_PTHREAD_RWLOCK_WRLOCK 1
#   define HAVE_PTHREAD_SPIN_LOCK 1
#   define HAVE_SCHED_YIELD 1
#   define HAVE_STRCASECMP 1
#   define HAVE_STRCMP 1
#   undef  HAVE_STRICMP
#   define HAVE_STRLEN 1
#   define HAVE_STRNCASECMP 1
#   define HAVE_STRNCMP 1
#   define HAVE_STRNCPY 1
#   undef  HAVE_STRNICMP
#   define HAVE_TOLOWER 1
#   define HAVE_TOUPPER 1
#   define HAVE_TOWLOWER 1
#   define HAVE_TOWUPPER 1
#   define HAVE_USLEEP 1
#   define HAVE_VPRINTF 1
#   define HAVE_WCSCMP 1
#   undef  HAVE_WCSICMP
#   define HAVE_WCSLEN 1
#   define HAVE_WCSNCMP 1
#   define HAVE_WCSNCPY 1
#   undef  HAVE_WCSNICMP
#   define HAVE_WCTOMB 1
#
#elif defined(OS_WINNT)
#
#	if defined(OS_MSC) || defined(OS_MINGW)
#
#		define HAVE_FLOAT_H
#		define HAVE_LIMITS_H
#		define HAVE_MALLOC_H
#		define HAVE_MATH_H
#		define HAVE_FLOAT_H
#		define HAVE_MEMORY_H
#		define HAVE_STDBOOL_H
#		define HAVE_STDLIB_H
#		define HAVE_STRING_H
#		define HAVE_SYS_STAT_H
#		define HAVE_SYS_TYPES_H
#		define HAVE_ERRNO_H
#		define HAVE_WCHAR_H
#		define HAVE_STDIO_H
#		define HAVE_WINDOWS_H
#		define HAVE_WCTYPE_H
#		define HAVE_WCHAR_H
#		define HAVE_IO_H
#		define HAVE_FCNTL_H
#		define HAVE_SYS_LOCKING_H
#		define HAVE_SYS_STAT_H
#		define HAVE_CTYPE_H
#
#		define HAVE_SQLEXT_H
#		define HAVE_SQLTYPES_H
#		define HAVE_SQL_H
#		define HAVE_ODBCINST_H
#
#		define HAVE_LOCKFILE
#		//define HAVE_CHSIZE
#		define HAVE_FLOOR
#		define HAVE_MALLOC
#		define HAVE_REALLOC
#		define HAVE_ALLOCA
#		define HAVE_MEMMOVE
#		define HAVE_MEMSET
#		define HAVE_VPRINTF
#
#		define HAVE_TOUPPER
#		define HAVE_TOLOWER
#		define HAVE_STRLEN
#		define HAVE_STRNCPY
#		define HAVE_STRICMP
#		define HAVE_STRNCMP
#		define HAVE_STRNICMP
#		define HAVE_STRCMP
#
#		define HAVE_TOWUPPER
#		define HAVE_TOWLOWER
#		define HAVE_WCSLEN
#		define HAVE_WCSCMP
#		define HAVE_WCSNCPY
#		define HAVE_WCSNCMP
#		define HAVE_WCSICMP
#		define HAVE_WCSNICMP
#		define HAVE_WCTOMB
#		define HAVE_MBTOWC
#
#		define HAVE__LOCKING
#		define HAVE__LSEEKI64
#		define HAVE_CHMOD
#
#		define HAVE_LOCALTIME
#
#		define HOST_BYTE_ORDER  LITTLE_ENDIAN
#
#       ifdef OS_MINGW
#		    define HAVE_SYS_TIME_H
#       endif
#	endif /* MSC || MINGW */
#
#	ifdef OS_MSC /* MSVC WIN32 specific settings */
#
#		define STDC_HEADERS
#
#		ifdef _MT
#			define HAVE_BEGIN_THREAD
#		endif
#
#	if _MSC_VER >= 1700
#		define HAVE_STDINT_H
#	endif
#
#	endif /* _MSC_VER */
#
#	ifdef OS_MINGW
#
#       define HAVE_STDINT_H
#       define HAVE_BEGIN_THREAD
#
#	endif /* __MINGW32__ */
#
#elif defined(OS_WINCE)
#
#	/* Windows CE platform settings. MSVC. */
#
#	define HAVE_FLOAT_H
#	define HAVE_LIMITS_H
#	define HAVE_MALLOC_H
#	define HAVE_MATH_H
#   define HAVE_FLOAT_H
#	define HAVE_MEMORY_H
#	define HAVE_STDLIB_H
#	define HAVE_STRING_H
#	define HAVE_ERRNO_H
#	define HAVE_WCHAR_H
#	define HAVE_STDIO_H
#	define HAVE_WINDOWS_H
#	define HAVE_WCTYPE_H
#	define HAVE_WCHAR_H
#
#	define HAVE_FLOOR
#	define HAVE_MALLOC
#	define HAVE_REALLOC
#	define HAVE_ALLOCA
#	define HAVE_MEMMOVE
#	define HAVE_MEMSET
#	define HAVE_VPRINTF
#
#	define HAVE_TOUPPER
#	define HAVE_TOLOWER
#	define HAVE_STRLEN
#	define HAVE_STRNCPY
#	define HAVE_STRICMP
#	define HAVE_STRNCMP
#	define HAVE_STRNICMP
#	define HAVE_STRCMP
#
#	define HAVE_TOWUPPER
#	define HAVE_TOWLOWER
#	define HAVE_WCSLEN
#	define HAVE_WCSCMP
#	define HAVE_WCSNCPY
#	define HAVE_WCSNCMP
#	define HAVE_WCSICMP
#	define HAVE_WCSNICMP
#
#	define HAVE__LOCKING
#	define HAVE__LSEEKI64
#
#/* Some WinCE hacks */
#	define stricmp _stricmp
#	define strnicmp _strnicmp
#
#	define O_RDONLY GENERIC_READ
#	define O_WRONLY GENERIC_WRITE
#	define O_RDWR   GENERIC_READ | GENERIC_WRITE
#
#	ifndef HOST_BYTE_ORDER
#		define HOST_BYTE_ORDER  LITTLE_ENDIAN
#	endif /* HOST_BYTE_ORDER */
#
#	/* DB_FILL_FILE_GAPS : OS doesn't support gapped file write? */
#	define DB_FILL_FILE_GAPS
#
#elif defined(OS_ANDROID)
#
#   define HAVE_ALLOCA_H
#   define HAVE_CTYPE_H
#   define HAVE_DIRENT_H
#   define HAVE_DLFCN_H
#   define HAVE_ENDIAN_H
#   define HAVE_ERRNO_H
#   define HAVE_FCNTL_H
#   define HAVE_FLOAT_H
#   define HAVE_FNMATCH_H
#   define HAVE_INTTYPES_H
#   define HAVE_LIMITS_H
#   define HAVE_MALLOC_H
#   define HAVE_MATH_H
#   define HAVE_MEMORY_H
#   define HAVE_PTHREAD_H
#   define HAVE_SCHED_H
#   define HAVE_SEMAPHORE_H
#   define HAVE_SIGNAL_H
#   define HAVE_STDBOOL_H
#   define HAVE_STDINT_H
#   define HAVE_STDIO_H
#   define HAVE_STDLIB_H
#   define HAVE_STRINGS_H
#   define HAVE_STRING_H
#   define HAVE_SYSLOG_H
#   define HAVE_SYS_MMAN_H
#   define HAVE_SYS_PARAM_H
#   define HAVE_SYS_SELECT_H
#   define HAVE_SYS_STAT_H
#   define HAVE_SYS_SYSCALL_H
#   define HAVE_SYS_TIME_H
#   define HAVE_SYS_TYPES_H
#   define HAVE_SYS_WAIT_H
#   define HAVE_TIME_H
#   define HAVE_UNISTD_H
#   define HAVE_WCHAR_H
#   define HAVE_WCTYPE_H
#
#   define HAVE_ALLOCA
#   define HAVE_CHMOD
#   define HAVE_CLOCK_GETTIME
#   define HAVE_CTIME_R
#   define HAVE_DLOPEN
#   define HAVE_FCNTL
#   define HAVE_FLOOR
#   define HAVE_FORK
#   define HAVE_FSYNC
#   define HAVE_FTIME
#   define HAVE_FTRUNCATE
#   define HAVE_GETEGID
#   define HAVE_GETEUID
#   define HAVE_GETGID
#   define HAVE_GETPGRP
#   define HAVE_GETPID
#   define HAVE_GETPPID
#   define HAVE_GETTIMEOFDAY
#   define HAVE_GETUID
#   define HAVE_LIBDL
#   define HAVE_LIBM
#   define HAVE_LOCALTIME
#   define HAVE_LSEEK64
#   define HAVE_MEMMOVE
#   define HAVE_MEMSET
#   define HAVE_NANOSLEEP
#   define HAVE_OPEN
#   define HAVE_REALPATH
#   define HAVE_SCHED_YIELD
#   define HAVE_SLEEP
#   define HAVE_STRCASECMP
#   define HAVE_STRCMP
#   define HAVE_STRLEN
#   define HAVE_STRNCASECMP
#   define HAVE_STRNCMP
#   define HAVE_STRNCPY
#   define HAVE_SYSCONF
#   define HAVE_TOLOWER
#   define HAVE_TOUPPER
#   define HAVE_TOWLOWER
#   define HAVE_TOWUPPER
#   define HAVE_USLEEP
#   define HAVE_VPRINTF
#   define HAVE_WAITPID
#   define HAVE_WCSCMP
#   define HAVE_WCSLEN
#   define HAVE_WCSNCMP
#   define HAVE_WCSNCPY
#
#elif defined(OS_LINUX)
#
#	define HAVE_STRING_H
#	define HAVE_STDLIB_H
#	define HAVE_STDBOOL_H
#	define HAVE_STDINT_H
#	define HAVE_LIMITS_H
#   define HAVE_SEMAPHORE_H
#	define HAVE_ERRNO_H
#   define HAVE_PTHREAD_H
#   define HAVE_DIRENT_H
#   define HAVE_FNMATCH_H
#   define HAVE_GLOB_H
#   define HAVE_UNISTD_H
#   define HAVE_STDIO_H
#   define HAVE_FCNTL_H
#   define HAVE_SYS_TYPES_H
#   define HAVE_SYS_STAT_H
#   define HAVE_MALLOC_H
#   define HAVE_MATH_H
#   define HAVE_FLOAT_H
#   define HAVE_MEMORY_H
#   define HAVE_SYS_PARAM_H
#   define HAVE_WCTYPE_H
#   define HAVE_WCHAR_H
#   define HAVE_TIME_H
#   define HAVE_SYS_TIME_H
#   define HAVE_SYS_TIMEB_H
#   define HAVE_SCHED_H
#   define HAVE_SYS_MMAN_H
#   define HAVE_SIGNAL_H
#   define HAVE_SYS_SYSCALL_H
#   define HAVE_CTYPE_H
#   define HAVE_DLFCN_H
#
#	define HAVE_TOUPPER
#   define HAVE_TOWUPPER
#	define HAVE_TOLOWER
#   define HAVE_TOWLOWER
#	define HAVE_STRLEN
#   define HAVE_WCSLEN
#	define HAVE_STRCMP
#   define HAVE_WCSCMP
#   define HAVE_WCSNCMP
#   define HAVE_STRCASECMP
#   define HAVE_WCSCASECMP
#   define HAVE_STRNCASECMP
#   define HAVE_WCSNCASECMP
#   define HAVE_STRNCPY
#   define HAVE_WCSNCPY
#	define HAVE_STRNCMP
#	define HAVE_STRNCPY
#   define HAVE_FTRUNCATE64
#   define HAVE_MMAP
#   define HAVE_GETPAGESIZE
#   define HAVE_CLOCK_GETTIME
#   define HAVE_LOCALTIME
#   define HAVE_SHM_OPEN
#   define HAVE_NANOSLEEP
#   define HAVE_SCHED_YIELD
#	define HAVE_MEMMOVE
#	define HAVE_MEMSET
#   define HAVE_WCTOMB
#   define HAVE_MBTOWC
#   define HAVE_GETPID
#   define HAVE_GETPGRP
#   define HAVE_GETPPID
#   define HAVE_GETUID
#   define HAVE_GETEUID
#   define HAVE_GETGID
#   define HAVE_GETEGID
#   define HAVE_DLOPEN
#
#   define HAVE_PTHREAD_BARRIER_WAIT
#   define HAVE_PTHREAD_RWLOCK_WRLOCK
#   define HAVE_PTHREAD_SPIN_LOCK
#
#elif defined(OS_QNX)
#
#	define HAVE_ALLOCA_H
#	define HAVE_ERRNO_H
#	define HAVE_FCNTL_H
#	define HAVE_FLOAT_H
#	define HAVE_INTTYPES_H
#	define HAVE_LIMITS_H
#	define HAVE_MALLOC_H
#	define HAVE_MATH_H
#   define HAVE_FLOAT_H
#	define HAVE_MEMORY_H
#	define HAVE_MMAN_H
#	define HAVE_PTHREAD_H
#	define HAVE_SCHED_H
#	define HAVE_SEMAPHORE_H
#	define HAVE_STDBOOL_H
#	define HAVE_STDINT_H
#	define HAVE_STDIO_H
#	define HAVE_STDLIB_H
#	define HAVE_STRINGS_H
#	define HAVE_STRING_H
#	define HAVE_SYS_PARAM_H
#	define HAVE_SYS_SELECT_H
#	define HAVE_SYS_SLOG2_H
#	define HAVE_SYS_STAT_H
#	define HAVE_SYS_TIME_H
#	define HAVE_SYS_TYPES_H
#	define HAVE_TIME_H
#	define HAVE_UNISTD_H
#	define HAVE_GLOB_H
#	define HAVE_WCHAR_H
#	define HAVE_WCTYPE_H
#
#	define HAVE_ALLOCA
#	define HAVE_CHMOD
#	define HAVE_CHSIZE
#	define HAVE_CLOCK_GETTIME
#	define HAVE_LOCALTIME
#	define HAVE_FCNTL
#	define HAVE_FSYNC
#	define HAVE_FTIME
#	define HAVE_FTRUNCATE
#	define HAVE_FTRUNCATE64
#	define HAVE_GETPAGESIZE
#   define HAVE_GETPID
#   define HAVE_GETPGRP
#   define HAVE_GETPPID
#   define HAVE_GETUID
#   define HAVE_GETEUID
#   define HAVE_GETGID
#   define HAVE_GETEGID
#	define HAVE_GETTIMEOFDAY
#	define HAVE_LOCKF
#	define HAVE_LSEEK64
#   define HAVE_MBTOWC
#	define HAVE_MEMMOVE
#	define HAVE_MEMSET
#	define HAVE_MMAP
#	define HAVE_NANOSLEEP
#	define HAVE_OPEN
#	define HAVE_REALPATH
#	define HAVE_SCHED_YIELD
#	define HAVE_SLEEP
#	define HAVE_STRCASECMP
#	define HAVE_STRCMP
#	define HAVE_STRICMP
#	define HAVE_STRLEN
#	define HAVE_STRNCASECMP
#	define HAVE_STRNCMP
#	define HAVE_STRNCPY
#	define HAVE_STRNICMP
#	define HAVE_TOLOWER
#	define HAVE_TOUPPER
#	define HAVE_TOWLOWER
#	define HAVE_TOWUPPER
#	define HAVE_USLEEP
#	define HAVE_VPRINTF
#	define HAVE_WCSCMP
#	define HAVE_WCSLEN
#	define HAVE_WCSNCMP
#	define HAVE_WCSNCPY
#   define HAVE_WCTOMB
#
#   define HAVE_PTHREAD_BARRIER_WAIT
#   define HAVE_PTHREAD_RWLOCK_WRLOCK
#   define HAVE_PTHREAD_SPIN_LOCK
#
#elif defined(OS_INTEGRITY)
#
#   define HAVE_ALLOCA_H
#   define HAVE_CTYPE_H
#   define HAVE_DIRENT_H
#   define HAVE_DLFCN_H
#   define HAVE_ERRNO_H
#   define HAVE_FCNTL_H
#   define HAVE_FLOAT_H
#   define HAVE_FNMATCH_H
#   define HAVE_GLOB_H
#   define HAVE_INTTYPES_H
#   define HAVE_LIMITS_H
#   define HAVE_MATH_H
#   define HAVE_PTHREAD_H
#   define HAVE_SCHED_H
#   define HAVE_SEMAPHORE_H
#   define HAVE_SIGNAL_H
#   define HAVE_STDBOOL_H
#   define HAVE_STDINT_H
#   define HAVE_STDIO_H
#   define HAVE_STDLIB_H
#   define HAVE_STRINGS_H
#   define HAVE_STRING_H
#   define HAVE_SYSLOG_H
#   define HAVE_SYS_MMAN_H
#   define HAVE_SYS_SELECT_H
#   define HAVE_SYS_STAT_H
#   define HAVE_SYS_TIME_H
#   define HAVE_SYS_TYPES_H
#   define HAVE_SYS_WAIT_H
#   define HAVE_TIME_H
#   define HAVE_UNISTD_H
#   define HAVE_WCHAR_H
#   define HAVE_WCTYPE_H
#   define HAVE_FILESYSTEM_PJFS_LIMITS_H
#
#   define HAVE_CHMOD
#   define HAVE_CTIME_R
#   define HAVE_FCNTL
#   define HAVE_FLOOR
#   define HAVE_FSYNC
#   define HAVE_FTRUNCATE
#   define HAVE_GETPID
#   define HAVE_GETTIMEOFDAY
#   define HAVE_LOCALTIME
#   define HAVE_LOCKF
#   define HAVE_MBTOWC
#   define HAVE_MEMMOVE
#   define HAVE_MEMSET
#   define HAVE_OPEN
#   define HAVE_REALPATH
#   define HAVE_SLEEP
#   define HAVE_STRCASECMP
#   define HAVE_STRCMP
#   define HAVE_STRLEN
#   define HAVE_STRNCASECMP
#   define HAVE_STRNCMP
#   define HAVE_STRNCPY
#   define HAVE_TOLOWER
#   define HAVE_TOUPPER
#   define HAVE_TOWLOWER
#   define HAVE_TOWUPPER
#   define HAVE_USLEEP
#   define HAVE_VPRINTF
#   define HAVE_WCSCMP
#   define HAVE_WCSLEN
#   define HAVE_WCSNCMP
#   define HAVE_WCSNCPY
#   define HAVE_WCTOMB
#
#elif defined(OS_VXWORKS)
#   if defined(OS_VXWORKS_55)
#
#       define HAVE_ERRNO_H
#       define HAVE_POSIX_TIME
#       define HAVE_CLOCK_GETTIME
#       define HAVE_LIMITS_H
#       define HAVE_STRING_H
#       define HAVE_TOUPPER
#       define HAVE_TOLOWER
#       define HAVE_STRLEN
#       define HAVE_STRNCPY
#       define HAVE_STRNCMP
#       define HAVE_STRCMP
#       define HAVE_IOCTL_FIOSYNC

#       define HAVE_OPEN_NONEXISTENT_FILE_BUG_ON_VXWORKS_55
/* refer to "oslib/src/posix/posix_file.c" for more details */

#       define HAVE_THREAD_PRIVATE_DATA_BUG_ON_VXWORKS_55
/* refer to "oslib/src/posix/posix_tls.c" for more details */

/* #    define HAVE_STRICMP */
/* #    define HAVE_STRNICMP */
#   elif defined(OS_VXWORKS_6x) || defined(OS_VXWORKS_7x)

/* headers */
#       undef  HAVE_ALLOCA_H
#       undef  HAVE_CONFIG_H
#       define HAVE_ERRNO_H
#       define HAVE_FCNTL_H
#       define HAVE_FLOAT_H
#       define HAVE_INTTYPES_H
#       undef  HAVE_IO_H
#       define HAVE_LIMITS_H
#       undef  HAVE_MALLOC_H
#       define HAVE_MATH_H
#       define HAVE_FLOAT_H
#       undef  HAVE_MEMORY_H
#       undef  HAVE_ODBCINST_H
#       define HAVE_PTHREAD_H
#       define HAVE_SCHED_H
#       define HAVE_SEMAPHORE_H
#       define HAVE_SEMLIB_H
#       undef  HAVE_SQL_H
#       undef  HAVE_SQLEXT_H
#       undef  HAVE_SQLTYPES_H
#       define HAVE_STDBOOL_H
#       define HAVE_STDINT_H    /* user level only */
#       define HAVE_STDIO_H
#       define HAVE_STDLIB_H
#       define HAVE_STRING_H
#       define HAVE_STRINGS_H
#       undef  HAVE_SYS_LOCKING_H
#       define HAVE_SYS_MMAN_H
#       undef  HAVE_SYS_PARAM_H
#       undef  HAVE_SYS_PLATFORM_H
#       define HAVE_SYS_SELECT_H
#       define HAVE_SYS_STAT_H
#       define HAVE_SYS_TIME_H
#       undef  HAVE_SYS_TIMEB_H
#       define HAVE_SYS_TYPES_H
#       define HAVE_TIME_H
#       define HAVE_UNISTD_H
#       define HAVE_WCHAR_H
#       define HAVE_WCTYPE_H
#       undef  HAVE_WINDOWS_H
#       define HAVE_CTYPE_H 1
#       define HAVE_DIRENT_H
#       define HAVE_STAT_H


/* functions */
#       undef  HAVE_FTIME
#       undef  HAVE__FTIME
#       undef  HAVE__LOCKING
#       undef  HAVE__LSEEKI64
#       undef  HAVE_ALLOCA
#       undef  HAVE_BEGIN_THREAD
#       define HAVE_CHMOD                   1
#       undef  HAVE_CHSIZE
#       define HAVE_CLOCK_GETTIME           1
#       define  HAVE_EXCEPTIONS             1 /* unused */
#       define HAVE_FCNTL                   1
#       define HAVE_FCNTL_LOCK              1
#       define HAVE_FLOOR                   1 /* unused */
#       define HAVE_FSYNC                   1
#       define HAVE_FTRUNCATE               1
#       undef  HAVE_FTRUNCATE64
#       undef  HAVE_GETPAGESIZE           /* unused */
#       undef  HAVE_GETTIMEOFDAY              /* user code only, use clock_gettime() instead */
#       define HAVE_IOCTL_FIOSYNC           1
#       undef  HAVE_LOCKF
#       undef  HAVE_LOCKFILE                  /* unused */
#       undef  HAVE_LOCKING
#       undef  HAVE_LSEEK64
#       define HAVE_MALLOC                  1 /* unused */
#       define  HAVE_MEMMOVE                1 /* unused */
#       define  HAVE_MEMSET                 1 /* unused */
#       define HAVE_MMAP                    1
#       define HAVE_NANOSLEEP               1
#       define HAVE_OFF_T                   1 /* unused */
#       undef  HAVE_OFF64_T               /* unused, there is off_t64, used for mmap64 */
#       define HAVE_OPEN                    1 /* unused */
#       undef  HAVE_PREAD
#       undef  HAVE_PTHREAD_YIELD
#       undef  HAVE_PWRITE
#       define HAVE_REALLOC                 1
#       undef  HAVE_REALPATH
#       undef  HAVE_RTTI
#       define HAVE_SCHED_YIELD             1
#       undef  HAVE_SEM_POST_MULTIPLE
#       define HAVE_SHM_OPEN                1
#       define HAVE_SLEEP                   1
#       undef  HAVE_STRCASECMP                /* it's not clear actually */
#       define HAVE_STRCMP                  1
#       undef  HAVE_STRICMP
#       define HAVE_STRLEN                  1
#       undef  HAVE_STRNCASECMP               /* it's not clear */
#       define HAVE_STRNCMP                 1
#       define HAVE_STRNCPY                 1
#       undef  HAVE_STRNICMP
#       define HAVE_SYSCONF                 1
#       define HAVE_TOLOWER                 1
#       define HAVE_TOUPPER
#       define HAVE_TOWLOWER                1
#       define HAVE_TOWUPPER                1
#       undef  HAVE_USLEEP
#       define HAVE_VPRINTF                 1
#       define HAVE_WCSCMP                  1
#       undef  HAVE_WCSICMP
#       define HAVE_WCSLEN                  1
#       define HAVE_WCSNCMP                 1
#       define HAVE_WCSNCPY                 1
#       undef  HAVE_WCSNICMP
#       undef  HAVE_YIELD
#       define HAVE_WCTOMB
#       define HAVE_MBTOWC

#       ifdef OS_VXWORKS_KERNEL
#           define HAVE_VMLIB_H
#           define HAVE_VMPAGESIZEGET
#       endif

#   else
#       error "unknown vxWorks version"
#   endif

#elif defined(OS_WICED)

/* Headers provided by WICED-Studio-6.0/43xxx_Wi-Fi/tools/ARM_GNU/include */
#   define HAVE_ALLOCA_H
#   define HAVE_CTYPE_H
#   define HAVE_ERRNO_H
#   define HAVE_FCNTL_H
#   define HAVE_FNMATCH_H
#   define HAVE_GLOB_H
#   define HAVE_INTTYPES_H
#   define HAVE_LIMITS_H
#   define HAVE_MALLOC_H
#   define HAVE_MATH_H
#   define HAVE_PTHREAD_H
#   define HAVE_SCHED_H
#   define HAVE_SIGNAL_H
#   define HAVE_STDINT_H
#   define HAVE_STDIO_H
#   define HAVE_STDLIB_H
#   define HAVE_STRINGS_H
#   define HAVE_STRING_H
#   define HAVE_SYS_PARAM_H
#   define HAVE_SYS_TIME_H
#   define HAVE_SYS_TYPES_H
#   define HAVE_SYS_WAIT_H
#   define HAVE_TIME_H
#   define HAVE_UNISTD_H
#   define HAVE_WCHAR_H
#   define HAVE_WCTYPE_H

/* Headers provided by WICED-Studio-6.0/43xxx_Wi-Fi/tools/ARM_GNU/include/tr1 */
#   define HAVE_CTYPE_H
#   define HAVE_FLOAT_H
#   define HAVE_INTTYPES_H
#   define HAVE_LIMITS_H
#   define HAVE_MATH_H
#   define HAVE_STDBOOL_H
#   define HAVE_STDINT_H
#   define HAVE_STDIO_H
#   define HAVE_STDLIB_H
#   define HAVE_WCHAR_H
#   define HAVE_WCTYPE_H

/* Functions available in any WICED application */
#   define HAVE_ALLOCA
#   define HAVE_CTIME_R
#   define HAVE_FCNTL
#   define HAVE_FLOOR
#   define HAVE_GETPID
#   define HAVE_GETTIMEOFDAY
#   define HAVE_LOCALTIME
#   define HAVE_MBTOWC
#   define HAVE_MEMMOVE
#   define HAVE_MEMSET
#   define HAVE_OPEN
#   define HAVE_STRCASECMP
#   define HAVE_STRCMP
#   define HAVE_STRLEN
#   define HAVE_STRNCASECMP
#   define HAVE_STRNCMP
#   define HAVE_STRNCPY
#   define HAVE_TOLOWER
#   define HAVE_TOUPPER
#   define HAVE_TOWLOWER
#   define HAVE_TOWUPPER
#   define HAVE_VPRINTF
#   define HAVE_WCSCASECMP
#   define HAVE_WCSCMP
#   define HAVE_WCSLEN
#   define HAVE_WCSNCASECMP
#   define HAVE_WCSNCMP
#   define HAVE_WCSNCPY
#   define HAVE_WCTOMB

#	ifndef HOST_BYTE_ORDER
#		define HOST_BYTE_ORDER  LITTLE_ENDIAN
#	endif /* HOST_BYTE_ORDER */

#elif defined(OS_IAR)

#       define HAVE_STRING_H
#       define HAVE_STDLIB_H
#       define HAVE_STDINT_H
#       define HAVE_STDIO_H
#       define HAVE_WCHAR_H
#       define HAVE_WCTYPE_H
#       define HAVE_LIMITS_H
#       define HAVE_MATH_H
#       define HAVE_FLOAT_H
#       define HAVE_CTYPE_H
#       define HAVE_ERRNO_H
#       define HAVE_STDBOOL_H

#       define HAVE_TOUPPER
#       define HAVE_TOLOWER
#       define HAVE_STRLEN
#       define HAVE_STRCMP
#       define HAVE_STRNCMP
#       define HAVE_STRNCPY
#       define HAVE_TOWUPPER
#       define HAVE_TOWLOWER
#       define HAVE_WCSLEN
#       define HAVE_WCSCMP
#       define HAVE_WCSNCMP
#       define HAVE_WCSNCPY
#       define HAVE_WCTOMB
#       define HAVE_MBTOWC

#elif defined(OS_ARMCC)

#       define HAVE_STRING_H
#       define HAVE_STDLIB_H
#       define HAVE_STDINT_H
#       define HAVE_STDIO_H
#       define HAVE_WCHAR_H
#       define HAVE_WCTYPE_H
#       define HAVE_LIMITS_H
#       define HAVE_MATH_H
#       define HAVE_CTYPE_H
#       define HAVE_FLOAT_H

#       define HAVE_TOUPPER
#       define HAVE_TOLOWER
#       define HAVE_STRLEN
#       define HAVE_STRCMP
#       define HAVE_STRNCMP
#       define HAVE_STRNCPY
#       define HAVE_TOWUPPER
#       define HAVE_TOWLOWER
#       define HAVE_WCSLEN
#       define HAVE_WCSCMP
#       define HAVE_WCSNCMP
#       define HAVE_WCSNCPY
#       define HAVE_WCTOMB
#       define HAVE_MBTOWC
#       define HAVE_STRCASECMP
#       define HAVE_STRNCASECMP

#if defined(__BIG_ENDIAN)
#if !defined(__BIG_ENDIAN__)
#       define __BIG_ENDIAN__
#endif
#elif !defined(__LITTLE_ENDIAN__)
#       define __LITTLE_ENDIAN__
#endif

#elif defined(OS_CROSSWORKS)

#       define HAVE_LIMITS_H
#       define HAVE_STRING_H
#       define HAVE_STDLIB_H
#       define HAVE_CTYPE_H
#       define HAVE_STDINT_H
#       define HAVE_MATH_H
#       define HAVE_FLOAT_H

#       define HAVE_TOUPPER
#       define HAVE_TOLOWER
#       define HAVE_STRLEN
#       define HAVE_STRCMP
#       define HAVE_STRNCMP
#       define HAVE_STRNCPY

#elif defined(OS_POSIX)

#	define HAVE_LIMITS_H

#	ifndef OS_VXWORKS_55
#   	define HAVE_STDINT_H
#	endif

#elif defined(OS_GNUC)

#   define HAVE_CTYPE_H
#   define HAVE_ERRNO_H
#   define HAVE_FCNTL_H
#   define HAVE_FLOAT_H
#   define HAVE_FNMATCH_H
#   define HAVE_GLOB_H
#   define HAVE_INTTYPES_H
#   define HAVE_LIMITS_H
#   define HAVE_MALLOC_H
#   define HAVE_MATH_H
#   define HAVE_PTHREAD_H
#   define HAVE_SCHED_H
#   define HAVE_SIGNAL_H
#   define HAVE_SPAWN_H
#   define HAVE_STDBOOL_H
#   define HAVE_STDINT_H
#   define HAVE_STDIO_H
#   define HAVE_STDLIB_H
#   define HAVE_STRINGS_H
#   define HAVE_STRING_H
#   define HAVE_SYS_PARAM_H
#   define HAVE_SYS_SELECT_H
#   define HAVE_SYS_STAT_H
#   define HAVE_SYS_TIME_H
#   define HAVE_SYS_TYPES_H
#   define HAVE_SYS_WAIT_H
#   define HAVE_TIME_H
#   define HAVE_UNISTD_H
#   define HAVE_WCHAR_H
#   define HAVE_WCTYPE_H
#   ifndef HOST_BYTE_ORDER
#       define HOST_BYTE_ORDER  LITTLE_ENDIAN
#   endif /* HOST_BYTE_ORDER */

#endif /* OS_GNUC */

#endif /* HAVE_CONFIG_H */

#if defined(HAVE_WCHAR_H) && !defined(DB_EXCLUDE_UNICODE)
#define DB_UNICODE_STRINGS
#endif

/** @name other settings: */
/** define DB_LIMIT_FILE_SIZE to allow storage size limitation, this
 * is currently used for debug purposes, however eventually will become
 * fixed storage size support;
 * Note that the support is canceled, since it requires extending
 * StorageMode with unused parameter
 */

#ifdef DB_DEBUG
/* #define DB_LIMIT_FILE_SIZE */
#endif

#if defined(HAVE_FCNTL_H) && defined(HAVE_FCNTL) && !defined(HAVE_FCNTL_LOCK)
#   define HAVE_FCNTL_LOCK
#endif

#endif /* OS_ENV_H */
