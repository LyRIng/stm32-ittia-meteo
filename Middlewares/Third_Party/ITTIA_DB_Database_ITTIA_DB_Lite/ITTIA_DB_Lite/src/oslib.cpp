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
#include "ittia/os/oslib.h"
#include "os/os_lib.h"

#include "ittia/os/os_atomic.h"
#include "ittia/os/os_barrier.h"
#include "ittia/os/os_byte_order.h"
#include "ittia/os/os_condvar.h"
#include "ittia/os/os_config.h"
#include "ittia/os/os_debug.h"
#include "ittia/os/os_dump.h"
#include "ittia/os/os_env.h"
#include "ittia/os/os_error.h"
#include "ittia/os/os_fastlock.h"
#if defined(HAVE_RAMFS_FILE) && defined(HAVE_THREADS)
#include "ittia/os/os_file.h"
#endif
#include "ittia/os/os_limits.h"
#include "ittia/os/os_malloc.h"
#include "ittia/os/os_mutex.h"
#include "ittia/os/os_rwlock.h"
#include "ittia/os/os_sem.h"
#include "ittia/os/os_thread.h"
#include "ittia/os/os_tls.h"
#include "ittia/os/os_wait_time.h"
#ifdef DB_ENABLE_FAILURE
#include "ittia/os/os_fail.h"
#endif

#ifdef OS_WIN32
#include "os/win32/win32.h"
#endif

#ifdef OS_POSIX
#include "os/posix/posix.h"
#endif

#ifdef OS_POSIX
#include "os/threadx/threadx.h"
#endif

static int init_count = 0;
typedef int (*os_fun_t)(void);

#ifdef HAVE_THREADS
static os_mutex_t oslib_mutex;
#endif

static const struct
{
    os_fun_t init, fini;
} init_table[] = {
#ifdef HAVE_THREADS
    { &os_atomic_init, &os_atomic_done },
    { &os_atomic2_init, &os_atomic2_done },
#endif
    { &os_mem_init, &os_mem_done },
#ifdef OS_WIN32
    { &os_win32_init, &os_win32_done },
#endif
#ifdef OS_POSIX
    { &os_posix_init, &os_posix_done },
#endif
#ifdef OS_THREADX
    { &os_threadx_init, &os_threadx_done },
#endif
#ifdef OS_UCOS_II
    { &os_ucos_ii_init, &os_ucos_ii_done },
#endif
#ifdef OS_UCOS_III
    { &os_ucos_iii_init, &os_ucos_iii_done },
#endif

#ifdef HAVE_THREADS
#   ifdef HAVE_WIN32_THREADS
    { &os_win32_threads_init, &os_win32_threads_done },
#   endif
#   if defined(HAVE_POSIX_THREADS)
    { &os_posix_threads_init, &os_posix_threads_done },
#   endif
#   if defined(HAVE_THREADX_THREADS)
    { &os_threadx_threads_init, &os_threadx_threads_done },
#   endif
#   ifdef HAVE_UCOS_II_THREADS
    { &os_ucos_ii_threads_init, &os_ucos_ii_threads_done },
#   endif
#   ifdef HAVE_UCOS_III_THREADS
    { &os_ucos_iii_threads_init, &os_ucos_iii_threads_done },
#   endif
#endif
#ifdef DB_ENABLE_FAILURE
    { &os_fail_init, &os_fail_done },
#endif
#ifdef DB_DEBUG
    { &os_dump_init, &os_dump_done },
#endif
    { &init_db_error, &done_db_error },

#if defined(HAVE_RAMFS_FILE) && defined(HAVE_THREADS)
    { &os_ramfs_init, &os_ramfs_done },
#endif
};

#define NITEMS (sizeof(init_table) / sizeof(init_table[0]))

/* these are not expected to be called concurrently */
DBDLL_API int os_init()
{
    int rc;
    int j;

    if (init_count == 0) {

#ifdef HAVE_THREADS
        rc = os_mutex_init( &oslib_mutex );
        if (DB_FAILED(rc)) {
            return rc;
        }
#endif

        for(j = 0; j < (int)NITEMS; j++) {
            rc = init_table[j].init();
            if (rc < 0) {
                int k;
                for (k = j - 1; k >= 0; --k) {
                    init_table[k].fini();
                }
                return rc;
            }
        }
    }

    ++ init_count;

    return DB_NOERROR;
}

DBDLL_API int os_done()
{
    int rc = DB_NOERROR;
    int j;

    if (init_count == 0)
        return DB_NOERROR;

    -- init_count;

    if (init_count == 0) {
        for(j = NITEMS - 1; j >= 0; j--) {
            int cur_rc = init_table[j].fini();
            if (cur_rc < 0 && rc == DB_NOERROR) {
                /* Capture first return code on failure, but continue cleanup anyway. */
                rc = cur_rc;
            }
        }

#ifdef DB_DEBUG
        // Debug file must be closed before destroying the oslib_mutex
        os_debug_file(NULL);
#endif

#ifdef HAVE_THREADS
        os_mutex_destroy( &oslib_mutex );
#endif
    }

    return rc;
}

#ifdef HAVE_THREADS
void
oslib_lock()
{
    os_mutex_lock( &oslib_mutex );
}

void
oslib_unlock()
{
    os_mutex_unlock( &oslib_mutex );
}

#endif
