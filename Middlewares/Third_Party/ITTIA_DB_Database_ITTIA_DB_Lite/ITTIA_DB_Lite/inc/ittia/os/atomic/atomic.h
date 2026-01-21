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

#ifndef ATOMIC_H
#define ATOMIC_H

#include "ittia/os/os_config.h"
#include "ittia/os/os_cpu.h"

/*
 * support level
 */

#define ATOMIC_NONE             0   /* not implemented */
#define ATOMIC_LOCAL            1   /* atomic operations are process local only */
#define ATOMIC_IPC              2   /* atomic operations are shared memory IPC safe */

/*
in general this file implements following sets of functions and types:

  === process-local atomic values ===

types

    os_AtomicValue     signed value to hold proper atomic value
    os_Atomic          placeholder for atomic value

macros

    OS_HAVE_ATOMIC     defined if OS supports process-local atomics
    OS_HAVE_ATOMIC_FETCH
    OS_HAVE_ATOMIC_STORE
    OS_HAVE_ATOMIC_CAS
    OS_HAVE_ATOMIC_ADD
    OS_HAVE_ATOMIC_INC
    OS_HAVE_ATOMIC_DEC
    OS_HAVE_ATOMIC_EXCH
    OS_HAVE_ATOMIC_TRYLOCK

functions

    os_AtomicValue os_atomic_fetch( const os_Atomic * );

    void os_atomic_store( os_Atomic *, os_AtomicValue value);

    db_bool_t os_atomic_cas( os_Atomic * ptr, os_AtomicValue oldval, os_AtomicValue newval );

    os_AtomicValue os_atomic_inc( os_Atomic * ptr );
        returns resulting value;
        return ++(*ptr);

    os_AtomicValue os_atomic_dec( os_Atomic * ptr );
        returns resulting value;
        return --(*ptr);

    os_AtomicValue os_atomic_add( os_Atomic * ptr, os_AtomicValue addval );
        returns resulting value;
        *ptr += addval;
        return *ptr;

    os_AtomicValue os_atomic_exch( os_Atomic * ptr, os_AtomicValue newval );
        exchange values:
        cur = *ptr;
        *cur = newval;
        return cur;

    db_bool_t os_atomic_trylock( os_Atomic * );
        test and set:
        return (*ptr == LOCKED) ? false : *ptr = LOCKED, true);

    void os_atomic_unlock(os_Atomic *)
        release lock acquired by os_atomic_trylock()
        os_atomic_dmb();
        *ptr = !LOCKED;

    void os_atomic_dmb()
	    data memory barrier (both load and store).

    void os_atomic_vadd(os_Atomic * ptr, os_AtomicValue add_val );
    void os_atomic_vinc(os_Atomic * ptr );
    void os_atomic_vdec(os_Atomic * ptr );

 */

/* include basic support */

/* use InterlockedXXX win32 API (available both for NT & CE) */

#if defined(OS_WIN32)
#   include "ittia/os/atomic/atomic_win32.h"
#endif

#if defined(OS_VXWORKS)
#   include "ittia/os/atomic/atomic_vxworks.h"
#endif

#if defined(__GNUC__) && defined(OS_CPU_ARM) && !defined(OS_VXWORKS)
    /* check for native ARM using gcc */
#   include "ittia/os/atomic/atomic_arm_gcc.h"
#endif

#if defined(OS_CPU_ARM) && defined(OS_LINUX)
#   include "ittia/os/atomic/atomic_arm_linux.h"
#endif

#if defined(__GNUC__) && defined(OS_CPU_X86)
#   include "ittia/os/atomic/atomic_x86_gcc.h"
#endif

#if defined(__GNUC__) && __GNUC__ >= 4
   /* gcc 4 has builtin atomic support */
#   include "ittia/os/atomic/atomic_gcc4.h"
#endif

/* complement support */

/* in case some specific functions missed */
#   include "ittia/os/atomic/atomic_generic.h"

#ifndef OS_HAVE_ATOMIC_VALUE
#   define OS_HAVE_ATOMIC_VALUE ATOMIC_LOCAL

    typedef int os_AtomicValue;
#   define OS_ATOMIC_NATIVE_INIT(x) (x)
#   define OS_ATOMIC_NATIVE_CTOR(x) (x)

#define os_atomic_native_fetch_lk(ptr)    (*(os_AtomicValue*)(ptr))
#define os_atomic_native_store_lk(ptr, newval)    (*(os_AtomicValue*)(ptr) = newval)

#endif /* OS_HAVE_ATOMIC_VALUE */

typedef struct os_Atomic
{
    os_AtomicValue val;
} os_Atomic;

#define OS_ATOMIC_PTR(x)    &((x)->val)
#define OS_ATOMIC_INIT(x)   { OS_ATOMIC_NATIVE_INIT(x) }

C_INLINE_DECL os_AtomicValue
os_atomic_fetch_lk( const os_Atomic * ptr )
{
    return os_atomic_native_fetch_lk( OS_ATOMIC_PTR(ptr) );
}

C_INLINE_DECL void
os_atomic_store_lk( os_Atomic * ptr, os_AtomicValue value)
{
    os_atomic_native_store_lk( OS_ATOMIC_PTR(ptr), value );
}


#if 0 /* omit duplicate part for awhile */

#ifdef __cplusplus
C_INLINE_DECL os_Atomic
OS_ATOMIC_CTOR(long x)
{
	os_Atomic w = OS_ATOMIC_INIT(x);
	return w;
}
#endif /* __cplusplus */

#if OS_HAVE_ATOMIC_FETCH    >= ATOMIC_LOCAL \
    && OS_HAVE_ATOMIC_STORE >= ATOMIC_LOCAL \
    && OS_HAVE_ATOMIC_CAS >= ATOMIC_LOCAL \
    && OS_HAVE_ATOMIC_ADD >= ATOMIC_LOCAL \
    && OS_HAVE_ATOMIC_INC >= ATOMIC_LOCAL \
    && OS_HAVE_ATOMIC_DEC >= ATOMIC_LOCAL \
    && OS_HAVE_ATOMIC_EXCH >= ATOMIC_LOCAL \
    && OS_HAVE_ATOMIC_TRYLOCK >= ATOMIC_LOCAL

#define OS_HAVE_ATOMIC_NATIVE /* guide the emulation layer */

#if 0
#   define OS_ATOMIC_DEBUG_YIELD os_debug_yield()
#else
#   define OS_ATOMIC_DEBUG_YIELD  (void)0
#endif

/* we have suitable set for native atomic operations */
C_INLINE_DECL os_AtomicValue
os_atomic_fetch( const os_Atomic * ptr )
{
    os_AtomicValue rc;

    OS_ATOMIC_DEBUG_YIELD;
    rc = os_atomic_native_fetch( OS_ATOMIC_PTR(ptr) );
    OS_ATOMIC_DEBUG_YIELD;
    return rc;
}

C_INLINE_DECL os_AtomicValue
os_atomic_fetch_lk( const os_Atomic * ptr )
{
    os_AtomicValue rc;
    OS_ATOMIC_DEBUG_YIELD;
    rc = os_atomic_native_fetch_lk( OS_ATOMIC_PTR(ptr) );
    OS_ATOMIC_DEBUG_YIELD;
    return rc;
}

C_INLINE_DECL void
os_atomic_store( os_Atomic * ptr, os_AtomicValue value)
{
    OS_ATOMIC_DEBUG_YIELD;
    os_atomic_native_store( OS_ATOMIC_PTR(ptr), value );
    OS_ATOMIC_DEBUG_YIELD;
}

C_INLINE_DECL void
os_atomic_store_lk( os_Atomic * ptr, os_AtomicValue value)
{
    OS_ATOMIC_DEBUG_YIELD;
    os_atomic_native_store_lk( OS_ATOMIC_PTR(ptr), value );
    OS_ATOMIC_DEBUG_YIELD;
}

C_INLINE_DECL db_bool_t
os_atomic_cas( os_Atomic * ptr,
               os_AtomicValue oldval,
               os_AtomicValue newval )
{
    db_bool_t rc;

    OS_ATOMIC_DEBUG_YIELD;
    rc = os_atomic_native_cas( OS_ATOMIC_PTR( ptr ), oldval, newval );
    OS_ATOMIC_DEBUG_YIELD;
    return rc;
}

C_INLINE_DECL os_AtomicValue
os_atomic_inc( os_Atomic * ptr )
{
    os_AtomicValue rc;

    OS_ATOMIC_DEBUG_YIELD;
    rc = os_atomic_native_inc( OS_ATOMIC_PTR( ptr ) );
    OS_ATOMIC_DEBUG_YIELD;
    return rc;
}

C_INLINE_DECL os_AtomicValue
os_atomic_dec( os_Atomic * ptr )
{
    os_AtomicValue rc;

    OS_ATOMIC_DEBUG_YIELD;
    rc = os_atomic_native_dec( OS_ATOMIC_PTR( ptr ) );
    OS_ATOMIC_DEBUG_YIELD;
    return rc;
}

C_INLINE_DECL os_AtomicValue
os_atomic_add( os_Atomic * ptr, os_AtomicValue addval )
{
    os_AtomicValue rc;

    OS_ATOMIC_DEBUG_YIELD;
    rc = os_atomic_native_add( OS_ATOMIC_PTR( ptr ), addval );
    OS_ATOMIC_DEBUG_YIELD;
    return rc;
}

C_INLINE_DECL os_AtomicValue
os_atomic_exch( os_Atomic * ptr, os_AtomicValue newval )
{
    os_AtomicValue rc;

    OS_ATOMIC_DEBUG_YIELD;
    rc = os_atomic_native_exch( OS_ATOMIC_PTR( ptr ), newval );
    OS_ATOMIC_DEBUG_YIELD;
    return rc;
}

C_INLINE_DECL db_bool_t
os_atomic_trylock( os_Atomic * ptr )
{
    db_bool_t rc;

    OS_ATOMIC_DEBUG_YIELD;
    rc = os_atomic_native_trylock( OS_ATOMIC_PTR( ptr ) );
    OS_ATOMIC_DEBUG_YIELD;
    return rc;
}

C_INLINE_DECL void
os_atomic_unlock( os_Atomic * ptr )
{
    OS_ATOMIC_DEBUG_YIELD;
    os_atomic_native_unlock( OS_ATOMIC_PTR( ptr ) );
    OS_ATOMIC_DEBUG_YIELD;
}
#else /* OS_HAVE_ATOMIC_FETCH ... */

#undef OS_HAVE_ATOMIC_NATIVE

/* approximate using locks or mutexes */
#ifndef OS_HAVE_ATOMIC_VALUE
#   define OS_HAVE_ATOMIC_VALUE
#   define OS_ATOMIC_NATIVE_INIT(x) (x)
#   define OS_ATOMIC_NATIVE_CTOR(x) (x)
    typedef int os_AtomicValue;
#endif

#define os_atomic_fetch_lk( ptr )           os_atomic_native_fetch_lk( OS_ATOMIC_PTR(ptr) )
#define os_atomic_store_lk( ptr, newval )   os_atomic_native_store_lk(OS_ATOMIC_PTR(ptr),  newval)

os_AtomicValue os_atomic_fetch( const os_Atomic *);
void os_atomic_store(os_Atomic * ptr, os_AtomicValue newval);
os_AtomicValue os_atomic_inc( os_Atomic * ptr);
os_AtomicValue os_atomic_dec( os_Atomic * ptr );
os_AtomicValue os_atomic_add( os_Atomic * ptr, os_AtomicValue addval);
db_bool_t os_atomic_cas( os_Atomic *, os_AtomicValue oldval, os_AtomicValue newval );
os_AtomicValue os_atomic_exch( os_Atomic * ptr, os_AtomicValue newval );
db_bool_t os_atomic_trylock( os_Atomic * ptr );
void os_atomic_unlock( os_Atomic * ptr );

#endif /* OS_HAVE_ATOMIC_FETCH ... */

/* easy locked access set (no barries) */
C_INLINE_DECL os_AtomicValue
os_atomic_add_lk( os_Atomic * ptr, os_AtomicValue addval )
{
    os_AtomicValue curval = os_atomic_fetch_lk( ptr );
    os_AtomicValue newval = curval + addval;
    os_atomic_store_lk( ptr, newval );
    return newval;
}

C_INLINE_DECL os_AtomicValue
os_atomic_inc_lk( os_Atomic * ptr )
{
    os_AtomicValue newval = os_atomic_fetch_lk( ptr ) + 1;
    os_atomic_store_lk( ptr, newval);
    return newval;
}

C_INLINE_DECL os_AtomicValue
os_atomic_dec_lk( os_Atomic * ptr )
{
    os_AtomicValue newval = os_atomic_fetch_lk( ptr ) - 1;
    os_atomic_store_lk( ptr, newval);
    return newval;
}

C_INLINE_DECL db_bool_t
os_atomic_cas_lk( os_Atomic * ptr, os_AtomicValue oldval, os_AtomicValue newval )
{
    os_AtomicValue curval = os_atomic_fetch_lk( ptr );
    if (curval == oldval)
        os_atomic_store_lk( ptr, newval);
    return curval == oldval;
}

C_INLINE_DECL os_AtomicValue
os_atomic_exch_lk( os_Atomic * ptr, os_AtomicValue newval )
{
    os_AtomicValue curval = os_atomic_fetch_lk( ptr );
    os_atomic_store_lk( ptr, newval);
    return curval;
}

#define os_atomic_unlock_lk(ptr)    os_atomic_native_unlock_lk(ptr)

#endif /* 0 - omitted part */

#endif /* ATOMIC_H */
