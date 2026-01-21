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
 */

#ifndef OS_WAIT_TIME_H
#define OS_WAIT_TIME_H

#include "ittia/os/os_config.h"

C_HEADER_BEGIN

typedef uint32_t os_wait_time_t;
typedef uint64_t os_day_time_t; /* time in microseconds (OS_DAY_TIME_PREC) */

#define OS_WAIT_FOREVER 0xFFFFFFFFu /* */

/** Number of wait time units per second. */
#define OS_WAIT_TIME_PREC 1000
#define OS_DAY_TIME_PREC  1000000

/** Convert time from milliseconds to wait time units. */
#define WAIT_MILLISEC(x) (os_wait_time_t)(x)
/** Convert time from microseconds to wait time units. */
#define WAIT_MICROSEC(x) (os_wait_time_t)(((x) + 500) / 1000)

DBDLL_API void os_sleep(os_wait_time_t time);
DBDLL_API os_wait_time_t os_current_time(void);
DBDLL_API void os_cpu_yield(void);
DBDLL_API void os_day_time(os_day_time_t *);

#ifndef HAVE_THREADS
/* Stub out yield and time functions for single-user. */
#define os_sleep(x) (0)
#define os_current_time() (0)
#define os_cpu_yield() (0)
#endif /* HAVE_THREADS */

#ifdef HAVE_THREADX_THREADS
DBDLL_API void os_set_tick_per_second(os_wait_time_t ticks);
#endif

#if defined(DB_ENABLE_TIME_STATISTICS) && defined(OS_WIN32)

#include "ittia/os/std/windows.h"

typedef struct stat_timer_t {
    DWORD b;
    LARGE_INTEGER lb, le;
};

#define timer_frequency(dest) do { LARGE_INTEGER fq; dest = (QueryPerformanceFrequency(&fq) ? fq.QuadPart : 1000); } while(0)
#define timer_declare(timer) struct stat_timer_t timer
#define timer_start(timer) do { (timer).b = GetTickCount(); QueryPerformanceCounter(&(timer).lb); } while (0)
#define timer_store_elapsed(timer, counter) (counter += QueryPerformanceCounter(&(timer).le) ? (timer).le.QuadPart - (timer).lb.QuadPart : GetTickCount() - (timer).b)
#define timer_store_elapsed_check_max(timer, counter, max) do { LARGE_INTEGER elapsed = QueryPerformanceCounter(&(timer).le) ? (timer).le.QuadPart - (timer).lb.QuadPart : GetTickCount() - (timer).b; (counter) += elapsed; if (elapsed > (max)) { (max) = elapsed; } } while (0)

#elif defined(DB_ENABLE_TIME_STATISTICS) && (defined(OS_POSIX) || defined(OS_POSIX_ERROR))

#include "ittia/os/std/time.h"

#define timer_frequency(dest) do { dest = 1000000000; } while (0)
#define timer_declare(timer) struct timespec timer
#define timer_start(timer) clock_gettime(CLOCK_MONOTONIC, &(timer))
#define timer_store_elapsed(timer, counter) do { struct timespec end; clock_gettime(CLOCK_MONOTONIC, &end); counter += (uint64_t)(end.tv_sec - (timer).tv_sec) * 1000000000 + (uint64_t)(end.tv_nsec - (timer).tv_nsec); } while (0)
#define timer_store_elapsed_check_max(timer, counter, max) do { struct timespec end; clock_gettime(CLOCK_MONOTONIC, &end); uint64_t elapsed = (uint64_t)(end.tv_sec - (timer).tv_sec) * 1000000000 + (uint64_t)(end.tv_nsec - (timer).tv_nsec); (counter) += elapsed; if (elapsed > (max)) { (max) = elapsed; } } while (0)

#else

#define timer_frequency(dest) do { dest = 1; } while (0)
#define timer_declare(timer)
#define timer_start(timer)
#define timer_store_elapsed(timer, counter)
#define timer_store_elapsed_check_max(timer, counter, max)

#endif


C_HEADER_END

#endif /* OS_WAIT_TIME_H */
