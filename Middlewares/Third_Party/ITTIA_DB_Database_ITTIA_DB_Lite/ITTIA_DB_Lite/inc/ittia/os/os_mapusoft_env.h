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


#ifndef OS_MAPUSOFT_ENV_H
#define OS_MAPUSOFT_ENV_H


#ifdef OS_MAPUSOFT
/* Ensure that C99 macros are also available in C++. */
#   define __STDC_CONSTANT_MACROS
#   define __STDC_LIMIT_MACROS

/* MapuSoft headers must be included before any system headers. */
#   include "os_target.h"
#   include "posix_interface.h"

/* Use the MapuSoft POSIX interface and ignore auto-config.h. */
#   define OS_POSIX 1
#   undef HAVE_CONFIG_H
#   define NO_STD_SSTREAM
#   define NO_STD_LOCALE

/* Specify available headers explicitly, as some headers are incompatible with CrossOS. */
#   if OS_TARGET == OS_VXWORKS
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
#       undef  HAVE_PTHREAD_H
#       undef  HAVE_SCHED_H
#       undef  HAVE_SEMAPHORE_H
#       undef  HAVE_SEMLIB_H
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
#       undef  HAVE_UNISTD_H
#       undef  HAVE_GLOB_H
#       define HAVE_WCHAR_H
#       define HAVE_WCTYPE_H
#       undef  HAVE_WINDOWS_H
#       define HAVE_CTYPE_H 1
#       define HAVE_DIRENT_H 1
#   elif OS_TARGET == OS_WINDOWS
#       define HAVE_CTYPE_H 1
#       define HAVE_DIRENT_H 1
#       define HAVE_ERRNO_H 1
#       define HAVE_FCNTL_H 1
#       define HAVE_FLOAT_H 1
#       define HAVE_INTTYPES_H 1
#       define HAVE_IO_H 1
#       define HAVE_LIMITS_H 1
#       define HAVE_LSEEK64 1
#       define HAVE_MALLOC_H 1
#       define HAVE_MATH_H 1
#       define HAVE_MEMORY_H 1
#       define HAVE_ODBCINST_H 1
#       undef  HAVE_PTHREAD_H
#       undef  HAVE_SCHED_H
#       undef  HAVE_SEMAPHORE_H
#       define HAVE_STDBOOL_H 1
#       define HAVE_STDINT_H 1
#       define HAVE_STDIO_H 1
#       define HAVE_STDLIB_H 1
#       define HAVE_STRINGS_H 1
#       define HAVE_STRING_H 1
#       define HAVE_SYS_LOCKING_H 1
#       define HAVE_SYS_PARAM_H 1
#       define HAVE_SYS_STAT_H 1
#       define HAVE_SYS_TIME_H 1
#       define HAVE_SYS_TYPES_H 1
#       define HAVE_TIME_H 1
#       undef  HAVE_UNISTD_H
#       define HAVE_WCHAR_H 1
#       define HAVE_WCTYPE_H 1
#       define HAVE_WINDOWS_H 1
#   endif

/* Include all system headers that might be provided by CrossOS, before
 * removing conflicting defines that CrossOS depends on. */
#   ifdef HAVE_STDINT_H
#       include <stdint.h>
#   endif

#   include "ittia/os/std/char.h"
#   include "ittia/os/std/dirent.h"
#   include "ittia/os/std/errno.h"
#   include "ittia/os/std/glob.h"
#   include "ittia/os/std/io.h"
#   include "ittia/os/std/limits.h"
#   include "ittia/os/std/malloc.h"
#   include "ittia/os/std/math.h"
#   include "ittia/os/std/memory.h"
#   include "ittia/os/std/param.h"
#   include "ittia/os/std/pthread.h"
#   include "ittia/os/std/semaphore.h"
#   include "ittia/os/std/stddef.h"
#   include "ittia/os/std/string.h"
#   include "ittia/os/std/time.h"
#   include "ittia/os/std/unix.h"

/* Remove conflicting defines from os_target_defines.h */
#   undef OS_NUCLEUS
#   undef OS_THREADX
#   undef OS_VXWORKS
#   undef OS_ECOS
#   undef OS_MQX
#   undef OS_UITRON
#   undef OS_PSOS
#   undef OS_LINUX
#   undef OS_WINDOWS
#   undef OS_TKERNEL
#   undef OS_QNX
#   undef OS_SOLARIS
#   undef OS_LYNXOS
#   undef OS_ANDROID
#   undef OS_NETBSD
#   undef OS_UCOS
#   undef SUCCESS
#endif

#endif
