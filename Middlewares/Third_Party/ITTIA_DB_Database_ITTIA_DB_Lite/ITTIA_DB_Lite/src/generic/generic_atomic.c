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

#if defined(HAVE_THREADS) && defined(HAVE_GENERIC_ATOMIC)

#include "ittia/os/os_atomic.h"
#include "os/os_lib.h"
#include "ittia/os/os_error.h"
#include "ittia/os/os_debug.h"

#include "os/os_mockup.h"
/* check if all atomic functions are inlined to the native atomic implementation. */
#if defined(OS_HAVE_ATOMIC) \
    && defined(OS_HAVE_ATOMIC_FETCH)    \
    && defined(OS_HAVE_ATOMIC_STORE)    \
    && defined(OS_HAVE_ATOMIC_CAS)      \
    && defined(OS_HAVE_ATOMIC_INC)      \
    && defined(OS_HAVE_ATOMIC_DEC)      \
    && defined(OS_HAVE_ATOMIC_ADD)      \
    && defined(OS_HAVE_ATOMIC_CAS_PTR)

int os_atomic_init(void)
{
    return DB_NOERROR;
}

int os_atomic_done(void)
{
    return DB_NOERROR;
}

#else /* OS_HAVE_ATOMIC */

#if defined(GENERIC_ATOMIC_USE_FASTLOCK)

#   include "ittia/os/os_fastlock.h"
#   define LOCK_TYPE        os_fastlock_t
#   define LOCK_INIT(p)     os_fastlock_init(p)
#   define LOCK_DESTROY(p)  os_fastlock_destroy(p)
#   define LOCK_ACQUIRE(p)  os_fastlock_lock(p)
#   define LOCK_RELEASE(p)  os_fastlock_unlock(p)

#elif defined(GENERIC_ATOMIC_USE_MUTEX)

#   include "ittia/os/os_mutex.h"
#   define LOCK_TYPE        os_mutex_t
#   define LOCK_INIT(p)     os_mutex_init(p)
#   define LOCK_DESTROY(p)  os_mutex_destroy(p)
#   define LOCK_ACQUIRE(p)  os_mutex_lock(p)
#   define LOCK_RELEASE(p)  os_mutex_unlock(p)

#elif defined(GENERIC_ATOMIC_USE_SEM)

#   include "ittia/os/os_sem.h"
#   define LOCK_TYPE        os_sem_t
#   define LOCK_INIT(p)     os_sem_init(p, 1)
#   define LOCK_DESTROY(p)  os_sem_destroy(p)
#   define LOCK_ACQUIRE(p)  os_sem_wait(p, OS_WAIT_FOREVER)
#   define LOCK_RELEASE(p)  os_sem_post(p, 1)

#else
#   error Choose generic atomic locking
#endif

#define GENERIC_ATOMIC_LOCK_COUNT 32
#define GENERIC_ATOMIC_LOCK_MASK  0x001Fu

static LOCK_TYPE locks[GENERIC_ATOMIC_LOCK_COUNT];
static int os_atomic_initialized = 0;

#ifdef GENERIC_ATOMIC_LOCK_MASK
#define LOCK_PTR(var) &locks[((uint32_t)(uintptr_t)(void*)(var)) & GENERIC_ATOMIC_LOCK_MASK]
#else
#define LOCK_PTR(var) &locks[((uint32_t)(uintptr_t)(void*)(var)) % GENERIC_ATOMIC_LOCK_COUNT]
#endif

#define LOCK(var)   LOCK_ACQUIRE(LOCK_PTR(var))
#define UNLOCK(var) LOCK_RELEASE(LOCK_PTR(var))

int os_atomic_init(void)
{
    int j;
    int rc;

    if (os_atomic_initialized)
        return DB_NOERROR;

    for(j = 0; j < GENERIC_ATOMIC_LOCK_COUNT; j++) {
        rc = LOCK_INIT(&locks[j]);
        if (DB_FAILED(rc)) {
            while(--j >= 0)
                LOCK_DESTROY(&locks[j]);

            return rc;
        }
    }
    os_atomic_initialized = 1;
    return DB_NOERROR;
}

int os_atomic_done(void)
{
    int j;
    if (os_atomic_initialized) {
        for(j = 0; j < GENERIC_ATOMIC_LOCK_COUNT; j++)
            LOCK_DESTROY(&locks[j]);
    }
    return DB_NOERROR;
}

#if defined(OS_HAVE_ATOMIC) \
    && defined(OS_HAVE_ATOMIC_FETCH)    \
    && defined(OS_HAVE_ATOMIC_STORE)    \
    && defined(OS_HAVE_ATOMIC_CAS)      \
    && defined(OS_HAVE_ATOMIC_INC)      \
    && defined(OS_HAVE_ATOMIC_DEC)      \
    && defined(OS_HAVE_ATOMIC_ADD)
/* os_atomic_t functions are inlined to the native implementation. */
#else

DBDLL_API int
os_atomic_cas(os_atomic_t * p, os_atomic_t old_value, os_atomic_t new_value)
{
    os_atomic_t value;

    DB_ASSERT( os_atomic_initialized != 0 );
    LOCK(p);
    value = *p;
    if (value == old_value)
        *p = new_value;
    UNLOCK(p);
    return value == old_value;
}

DBDLL_API os_atomic_t
os_atomic_add(os_atomic_t * p, os_atomic_t addval)
{
    os_atomic_t curval;
    os_atomic_t newval;

    DB_ASSERT( os_atomic_initialized != 0 );
    LOCK(p);
    curval = *p;
    newval = curval + addval;
    *p = newval;
    UNLOCK(p);
    return newval;
}

/**
 * set variable to given value and return previous one
 */
DBDLL_API os_atomic_t
os_atomic_exchange(os_atomic_t * p, os_atomic_t new_value)
{
    os_atomic_t old_value;

    DB_ASSERT( os_atomic_initialized != 0 );
    LOCK(p);
    old_value = *p;
    *p = new_value;
    UNLOCK(p);
    return old_value;
}

/**
 * increment value by one and return resulting value.
 */
DBDLL_API os_atomic_t
os_atomic_inc(os_atomic_t * p)
{
    os_atomic_t new_value;

    DB_ASSERT( os_atomic_initialized != 0 );
    LOCK(p);
    new_value = ++(*p);
    UNLOCK(p);
    return new_value;
}

DBDLL_API os_atomic_t os_atomic_dec(os_atomic_t * p)
{
    os_atomic_t new_value;

    DB_ASSERT( os_atomic_initialized != 0 );
    LOCK(p);
    new_value = --(*p);
    UNLOCK(p);
    return new_value;
}

DBDLL_API os_atomic_t os_atomic_fetch(const os_atomic_t * p)
{
    os_atomic_t value;

    DB_ASSERT( os_atomic_initialized != 0 );
    LOCK(p);
    value = *p;
    UNLOCK(p);
    return value;
}

DBDLL_API void os_atomic_store(os_atomic_t * p, os_atomic_t value)
{
    DB_ASSERT( os_atomic_initialized != 0 );
    LOCK(p);
    *p = value;
    UNLOCK(p);
}

#endif

#if defined(OS_HAVE_ATOMIC) && defined(OS_HAVE_ATOMIC_CAS_PTR)
/* atomic pointer functions are inlined to the native implementation. */
#else
DBDLL_API int
os_atomic_cas_ptr(void ** p, void * old_value, void * new_value)
{
    void * value;

    DB_ASSERT( os_atomic_initialized != 0 );
    LOCK(p);
    value = *p;
    if (value == old_value)
        *p = new_value;
    UNLOCK(p);
    return value == old_value;
}
#endif

#endif /* OS_HAVE_ATOMIC */

#endif /* HAVE_GENERIC_ATOMIC */
