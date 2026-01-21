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

#ifndef ATOMIC_GCC4_H
#define ATOMIC_GCC4_H

#include "ittia/os/os_debug.h"

/* === gcc 4.1 builtin atomic support === */

/* - excluding ARM: mine version of ARM gcc doesn't support SHM safe
 *   atomics, so use generic implementation
 *
 * - atomics were introduced in gcc 4.1. assume next versions would
 * continue to support them
 */

#if defined(__GNUC__) && !defined(OS_CPU_ARM) && !defined(OS_VXWORKS) \
    && ((__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1))


#define OS_HAVE_ATOMIC

#ifndef OS_HAVE_ATOMIC_VALUE

#define OS_HAVE_ATOMIC_VALUE
#define OS_ATOMIC_NATIVE_INIT(x) (x)
#define OS_ATOMIC_NATIVE_CTOR(x) (x)
typedef int    os_AtomicValue;

#endif /* OS_HAVE_ATOMIC_VALUE */

#ifndef OS_HAVE_ATOMIC_PTR_VALUE
#define OS_HAVE_ATOMIC_PTR_VALUE
typedef void * os_AtomicPtrValue;
#endif /* OS_HAVE_ATOMIC_PTR_VALUE */

#ifdef DB_DEBUG
C_INLINE_DECL os_AtomicPtrValue _os_atomic_ptr_check(os_AtomicPtrValue x, os_AtomicPtrValue x1) { DB_ASSERT(x == x1); return x; }
C_INLINE_DECL os_AtomicValue    _os_atomic_int_check(os_AtomicValue x, os_AtomicValue x1) { DB_ASSERT(x == x1); return x; }
#   define  _os_atomic_ptr(x) _os_atomic_ptr_check(x,x)
#   define  _os_atomic_int(x) _os_atomic_int_check(x,x)
#else
#   define  _os_atomic_ptr(x) (x)
#   define  _os_atomic_int(x) (x)
#endif


/* TODO we cannot guess if gcc supports sync primitives without
 * linking, so it's a future task for configure.
 * assume gcc supports SMP implementation
 *
 * Keep this header allowing to complement native implementation.
 *
 * === Specific notes ===
 *
 * CodeSourcery: gcc (Sourcery G++ Lite 2010q1-188) 4.4.1
 *      always generate stubs regardles the -march given
 *
 * Mingw32: gcc 4.4.0
 *      generates SMP code
 *      doesn't define any of __GCC_HAVE_SYNC_COMPARE_AND_SWAP_n
 *
 * Debian 5.0: gcc 4.3.2
 *      generates SMP code
 *      define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_{1|2|4}
 *
 */

#ifndef OS_HAVE_ATOMIC_FETCH
#   define OS_HAVE_ATOMIC_FETCH         ATOMIC_IPC
#   define os_atomic_native_fetch(ptr) (*(ptr))
#   define os_atomic_native_fetch_lk(ptr) (*(os_AtomicValue*)(ptr))
#endif

#ifndef OS_HAVE_ATOMIC_STORE
#   define OS_HAVE_ATOMIC_STORE         ATOMIC_IPC
    /* TODO this cannot be true for any processorm replace with something like __sync_add_and_fetch?  */
#   define os_atomic_native_store(ptr, newval) (*(ptr) = (newval))
#   define os_atomic_native_store_lk(ptr, newval) (*(os_AtomicValue*)(ptr) = (newval))
#endif

#ifndef OS_HAVE_ATOMIC_CAS
#   define OS_HAVE_ATOMIC_CAS           ATOMIC_IPC
#   define os_atomic_native_cas(ptr, oldval, newval) __sync_bool_compare_and_swap(ptr, oldval, newval)
#endif

#ifndef OS_HAVE_ATOMIC_ADD
#   define OS_HAVE_ATOMIC_ADD           ATOMIC_IPC
#   define os_atomic_native_add(ptr, addval) __sync_add_and_fetch(ptr, addval)
#endif

#ifndef OS_HAVE_ATOMIC_INC
#   define OS_HAVE_ATOMIC_INC           ATOMIC_IPC
#   define os_atomic_native_inc(ptr)    __sync_add_and_fetch(ptr, 1)
#endif

#ifndef OS_HAVE_ATOMIC_DEC
#   define OS_HAVE_ATOMIC_DEC           ATOMIC_IPC
#   define os_atomic_native_dec(ptr)    __sync_add_and_fetch(ptr, -1)
#endif

#ifndef OS_HAVE_ATOMIC_TRYLOCK
#   define OS_HAVE_ATOMIC_TRYLOCK       ATOMIC_IPC
#   define os_atomic_native_trylock(ptr) (__sync_lock_test_and_set(ptr, 1) != 1)
/* todo it's not clear if unlock should contain membar or not */
#   define os_atomic_native_unlock(ptr)  (__sync_lock_release(ptr))
#   define os_atomic_native_unlock_lk(ptr)  (_os_atomic_native_unlock(ptr))
#endif

#ifndef OS_HAVE_ATOMIC_DMB
#   define OS_HAVE_ATOMIC_DMB           ATOMIC_IPC
#   define os_atomic_native_dmb()  __sync_synchronize()
#endif

#ifndef OS_HAVE_ATOMIC_CAS_PTR
#   define OS_HAVE_ATOMIC_CAS_PTR       ATOMIC_IPC
#   define os_atomic_native_cas_ptr(ptr, oldval, newval) __sync_bool_compare_and_swap(ptr, _os_atomic_ptr(oldval), _os_atomic_ptr(newval))
#endif

#endif /* is GCC 4.x */

#endif /* ATOMIC_GCC4_H */
