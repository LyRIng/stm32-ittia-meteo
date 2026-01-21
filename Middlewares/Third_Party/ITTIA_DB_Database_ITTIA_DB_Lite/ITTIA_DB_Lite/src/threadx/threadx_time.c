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

#include "ittia/os/os_config.h"

#ifdef HAVE_THREADX_THREADS

#include "os/threadx/threadx.h"
#include "ittia/os/os_wait_time.h"
#include "os_utils.h"

#include "os/os_mockup.h"

#ifdef OS_WICED
#include "wiced_time.h"
#endif

#include <time.h>

/* OS_TICKS_PER_SECOND is application dependent. Normally there are 10ms
 * between timer ticks, but it can be customized in _tx_initialize_low_level.
 *
 * Is it possible to get current tick interval? -Oleg
 */
#ifndef OS_TICKS_PER_SECOND
#define OS_TICKS_PER_SECOND 100
#endif

static os_wait_time_t os_tick_per_second = OS_TICKS_PER_SECOND;

DBDLL_API void
os_set_tick_per_second(os_wait_time_t ticks)
{
	os_tick_per_second = ticks;
}

#undef os_current_time
DBDLL_API os_wait_time_t
os_current_time(void)
{
    return SCALE_TIME( tx_time_get(), os_tick_per_second, OS_WAIT_TIME_PREC );
}

#undef os_sleep
DBDLL_API void
os_sleep(os_wait_time_t time)
{
	if (time == OS_WAIT_FOREVER)
		tx_thread_sleep(TX_WAIT_FOREVER);
	else
    	tx_thread_sleep( SCALE_TIME( time, OS_WAIT_TIME_PREC, os_tick_per_second ));
}

#undef os_cpu_yield
DBDLL_API void
os_cpu_yield(void)
{
    tx_thread_sleep(1);
}

#ifdef OS_WICED
DBDLL_API void
os_day_time(os_day_time_t * current_time)
{
    wiced_result_t result;
    wiced_utc_time_ms_t utc_time_ms;

    result = wiced_time_get_utc_time_ms(&utc_time_ms);
    *current_time = SCALE_TIME( utc_time_ms, 1000, OS_DAY_TIME_PREC );
}
#else
DBDLL_API void
os_day_time(os_day_time_t * current_time)
{
    static os_day_time_t base_time = 0;
    static ULONG prev_time = 0;
    const ULONG now_time = tx_time_get();
    if (base_time == 0) {
        // Initialize base timestamp from time() function on first use
        base_time = time(NULL);
        base_time = SCALE_TIME(base_time, 1, OS_DAY_TIME_PREC);
        base_time -= SCALE_TIME(now_time, os_tick_per_second, OS_DAY_TIME_PREC);
    }
    else if (now_time < prev_time) {
    	// When the timer overflows, increase the base time
    	base_time += SCALE_TIME( UINT32_MAX, os_tick_per_second, OS_DAY_TIME_PREC );
    }
    *current_time = base_time + SCALE_TIME( now_time, os_tick_per_second, OS_DAY_TIME_PREC );
    prev_time = now_time;
}
#endif

#endif /* HAVE_THREADX_THREADS */
