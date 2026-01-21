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

#include "ittia/os/atomic/atomic.h"
#include "os/os_lib.h"
/* compiler diagnostic */

#if (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 4) || defined(_MSC_VER)

#	define TO_STRING__(x)	#x
#	define TO_STRING(x)	TO_STRING__(x)

#	pragma message ("=== Compiling Environment Info ===")

#	ifdef __GNUC__
#		pragma message ("	Compiler: GCC " TO_STRING(__GNUC__) "." TO_STRING(__GNUC_MINOR__) "." TO_STRING(__GNUC_PATCHLEVEL__))
#	elif defined(_MSC_VER)
#		pragma message ("	Compiler: MSC (ver:" TO_STRING(_MSC_VER) ")" )
#	else
#		pragma message ("	Compiler: Unknown")
#	endif

#	if defined(OS_WINCE)
#		pragma message ("	OS      : WinCE")
#	elif defined(OS_WIN32)
#		pragma message ("	OS      : Win32")
#	elif defined(OS_VXWORKS)
#		pragma message ("	OS      : VxWorks")
#	elif defined(OS_LINUX)
#		pragma message ("	OS      : Linux")
#	else
#		pragma message ("	OS      : Unknown")
#	endif

#	if defined(OS_CPU_ARM)
#		pragma message ("	CPU     : ARM (arch: " TO_STRING(OS_ARM_ARCH) ")")
#	elif defined(OS_CPU_X86)
#		pragma message ("	CPU     : X86 (arch: " TO_STRING(OS_X86_ARCH) ")")
#	elif defined(OS_CPU_GENERIC)
#		pragma message ("	CPU     : Generic")
#	else
#		pragma message ("	CPU     : Unknown")
#	endif

#	pragma message ("=== Atomic Package ===")
#	ifdef OS_HAVE_ATOMIC
#		pragma message ("	Have atomics")
#	else
#		pragma message ("	No atomics")
#	endif

#	ifdef OS_HAVE_ATOMIC_FETCH
#		pragma message ("	FETCH    : " TO_STRING(OS_HAVE_ATOMIC_FETCH))
#	else
#		pragma message ("	FETCH    : NOT DEFINED")
#	endif

#	ifdef OS_HAVE_ATOMIC_STORE
#		pragma message ("	STORE    : " TO_STRING(OS_HAVE_ATOMIC_STORE))
#	else
#		pragma message ("	STORE    : NOT DEFINED")
#	endif

#	ifdef OS_HAVE_ATOMIC_CAS
#		pragma message ("	CAS      : " TO_STRING(OS_HAVE_ATOMIC_CAS))
#	else
#		pragma message ("	CAS      : NOT DEFINED")
#	endif

#	ifdef OS_HAVE_ATOMIC_INC
#		pragma message ("	INC      : " TO_STRING(OS_HAVE_ATOMIC_INC))
#	else
#		pragma message ("	INC      : NOT DEFINED")
#	endif

#	ifdef OS_HAVE_ATOMIC_DEC
#		pragma message ("	DEC      : " TO_STRING(OS_HAVE_ATOMIC_DEC))
#	else
#		pragma message ("	DEC      : NOT DEFINED")
#	endif

#	ifdef OS_HAVE_ATOMIC_ADD
#		pragma message ("	ADD      : " TO_STRING(OS_HAVE_ATOMIC_ADD))
#	else
#		pragma message ("	ADD      : NOT DEFINED")
#	endif

#	ifdef OS_HAVE_ATOMIC_EXCH
#		pragma message ("	EXCH     : " TO_STRING(OS_HAVE_ATOMIC_EXCH))
#	else
#		pragma message ("	EXCH     : NOT DEFINED")
#	endif

#	ifdef OS_HAVE_ATOMIC_TRYLOCK
#		pragma message ("	TRYLOCK  : " TO_STRING(OS_HAVE_ATOMIC_TRYLOCK))
#	else
#		pragma message ("	TRYLOCK  : NOT DEFINED")
#	endif

#	ifdef OS_HAVE_ATOMIC_FETCH_PTR
#		pragma message ("	FETCH PTR: " TO_STRING(OS_HAVE_ATOMIC_FETCH_PTR))
#	else
#		pragma message ("	FETCH PTR: NOT DEFINED")
#	endif

#	ifdef OS_HAVE_ATOMIC_STORE_PTR
#		pragma message ("	STORE PTR: " TO_STRING(OS_HAVE_ATOMIC_STORE_PTR))
#	else
#		pragma message ("	STORE PTR: NOT DEFINED")
#	endif

#	ifdef OS_HAVE_ATOMIC_CAS_PTR
#		pragma message ("	CAS PTR  : " TO_STRING(OS_HAVE_ATOMIC_CAS_PTR))
#	else
#		pragma message ("	CAS PTR  : NOT DEFINED")
#	endif

#	ifdef OS_HAVE_ATOMIC_EXCH_PTR
#		pragma message ("	EXCH PTR : " TO_STRING(OS_HAVE_ATOMIC_EXCH_PTR))
#	else
#		pragma message ("	EXCH PTR : NOT DEFINED")
#	endif

#endif

#if 0 /* omit duplicate part for awile */
#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS)

#include "os/os_lib.h"
#include "ittia/os/os_error.h"
#include "ittia/os/os_debug.h"

#ifndef OS_HAVE_ATOMIC_NATIVE

#include "ittia/os/atomic/atomic_lock.h"

#ifdef OS_HAVE_ATOMIC_LOCK
static os_AtomicLockDomain app_domain;
#endif

#if !defined(OS_HAVE_ATOMIC_LOCK) && defined(HAVE_THREADS)

#include "ittia/os/os_mutex.h"

#define OS_HAVE_ATOMIC_MUTEX

#define GENERIC_ATOMIC_LOCK_COUNT 32

static os_mutex_t locks[GENERIC_ATOMIC_LOCK_COUNT];
static os_AtomicValue os_atomic_initialized = 0;

static os_mutex_t&
lock_ptr( volatile void * ptr )
{
    size_t index = ((size_t)(ptr) >> 2) % GENERIC_ATOMIC_LOCK_COUNT;

    os_mutex_t& lock = locks[ index ];
    os_mutex_lock( &lock );

    return lock;
}

static C_INLINE_SPECIFIER void
unlock_ptr( os_mutex_t& mutex )
{
    os_mutex_unlock( &mutex );
}

#endif

os_AtomicValue
os_atomic_fetch(volatile const os_Atomic * ptr)
{
#ifdef OS_HAVE_ATOMIC_LOCK

    os_AtomicValue curval;

    os_ptr_lock( &app_domain, ptr );
    curval = os_atomic_fetch_lk( ptr );
    os_ptr_unlock( &app_domain, ptr );
    return curval;

#elif defined(OS_HAVE_ATOMIC_MUTEX)

    os_AtomicValue curval;

    os_mutex_t& mutex = lock_ptr( ptr );
    curval = os_atomic_fetch_lk( ptr );
    unlock_ptr( mutex );
    return curval;

#else
#error "impl?"
#endif
}

void
os_atomic_store(volatile os_Atomic * ptr, os_AtomicValue newval)
{
#ifdef OS_HAVE_ATOMIC_LOCK

    os_ptr_lock( &app_domain, ptr );
    os_atomic_store_lk( ptr, newval );
    os_ptr_unlock( &app_domain, ptr );

#elif defined(OS_HAVE_ATOMIC_MUTEX)

    os_mutex_t& mutex = lock_ptr( ptr );
    os_atomic_store_lk( ptr, newval );
    unlock_ptr( mutex );

#else
#error "impl?"
#endif
}

os_AtomicValue
os_atomic_inc( volatile os_Atomic * ptr)
{
#ifdef OS_HAVE_ATOMIC_LOCK

    os_AtomicValue newval;
    
    os_ptr_lock( &app_domain, ptr );
    newval = os_atomic_fetch_lk( ptr ) + 1;
    os_atomic_store_lk( ptr, newval );
    os_ptr_unlock( &app_domain, ptr );
    return newval;

#elif defined(OS_HAVE_ATOMIC_MUTEX)
    os_AtomicValue newval;
    
    os_mutex_t& mutex = lock_ptr( ptr );
    newval = os_atomic_fetch_lk( ptr ) + 1;
    os_atomic_store_lk( ptr, newval );
    unlock_ptr( mutex );
    return newval;

#else
#error "impl?"
#endif
}

os_AtomicValue
os_atomic_dec( volatile os_Atomic * ptr )
{
#ifdef OS_HAVE_ATOMIC_LOCK

    os_AtomicValue newval;

    os_ptr_lock( &app_domain, ptr );
    newval = os_atomic_fetch_lk( ptr ) - 1;
    os_atomic_store_lk( ptr, newval );
    os_ptr_unlock( &app_domain, ptr );
    return newval;

#elif defined(OS_HAVE_ATOMIC_MUTEX)

    os_AtomicValue newval;

    os_mutex_t& mutex = lock_ptr( ptr );
    newval = os_atomic_fetch_lk( ptr ) - 1;
    os_atomic_store_lk( ptr, newval );
    unlock_ptr( mutex );
    return newval;

#else
#error "impl?"
#endif
}

os_AtomicValue
os_atomic_add( volatile os_Atomic * ptr, os_AtomicValue addval)
{
#ifdef OS_HAVE_ATOMIC_LOCK

    os_AtomicValue curval;
    os_AtomicValue newval;

    os_ptr_lock( &app_domain, ptr );
    curval = os_atomic_fetch_lk( ptr );
    newval = curval + addval;
    os_atomic_store_lk( ptr, newval);
    os_ptr_unlock( &app_domain, ptr );
    return newval;

#elif defined(OS_HAVE_ATOMIC_MUTEX)

    os_AtomicValue curval;
    os_AtomicValue newval;

    os_mutex_t& mutex = lock_ptr( ptr );
    curval = os_atomic_fetch_lk( ptr );
    newval = curval + addval;
    os_atomic_store_lk( ptr, newval);
    unlock_ptr( mutex );
    return newval;

#else
#error "impl?"
#endif
}

db_bool_t
os_atomic_cas( volatile os_Atomic * ptr, os_AtomicValue oldval, os_AtomicValue newval )
{
#ifdef OS_HAVE_ATOMIC_LOCK

    os_AtomicValue curval;

    os_ptr_lock( &app_domain, ptr );
    curval = os_atomic_fetch_lk( ptr );
    if (newval == oldval)
        os_atomic_store_lk( ptr, newval );
    os_ptr_unlock( &app_domain, ptr );
    return curval == oldval;

#elif defined(OS_HAVE_ATOMIC_MUTEX)

    os_AtomicValue curval;

    os_mutex_t& mutex = lock_ptr( ptr );
    curval = os_atomic_fetch_lk( ptr );
    if (newval == oldval)
        os_atomic_store_lk( ptr, newval );
    unlock_ptr( mutex );
    return curval == oldval;

#else
#error "impl?"
#endif
}

os_AtomicValue
os_atomic_exch( volatile os_Atomic * ptr, os_AtomicValue newval )
{
#ifdef OS_HAVE_ATOMIC_LOCK

    os_AtomicValue curval;

    os_ptr_lock( &app_domain, ptr );
    curval = os_atomic_fetch_lk( ptr );
    os_atomic_store_lk( ptr, newval );
    os_ptr_unlock( &app_domain, ptr );
    return curval;

#elif defined(OS_HAVE_ATOMIC_MUTEX)

    os_AtomicValue curval;

    os_mutex_t& mutex = lock_ptr( ptr );
    curval = os_atomic_fetch_lk( ptr );
    os_atomic_store_lk( ptr, newval );
    unlock_ptr( mutex );
    return curval;

#else
#error "impl?"
#endif
}

db_bool_t
os_atomic_trylock( volatile os_Atomic * ptr )
{
#ifdef OS_HAVE_ATOMIC_LOCK

    os_AtomicValue curval;

    os_ptr_lock( &app_domain, ptr );
    curval = os_atomic_fetch_lk( ptr );
    os_atomic_store_lk( ptr, 1 );
    os_ptr_unlock( &app_domain, ptr );
    return curval == 0;


#elif defined(OS_HAVE_ATOMIC_MUTEX)

    os_AtomicValue curval;

    os_mutex_t& mutex = lock_ptr( ptr );
    curval = os_atomic_fetch_lk( ptr );
    if (curval == 0)
        os_atomic_store_lk( ptr, 1 );
    unlock_ptr( mutex );
    return curval == 0;

#else
#error "impl?"
#endif
}

void
os_atomic_unlock( volatile os_Atomic * ptr )
{
#ifdef OS_HAVE_ATOMIC_LOCK

    os_ptr_lock( &app_domain, ptr );
    os_atomic_store_lk( ptr, 0 );
    os_ptr_unlock( &app_domain, ptr );

#elif defined(OS_HAVE_ATOMIC_MUTEX)

    os_mutex_t& mutex = lock_ptr( ptr );
    os_atomic_store_lk( ptr, 0 );
    unlock_ptr( mutex );

#else
#error "impl?"
#endif
}

#endif /* OS_HAVE_ATOMIC_NATIVE */

int
os_atomic2_init(void)
{
#ifdef OS_HAVE_ATOMIC_NATIVE
    return DB_NOERROR;
#elif defined( OS_HAVE_ATOMIC_LOCK )
    os_ptr_lock_init( & app_domain );
    return DB_NOERROR;
#elif defined( OS_HAVE_ATOMIC_MUTEX )

    int j;
    int rc;

    if (os_atomic_initialized)
        return DB_NOERROR;

    for(j = 0; j < GENERIC_ATOMIC_LOCK_COUNT; j++) {
        rc = os_mutex_init( &locks[j] );
        if (DB_FAILED(rc)) {
            while(--j >= 0)
                os_mutex_destroy( &locks[j] );

            return rc;
        }
    }
    os_atomic_initialized = 1;
    return DB_NOERROR;
#else
#error "initialization?"
#endif
}

int
os_atomic2_done(void)
{
#ifdef OS_HAVE_ATOMIC_NATIVE

    return DB_NOERROR;

#elif defined(OS_HAVE_ATOMIC_LOCK)

    return DB_NOERROR;

#elif defined(OS_HAVE_ATOMIC_MUTEX)

    int j;
    if (os_atomic_initialized) {
        for(j = 0; j < GENERIC_ATOMIC_LOCK_COUNT; j++)
            os_mutex_destroy( &locks[j] );
    }

#endif
    return DB_NOERROR;
}

#endif /* HAVE_THREADS */

#else /* stub code */

#include "ittia/os/os_error.h"

int
os_atomic2_init(void)
{
    return DB_NOERROR;
}

int
os_atomic2_done(void)
{
    return DB_NOERROR;
}

#endif
