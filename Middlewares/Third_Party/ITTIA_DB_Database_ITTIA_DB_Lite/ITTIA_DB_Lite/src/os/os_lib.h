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

#ifndef OS_LIB_H
#define OS_LIB_H

#include "ittia/os/os_config.h"
#include "ittia/os/os_error.h"

C_HEADER_BEGIN

dbstatus_t init_db_error(void);
dbstatus_t done_db_error(void);

int os_atomic_init(void);
int os_atomic_done(void);

#if defined(HAVE_GENERIC_ATOMIC) \
	&& !defined(GENERIC_ATOMIC_USE_FASTLOCK) \
	&& !defined(GENERIC_ATOMIC_USE_MUTEX) \
	&& !defined(GENERIC_ATOMIC_USE_SEMAPHORE)

#   define GENERIC_ATOMIC_USE_FASTLOCK

#endif

#ifdef OS_WIN32
int os_win32_init(void);
int os_win32_done(void);
#endif

#ifdef OS_POSIX
int os_posix_init(void);
int os_posix_done(void);
#endif

#ifdef OS_THREADX
int os_threadx_init(void);
int os_threadx_done(void);
#endif

#ifdef OS_UCOS_II
int os_ucos_ii_init(void);
int os_ucos_ii_done(void);
#endif

#ifdef OS_UCOS_III
int os_ucos_iii_init(void);
int os_ucos_iii_done(void);
#endif

#ifdef HAVE_WIN32_THREADS
int os_win32_threads_init(void);
int os_win32_threads_done(void);
#endif

#if defined(HAVE_POSIX_THREADS)
int os_posix_threads_init(void);
int os_posix_threads_done(void);
#endif

#if defined(HAVE_THREADX_THREADS)
int os_threadx_threads_init(void);
int os_threadx_threads_done(void);
#endif

#ifdef HAVE_UCOS_II_THREADS
int os_ucos_ii_threads_init(void);
int os_ucos_ii_threads_done(void);
#endif

#ifdef HAVE_UCOS_III_THREADS
int os_ucos_iii_threads_init(void);
int os_ucos_iii_threads_done(void);
#endif

extern int os_mem_init(void);
extern int os_mem_done(void);
#ifdef DB_DEBUG
extern int os_dump_init(void);
extern int os_dump_done(void);
#endif
#if defined(HAVE_RAMFS_FILE) && defined(HAVE_THREADS)
extern int os_ramfs_init(void);
extern int os_ramfs_done(void);
#endif
#ifdef HAVE_THREADS
extern int os_atomic2_init(void);
extern int os_atomic2_done(void);
#endif
#ifdef DB_ENABLE_FAILURE
extern int os_fail_init(void);
extern int os_fail_done(void);
#endif

int os_dump_init(void);
int os_dump_done(void);

/* execute thread finalizers */
void os_thread_call_finalizers(void);

/* could be used when locking granularity is not critical */
void oslib_lock(void);
void oslib_unlock(void);

C_HEADER_END

#endif /* OS_LIB_H */
