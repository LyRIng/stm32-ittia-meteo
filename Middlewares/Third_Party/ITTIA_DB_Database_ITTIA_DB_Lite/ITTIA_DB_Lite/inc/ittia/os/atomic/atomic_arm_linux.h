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

#ifndef ATOMIC_ARM_LINUX_H
#define ATOMIC_ARM_LINUX_H

#include "ittia/os/os_config.h"

/* Linux 2.6.x kernels provide some atomic facility on ARM.
 * see arch/arm/kernel/entry-armv.S for more info (look for
 * __kernel_cmpxchg).
 */
 
/* TODO ARMv6 has native support for atomic operations (STREX/LDREX). */

/* CHECKME we could implement all of them using ARM's pre v6 atomic SWP
 * operation while this uses kernel call. I think there are no
 * shared memory SMP systems for ARMv5 and above, so it might be much
 * faster just to deal with spin locks. Which is faster actually?
 */

#if defined(OS_CPU_ARM) && defined(OS_LINUX)

#define OS_HAVE_ATOMIC
#ifndef OS_HAVE_ATOMIC_VALUE
#define OS_HAVE_ATOMIC_VALUE
#define OS_ATOMIC_NATIVE_INIT(x) (x)
#define OS_ATOMIC_NATIVE_CTOR(x) (x)
typedef int os_AtomicValue;
#endif

#define OS_HAVE_ATOMIC_PTR_VALUE
typedef void * os_AtomicPtrValue;

C_LINKAGE typedef int (_OsArmLinuxCas)(int oldval, int newval, int * ptr);
C_LINKAGE typedef void * (_OsArmLinuxCasPtr)(void * oldval, void * newval, void ** ptr);

C_LINKAGE typedef void (_OsArmDmb)(void);

#define _OS_ARM_LINUX_CAS     (0xffff0fc0)
#define _OS_ARM_LINUX_DMB     (0xffff0fa0)

#ifndef OS_HAVE_ATOMIC_FETCH
#   define OS_HAVE_ATOMIC_FETCH     ATOMIC_IPC
#   define os_atomic_native_fetch( ptr ) (*(ptr))
#   define os_atomic_native_fetch_lk( ptr ) (*(os_AtomicValue*))(ptr))
#endif

#ifndef OS_HAVE_ATOMIC_STORE
#define OS_HAVE_ATOMIC_STORE        ATOMIC_IPC
#   define os_atomic_native_store( ptr, value) (*(ptr) = value)
#   define os_atomic_native_store_lk( ptr, value) (*(os_AtomicValue*)(ptr) = value)
#endif

#ifndef OS_HAVE_ATOMIC_CAS
#   define OS_HAVE_ATOMIC_CAS       ATOMIC_IPC
#   define os_atomic_native_cas( ptr, oldval, newval ) ((*(_OsArmLinuxCas*)_OS_ARM_LINUX_CAS)( oldval, newval, (int*)ptr ) == 0)
#endif

#ifndef OS_HAVE_ATOMIC_DMB
#   define OS_HAVE_ATOMIC_DMB       ATOMIC_IPC
#   define os_atomic_native_dmb()   (*(_OsArmDmb*)_OS_ARM_LINUX_DMB)
#endif

#endif /* OS_CPU_ARM */

#endif /* ATOMIC_ARM_LINUX_H */
