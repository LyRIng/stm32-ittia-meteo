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

#ifndef ATOMIC_LOCK_H
#define ATOMIC_LOCK_H

#include "ittia/os/os_wait_time.h"

#if !defined(OS_HAVE_ATOMIC_LOCK)                         \
    && (OS_HAVE_ATOMIC_TRYLOCK >= ATOMIC_IPC       \
        || OS_HAVE_ATOMIC_CAS >= ATOMIC_IPC        \
        || OS_HAVE_ATOMIC_EXCH >= ATOMIC_IPC)

#define OS_HAVE_ATOMIC_LOCK         ATOMIC_IPC
#define OS_ATOMIC_LOCK_DOMAIN_SIZE   32
typedef struct os_AtomicLockDomain
{
    os_AtomicValue locks[OS_ATOMIC_LOCK_DOMAIN_SIZE];
} os_AtomicLockDomain;

C_INLINE_DECL size_t
os_ptr_index( void * ptr )
{
    return ((size_t)(uintptr_t)ptr >> 2) % OS_ATOMIC_LOCK_DOMAIN_SIZE;
}


C_INLINE_DECL void
os_ptr_lock_init( os_AtomicLockDomain * dom )
{
    size_t j;

    for (j = OS_ATOMIC_LOCK_DOMAIN_SIZE; j-- > 0; )
        dom->locks[j] = 0;
}

C_INLINE_DECL void
os_ptr_lock( os_AtomicLockDomain * dom, void * ptr )
{
    size_t index = os_ptr_index( ptr );
    
#ifdef OS_HAVE_ATOMIC_TRYLOCK
    while ( os_atomic_native_trylock( &dom->locks[index] ) == 0 )
        os_cpu_yield();
#elif defined(OS_HAVE_ATOMIC_CAS)
    while ( os_atomic_native_cas( &dom->locks[index], 0, 1 ) == 0 )
        os_cpu_yield();
#elif defined(OS_HAVE_ATOMIC_EXCH)
    while ( os_atomic_native_exch( &dom->locks[index], 1 ) == 1 )
        os_cpu_yield();

#else
#error "implementation?"
#endif
    /* TODO some of these might require memory barrier, some not.
     * Currently I expect the necessary barriers are included into
     * primitives themself.
     */
/*    os_atomic_native_rmb();
 */
}

C_INLINE_DECL void
os_ptr_unlock( os_AtomicLockDomain * dom, void * ptr )
{
    size_t index = os_ptr_index(ptr);
    
    /* TODO some of these might require memory barrier, some not.
     * Currently I expect the necessary barriers are included into
     * primitives themself.
     */
/*    os_atomic_native_wmb();
 */
    
#ifdef OS_HAVE_ATOMIC_TRYLOCK
    os_atomic_native_unlock( &dom->locks[index] );
#else
    os_atomic_native_store( &dom->locks[index], 0 );
#endif
}

#endif

#if !defined(OS_HAVE_ATOMIC_LOCK)                   \
    && (OS_HAVE_ATOMIC_ADD >= ATOMIC_IPC       \
        || OS_HAVE_ATOMIC_INC >= ATOMIC_IPC    \
        || OS_HAVE_ATOMIC_DEC >= ATOMIC_IPC)

#define OS_HAVE_ATOMIC_LOCK         ATOMIC_IPC
#define OS_ATOMIC_LOCK_DOMAIN_SIZE   32

typedef struct os_AtomicLockDomain
{
    struct {
        os_AtomicValue ticket, pass;
    } locks[OS_ATOMIC_DOMAIN_SIZE];
} os_AtomicLockDomain;

C_INLINE_DECL size_t
os_ptr_index( void * ptr )
{
    return ((size_t)(uintptr_t)ptr >> 2) % OS_ATOMIC_LOCK_DOMAIN_SIZE;
}

C_INLINE_DECL void
os_ptr_lock( os_AtomicLockDomain * dom, void * ptr )
{
    size_t index = os_ptr_index(ptr);
    os_AtomicValue mine;

#if defined( OS_HAVE_ATOMIC_INC )
    mine = os_atomic_native_inc( &dom->locks[ index ].ticket );
#elif defined( OS_HAVE_ATOMIC_DEC )
    mine = os_atomic_native_dec( &dom->locks[ index ].ticket );
#elif defined( OS_HAVE_ATOMIC_ADD )
    mine = os_atomic_native_add( &dom->locks[ index ].ticket, 1 );
#else
#error "implementation?"
#endif

    while( dom->locks[ index ].pass != mine )
        os_cpu_yield();
    os_atomic_native_rmb();
}

C_INLINE_DECL void
os_ptr_unlock( os_AtomicLockDomain * dom, void * ptr )
{
    os_AtomicValue next;
    size_t index = os_ptr_index(ptr);

    os_atomic_native_wmb();

    nextval = os_atomic_native_fetch_lk( &dom->locks[index].pass) );

#if defined( OS_HAVE_ATOMIC_INC )
    nextval = os_atomic_native_inc( &dom->locks[ index ].ticket );
#elif defined( OS_HAVE_ATOMIC_DEC )
    nextval = os_atomic_native_dec( &dom->locks[ index ].ticket );
#elif defined( OS_HAVE_ATOMIC_ADD )
    nextval = os_atomic_native_add( &dom->locks[ index ].ticket, 1 );
#else
#error "implementation?"
#endif

    os_atomic_native_store( &dom->locks[index].pass, nextval );
}


#endif

#endif
