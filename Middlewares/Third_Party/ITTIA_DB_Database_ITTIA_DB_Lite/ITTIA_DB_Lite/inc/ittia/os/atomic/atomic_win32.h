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

#ifndef ATOMIC_WIN32_H
#define ATOMIC_WIN32_H

#ifdef OS_WIN32

#include "ittia/os/std/windows.h"

#define OS_HAVE_ATOMIC
#define OS_HAVE_ATOMIC_VALUE

#define OS_ATOMIC_NATIVE_INIT(x) (x)
#define OS_ATOMIC_NATIVE_CTOR(x) (x)
typedef signed long os_AtomicValue;

C_LINKAGE typedef LONG  (WINAPI * InterlockedCompareExchangeInt)(LONG volatile * ptr, LONG, LONG);
C_LINKAGE typedef PVOID (WINAPI * InterlockedCompareExchangePtr)(PVOID * ptr, PVOID, PVOID);

// CE has InterlockedTestExchange, equiv to InterlockedCompareExchange

#define OS_HAVE_ATOMIC_FETCH    ATOMIC_IPC
#define os_atomic_native_fetch( ptr ) (*(ptr))
#define os_atomic_native_fetch_lk( ptr ) (*(os_AtomicValue*)(ptr))

#define OS_HAVE_ATOMIC_STORE    ATOMIC_IPC
#define os_atomic_native_store( ptr, value) (*(ptr) = value)
#define os_atomic_native_store_lk( ptr, value) (*(os_AtomicValue*)(ptr) = value)

#define OS_HAVE_ATOMIC_CAS      ATOMIC_IPC

C_INLINE_DECL db_bool_t
os_atomic_native_cas(os_AtomicValue * ptr,
                     os_AtomicValue oldval,
                     os_AtomicValue newval)
{
#if defined(InterlockedCompareExchange)
	return InterlockedCompareExchange( ptr, newval, oldval ) == oldval;
#else
	static InterlockedCompareExchangeInt fun = (InterlockedCompareExchangeInt)&InterlockedCompareExchange;
	return fun( ptr, newval, oldval ) == oldval;
#endif
}

#define OS_HAVE_ATOMIC_INC      ATOMIC_IPC
#define os_atomic_native_inc( ptr )   InterlockedIncrement( (PLONG)ptr )

#define OS_HAVE_ATOMIC_DEC      ATOMIC_IPC
#define os_atomic_native_dec( ptr )   InterlockedDecrement( (PLONG)ptr )

#define OS_HAVE_ATOMIC_ADD      ATOMIC_IPC
C_INLINE_DECL os_AtomicValue
os_atomic_native_add( os_AtomicValue * ptr, os_AtomicValue addval )
{
    return InterlockedExchangeAdd( (PLONG)ptr, addval ) + addval;
}

#define OS_HAVE_ATOMIC_EXCH     ATOMIC_IPC
#define os_atomic_native_exch( ptr, newval ) InterlockedExchange( (PLONG)ptr, newval )

#if 0
/* Memory barriers are not currently used. */
#define OS_HAVE_ATOMIC_DMB
C_INLINE_DECL void
os_atomic_native_dmb()
{
    LONG x;
    
    InterlockedExchange(&x, 0);
    #ifdef __GNUC__
    __asm__ __volatile__("" ::: "memory");
    #elif defined(_MSC_VER) && defined(OS_CPU_X86)
	/* TODO this needs some investigation. I just assume that due to
	* pure ASM syntax in MSVC, it would not reorder instructions
	* over the asm statement
	*/
    __asm nop ;
    #else
    #error "unknown compiler"
    #endif
}
#endif

#define OS_HAVE_ATOMIC_TRYLOCK  ATOMIC_IPC
#define os_atomic_native_trylock(ptr)    (InterlockedExchange((PLONG)ptr, 1) == 0)
#define os_atomic_native_unlock(ptr)     (InterlockedExchange((PLONG)ptr, 0))
#define os_atomic_native_unlock_lk(ptr)  os_atomic_native_unlock(ptr)

#define OS_HAVE_ATOMIC_PTR_VALUE
typedef void * os_AtomicPtrValue;

#define OS_HAVE_ATOMIC_FETCH_PTR    ATOMIC_IPC
#define os_atomic_native_fetch_ptr( ptr ) (*(ptr))
#define os_atomic_native_fetch_ptr_lk( ptr ) (*(os_AtomicValue*)(ptr))

#define OS_HAVE_ATOMIC_STORE_PTR    ATOMIC_IPC
#define os_atomic_native_store_ptr( ptr, value) (*(ptr) = value)
#define os_atomic_native_store_ptr_lk( ptr, value) (*(os_AtomicValue*)(ptr) = value)

#define OS_HAVE_ATOMIC_CAS_PTR      ATOMIC_IPC
#define os_atomic_native_cas_ptr( ptr, oldval, newval ) (InterlockedCompareExchangePointer( (PVOID)ptr, newval, oldval ) == oldval)

#define OS_HAVE_ATOMIC_EXCH_PTR     ATOMIC_IPC
#ifdef OS_WINCE
#   define os_atomic_native_exch_ptr( ptr, newval ) InterlockedExchange( (PLONG)ptr, (LONG)newval )
#else
#   define os_atomic_native_exch_ptr( ptr, newval ) InterlockedExchangePointer( (PVOID)ptr, newval )
#endif

#endif

#endif
