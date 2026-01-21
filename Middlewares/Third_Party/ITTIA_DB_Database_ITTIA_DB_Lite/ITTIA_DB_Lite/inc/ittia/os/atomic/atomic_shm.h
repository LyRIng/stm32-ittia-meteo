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

#ifndef ATOMIC_SHM_H
#define ATOMIC_SHM_H

#include "ittia/os/atomic/atomic.h"

#if defined(OS_HAVE_ATOMIC)

#   include "ittia/os/atomic/atomic_lock.h"
#   include "ittia/os/os_debug.h"

#if OS_HAVE_ATOMIC_FETCH    >= ATOMIC_IPC \
    && OS_HAVE_ATOMIC_STORE >= ATOMIC_IPC \
    && OS_HAVE_ATOMIC_CAS >= ATOMIC_IPC \
    && OS_HAVE_ATOMIC_ADD >= ATOMIC_IPC \
    && OS_HAVE_ATOMIC_INC >= ATOMIC_IPC \
    && OS_HAVE_ATOMIC_DEC >= ATOMIC_IPC \
    && OS_HAVE_ATOMIC_EXCH >= ATOMIC_IPC \
    && OS_HAVE_ATOMIC_TRYLOCK >= ATOMIC_IPC

typedef os_Atomic sm_Atomic;
typedef os_AtomicValue sm_AtomicValue;

/* CPU is has native/short path support for shm atomic operations */
#define OS_HAVE_SHM_ATOMIC

#define OS_DECLARE_ATOMIC_DOMAIN( name )
#define OS_USE_ATOMIC_DOMAIN( name )          (NULL)
typedef void sm_AtomicDomain;

#define sm_atomic_init( dom )

#ifdef DB_DEBUG
C_INLINE_DECL sm_AtomicValue
sm_atomic_fetch(sm_AtomicDomain * dom, const sm_Atomic * ptr)
{
    DB_UNUSED( dom );
    return os_atomic_native_fetch( OS_ATOMIC_PTR(ptr) );
}

C_INLINE_DECL void
sm_atomic_store( sm_AtomicDomain * dom, sm_Atomic * ptr, sm_AtomicValue newval )
{
    DB_UNUSED( dom );
    os_atomic_native_store(OS_ATOMIC_PTR(ptr), newval);
}

C_INLINE_DECL db_bool_t
sm_atomic_cas( sm_AtomicDomain * dom, sm_Atomic * ptr, sm_AtomicValue oldval, sm_AtomicValue newval)
{
    DB_UNUSED( dom );
    return os_atomic_native_cas(OS_ATOMIC_PTR(ptr), oldval, newval);
}

C_INLINE_DECL sm_AtomicValue
sm_atomic_add(sm_AtomicDomain * dom, sm_Atomic * ptr, sm_AtomicValue addval)
{
    DB_UNUSED( dom );
    return os_atomic_native_add(OS_ATOMIC_PTR(ptr), addval);
}

C_INLINE_DECL sm_AtomicValue
sm_atomic_inc(sm_AtomicDomain * dom, sm_Atomic * ptr)
{
    DB_UNUSED( dom );
    return os_atomic_native_inc(OS_ATOMIC_PTR(ptr));
}

C_INLINE_DECL sm_AtomicValue
sm_atomic_dec(sm_AtomicDomain * dom, sm_Atomic * ptr)
{
    DB_UNUSED( dom );
    return os_atomic_native_dec(OS_ATOMIC_PTR(ptr));
}

C_INLINE_DECL sm_AtomicValue
sm_atomic_exch(sm_AtomicDomain * dom, sm_Atomic * ptr, sm_AtomicValue newval)
{
    DB_UNUSED( dom );
    return os_atomic_native_exch(OS_ATOMIC_PTR(ptr), newval);
}

C_INLINE_DECL db_bool_t
sm_atomic_trylock(sm_AtomicDomain * dom, sm_Atomic * ptr)
{
    DB_UNUSED( dom );
    return os_atomic_native_trylock(OS_ATOMIC_PTR(ptr)) ? DB_TRUE : DB_FALSE;
}

C_INLINE_DECL void
sm_atomic_unlock(sm_AtomicDomain * dom, sm_Atomic * ptr)
{
    DB_UNUSED( dom );
    os_atomic_native_unlock(OS_ATOMIC_PTR(ptr));
}

#else
#   define sm_atomic_fetch(dom, ptr)               os_atomic_native_fetch( OS_ATOMIC_PTR(ptr) )
#   define sm_atomic_store(dom, ptr, newval)       os_atomic_native_store(OS_ATOMIC_PTR(ptr), newval)
#   define sm_atomic_cas(dom, ptr, oldval, newval) os_atomic_native_cas(OS_ATOMIC_PTR(ptr), oldval, newval)
#   define sm_atomic_add(dom, ptr, addval)         os_atomic_native_add(OS_ATOMIC_PTR(ptr), addval)
#   define sm_atomic_inc(dom, ptr)                 os_atomic_native_inc(OS_ATOMIC_PTR(ptr))
#   define sm_atomic_dec(dom, ptr)                 os_atomic_native_dec(OS_ATOMIC_PTR(ptr))
#   define sm_atomic_exch(dom, ptr, newval)        os_atomic_native_exch(OS_ATOMIC_PTR(ptr), newval)
#   define sm_atomic_trylock(dom, ptr)             os_atomic_native_trylock(OS_ATOMIC_PTR(ptr))
#   define sm_atomic_unlock(dom, ptr)              os_atomic_native_unlock(OS_ATOMIC_PTR(ptr))
#endif

#elif OS_HAVE_ATOMIC_LOCK >= ATOMIC_IPC
    /* could emulate using locks */

//#pragma message ("simulate shm atomic using lock")
#   include "ittia/os/atomic/atomic_lock.h"

#define OS_HAVE_SHM_ATOMIC

typedef os_AtomicLockDomain sm_AtomicDomain;
typedef os_Atomic sm_Atomic;
typedef os_AtomicValue sm_AtomicValue;

#define OS_DECLARE_ATOMIC_DOMAIN( name )        sm_AtomicDomain name;
#define OS_USE_ATOMIC_DOMAIN( name )          &name

C_INLINE_DECL void
sm_atomic_init( sm_AtomicDomain * dom )
{
    os_ptr_lock_init( dom );
}

C_INLINE_DECL sm_AtomicValue
sm_atomic_fetch(sm_AtomicDomain * dom, sm_Atomic * ptr)
{
    sm_AtomicValue curval;

    os_ptr_lock( dom, ptr );
    curval = os_atomic_fetch_lk( ptr );
    os_ptr_unlock( dom, ptr );
    return curval;
}

C_INLINE_DECL void
sm_atomic_store(sm_AtomicDomain * dom, sm_Atomic * ptr, sm_AtomicValue newval)
{
    os_ptr_lock( dom, ptr );
    os_atomic_store_lk( ptr, newval );
    os_ptr_unlock( dom, ptr );
}

C_INLINE_DECL sm_AtomicValue
sm_atomic_inc( sm_AtomicDomain * dom, sm_Atomic * ptr)
{
    sm_AtomicValue newval;
    
    os_ptr_lock( dom, ptr );
    newval = os_atomic_fetch_lk( ptr ) + 1;
    os_atomic_store_lk( ptr, newval );
    os_ptr_unlock( dom, ptr );
    return newval;
}

C_INLINE_DECL sm_AtomicValue
sm_atomic_dec( sm_AtomicDomain * dom, sm_Atomic * ptr )
{
    sm_AtomicValue newval;

    os_ptr_lock( dom, ptr );
    newval = os_atomic_fetch_lk( ptr ) - 1;
    os_atomic_store_lk( ptr, newval );
    os_ptr_unlock( dom, ptr );
    return newval;
}

C_INLINE_DECL sm_AtomicValue
sm_atomic_add( sm_AtomicDomain * dom, sm_Atomic * ptr, sm_AtomicValue addval)
{
    sm_AtomicValue curval;
    sm_AtomicValue newval;

    os_ptr_lock( dom, ptr );
    curval = os_atomic_fetch_lk( ptr );
    newval = curval + addval;
    os_atomic_store_lk( ptr, newval);
    os_ptr_unlock( dom, ptr );
    return newval;
}

C_INLINE_DECL db_bool_t
sm_atomic_cas( sm_AtomicDomain * dom, sm_Atomic * ptr, sm_AtomicValue oldval, sm_AtomicValue newval )
{
    sm_AtomicValue curval;

    os_ptr_lock( dom, ptr );
    curval = os_atomic_fetch_lk( ptr );
    if (curval == oldval)
        os_atomic_store_lk( ptr, newval );
    os_ptr_unlock( dom, ptr );
    return curval == oldval;
}

C_INLINE_DECL sm_AtomicValue
sm_atomic_exch( sm_AtomicDomain * dom, sm_Atomic * ptr, sm_AtomicValue newval )
{
    sm_AtomicValue curval;

    os_ptr_lock( dom, ptr );
    curval = os_atomic_fetch_lk( ptr );
    os_atomic_store_lk( ptr, newval );
    os_ptr_unlock( dom, ptr );
    return curval;
}

C_INLINE_DECL db_bool_t
sm_atomic_trylock( sm_AtomicDomain * dom, sm_Atomic * ptr )
{
    sm_AtomicValue curval;

    os_ptr_lock( dom, ptr );
    curval = os_atomic_fetch_lk( ptr );
    if (curval == 0)
        os_atomic_store_lk( ptr, 1 );
    os_ptr_unlock( dom, ptr );
    return curval == 0;
}

C_INLINE_DECL void
sm_atomic_unlock( sm_AtomicDomain * dom, sm_Atomic * ptr )
{
    os_ptr_lock( dom, ptr );
    os_atomic_store_lk( ptr, 0 );
    os_ptr_unlock( dom, ptr );
}

#else
    /* there is no known way to support shared memory operations using atomic
     * This doesn't mean it's imposible at all, however we need a kind of dispatcher
     * which is out of scope of this implementation.
     */
#pragma message ("no shm atomic")
#endif

/* convinience macros for locked access */

#define sm_atomic_fetch_lk(ptr) os_atomic_fetck_lk(ptr)
#define sm_atomic_store_lk(ptr, newval) os_atomic_store_lk(ptr, newval)
#define sm_atomic_add_lk(ptr, addval) os_atomic_add_lk( ptr, addval )
#define sm_atomic_inc_lk(ptr)   os_atomic_inc_lk(ptr)
#define sm_atomic_dec_lk(ptr)   os_atomic_dec_lk(ptr)
#define sm_atomic_cas_lk(ptr, oldval, newval) os_atomic_cas_lk(ptr, oldval, newval)
#define sm_atomic_exch_lk(ptr, newval) os_atomic_exch_lk(ptr, newval)
#define sm_atomic_unlock_lk(ptr, newval) os_atomic_unlock_lk(ptr, newval)

#endif /* OS_HAVE_ATOMIC */

#endif /* ATOMIC_SHM_H */
