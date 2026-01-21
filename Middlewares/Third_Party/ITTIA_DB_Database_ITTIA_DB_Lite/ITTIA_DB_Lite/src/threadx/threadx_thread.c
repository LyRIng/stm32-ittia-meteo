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
 
#if defined(HAVE_THREADS) && defined(HAVE_THREADX_THREADS)

#include "ittia/os/os_error.h"
#include "os/threadx/threadx.h"
#include "os/os_lib.h"
#include "ittia/os/os_thread.h"
#include "ittia/os/os_atomic.h"
#include "ittia/os/os_malloc.h"
#include "ittia/os/os_tls.h"
#include "ittia/os/os_debug.h"
#include "ittia/os/std/errno.h"
#include "ittia/os/std/memory.h"
#include "ittia/os/generic/generic_tls.h"
#include "ittia/os/os_sem.h"

#include <signal.h>

#include "os/os_mockup.h"

struct os_thread_t
{
    TX_THREAD* thread_ptr;
    /* Pointer to the thread stack, if allocated by os_spawn_thread. */
    void* thread_stack;
    os_atomic_t ref_count;
    int is_joinable;
    os_sem_t join_sem;
    os_thread_proc_t proc;
    void * param;
};

typedef void *(*start_t) (void *);
static os_tls_key_t os_threadx_self;

static int _thread_close(os_thread_t * thread)
{
    if (os_atomic_dec(&thread->ref_count) == 0) {
        if (thread->thread_stack != NULL) {
            if (thread->is_joinable)
                os_sem_destroy(&thread->join_sem);
            
            tx_thread_delete(thread->thread_ptr);
            DB_FREE( thread->thread_ptr );
            DB_FREE( thread->thread_stack );
        }
        DB_FREE( thread );
    }
    return DB_NOERROR;
}

static void thread_start(ULONG p)
{
    os_thread_t * arg = (os_thread_t *)p;

    os_tls_set(os_threadx_self, arg);
    arg->proc(arg->param);

    os_thread_call_finalizers();

    if (arg->is_joinable) {
        /* Notify waiting thread that this thread is finished. */
        os_sem_post(&arg->join_sem, 1);
    }
    os_tls_set(os_threadx_self, NULL);
    _thread_close(arg);
}

DBDLL_API int os_thread_spawn(os_thread_proc_t proc,
                                       void * arg,
                                       int stack_size, 
                                       int flags,
                                       os_thread_t ** handle )
{
    int status;
    os_thread_t * h = NULL;
    UINT priority, preemption_threshold;
    ULONG time_slice;

    if ((flags & ~OS_THREAD_JOIN_MASK) != 0 || proc == NULL || stack_size < 0) {
        return set_db_error( DB_EINVAL );
    }
    
    if (stack_size == DEFAULT_STACK_SIZE)
        stack_size = SMALL_STACK_SIZE;

    if ((h = DB_MALLOC( sizeof(struct os_thread_t) )) == NULL)
        goto error;
    
    h->thread_ptr = NULL;
    h->thread_stack = NULL;

    /* Allocate thread control block. */    
    if ((h->thread_ptr = DB_MALLOC( sizeof(TX_THREAD) )) == NULL )
        goto error;
    /* Allocate thread stack. */
    if ((h->thread_stack = DB_MALLOC( stack_size )) == NULL )
        goto error;

    if ((flags && OS_THREAD_JOIN_MASK) == OS_THREAD_JOINABLE) {
        h->is_joinable = 1;
        os_sem_init(&h->join_sem, 0);
        /* Joinable threads have an extra reference that is consumed
         * by os_thread_join(). */
        h->ref_count = 2;
    } else {
        h->is_joinable = 0;
        h->ref_count = 1;
    }

    h->proc = proc;
    h->param = arg;
 
    if (handle)
        *handle = h;
    
    /* Inherit priority, preemption threshold, and time slice from this thread. */
    if ((status = tx_thread_info_get(tx_thread_identify(), TX_NULL, TX_NULL, TX_NULL, &priority, &preemption_threshold, &time_slice, TX_NULL, TX_NULL)) != TX_SUCCESS)
        goto error;
    
    /* Create and start thread. */
    if ((status = tx_thread_create(h->thread_ptr, "os_thread_spawn", thread_start, (ULONG)h, h->thread_stack, stack_size, priority, preemption_threshold, time_slice, TX_AUTO_START)) != TX_SUCCESS)
        goto error;

    return DB_NOERROR;

error:
    if (h != NULL) {
        if (h->thread_ptr != NULL)
            DB_FREE( h->thread_ptr );
        if (h->thread_stack != NULL)
            DB_FREE( h->thread_stack );
        DB_FREE( h );
    }
    return os_threadx_error(status);
}

DBDLL_API uintptr_t
os_thread_id()
{
    TX_THREAD* thread_ptr = tx_thread_identify();
    return (uintptr_t)thread_ptr;
}

DBDLL_API os_thread_t * os_thread_self(void)
{
    os_thread_t * p;
	DB_VERIFY( DB_NOERROR == os_tls_get(os_threadx_self, (void*)&p) );
	
    if (p == NULL) {
        if (os_thread_attach() < 0)
            return NULL;
		DB_VERIFY( DB_NOERROR == os_tls_get(os_threadx_self, (void*)&p) );
    }
    DB_ASSERT( p != NULL );
    return p;
}

DBDLL_API void os_thread_set_name(os_thread_t * h, const char * thread_name)
{
}

DBDLL_API int os_thread_join(os_thread_t * h)
{
    if (h == NULL)
        return set_db_error(DB_EINVAL);

    if ( tx_thread_identify() == h->thread_ptr )
        return set_db_error( DB_ESTATE );
    
    if (!h->is_joinable)
        return set_db_error( DB_ESTATE );

    os_sem_wait(&h->join_sem, OS_WAIT_FOREVER);
    return _thread_close(h);
}

DBDLL_API int os_thread_dup(os_thread_t * h)
{
    if (h == NULL)
        return set_db_error(DB_EINVAL);

    os_atomic_inc(&h->ref_count);
    return DB_NOERROR;
}

DBDLL_API int os_thread_close(os_thread_t * h)
{
    if (h == NULL)
        return set_db_error(DB_EINVAL);

    if ( tx_thread_identify() == h->thread_ptr )
        return set_db_error( DB_ESTATE );

    return _thread_close(h);
}

DBDLL_API int os_thread_attach(void)
{
    os_thread_t * p;
    int rc;

	rc = os_tls_get(os_threadx_self, (void*)&p);
	DB_ASSERT( rc == DB_NOERROR);
	
    /* A thread is only attached once. If it was started with os_spawn_thread
     * then it is already attached. */
    if (p == NULL) {
        p = DB_MALLOC( sizeof(struct os_thread_t) );
        if (p == NULL)
            return DB_FAILURE;

        /* Attach current thread. */
        p->thread_ptr = tx_thread_identify();
        /* Thread stack and control block are not owned by oslib. */
        p->thread_stack = NULL;
        p->proc = NULL;
        p->param = NULL;
        p->ref_count = 1;
        p->is_joinable = 0;
		if ((rc = os_tls_set(os_threadx_self, p)) != DB_NOERROR) {
            DB_FREE( p );
            return rc;
        }
    }
    return DB_NOERROR;
}

DBDLL_API int os_thread_detach(void)
{
    os_thread_t * p;
    int rc;

	if (DB_FAILED( rc = os_tls_get(os_threadx_self, (void*)&p) ))
        return rc;
    
    if (p != NULL) {
        /* Cannot detach from a thread started by os_spawn_thread because a thread
        * should not delete itself. */
        if (p->thread_stack != NULL) {
            return set_db_error(DB_ESTATE);
        }

        return _thread_close(p);
    }

    return DB_NOERROR;
}

int os_threadx_threads_init(void)
{
    return os_tls_init(&os_threadx_self);
}

int os_threadx_threads_done(void)
{
    return os_tls_destroy(&os_threadx_self);
}

#endif /* HAVE_THREADX_THREADS */
