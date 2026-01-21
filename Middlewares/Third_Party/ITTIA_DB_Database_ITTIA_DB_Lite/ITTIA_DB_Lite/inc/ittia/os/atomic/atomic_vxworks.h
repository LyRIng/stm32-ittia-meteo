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

#ifndef ATOMIC_VXWORKS_H
#define ATOMIC_VXWORKS_H

#ifdef OS_VXWORKS

#include <vxWorks.h>
#include <vxAtomicLib.h>


#define OS_HAVE_ATOMIC
#define OS_HAVE_ATOMIC_VALUE

#define OS_ATOMIC_NATIVE_INIT(x) (x)
#define OS_ATOMIC_NATIVE_CTOR(x) (x)
typedef atomic_t os_AtomicValue;

#define OS_HAVE_ATOMIC_FETCH    ATOMIC_IPC
#define os_atomic_native_fetch( ptr ) (*(ptr))
#define os_atomic_native_fetch_lk( ptr ) (*(os_AtomicValue*)(ptr))

#define OS_HAVE_ATOMIC_STORE    ATOMIC_IPC
#define os_atomic_native_store( ptr, value) vxAtomicSet(ptr, value)
#define os_atomic_native_store_lk( ptr, value) (*(os_AtomicValue*)(ptr) = value)

#define OS_HAVE_ATOMIC_CAS      ATOMIC_IPC
#if defined(OS_VXWORKS_6x) && OS_VXWORKS_6x >= 4
#define os_atomic_native_cas( ptr, oldval, newval ) vxAtomicCas(ptr, oldval, newval)
#else
#define os_atomic_native_cas( ptr, oldval, newval ) vxCas(ptr, oldval, newval)
#endif

#define OS_HAVE_ATOMIC_INC      ATOMIC_IPC
#define os_atomic_native_inc( ptr )   (vxAtomicInc( (atomic_t*)ptr ) + 1)

#define OS_HAVE_ATOMIC_DEC      ATOMIC_IPC
#define os_atomic_native_dec( ptr )   (vxAtomicDec( (atomic_t*)ptr ) - 1)

#define OS_HAVE_ATOMIC_ADD      ATOMIC_IPC
C_INLINE_DECL os_AtomicValue
os_atomic_native_add( os_AtomicValue * ptr, os_AtomicValue addval )
{
    return vxAtomicAdd( (atomic_t*)ptr, addval ) + addval;
}

#define OS_HAVE_ATOMIC_EXCH     ATOMIC_IPC
#define os_atomic_native_exch( ptr, newval ) vxAtomicSet( (atomic_t*)ptr, newval )

#define OS_HAVE_ATOMIC_TRYLOCK  ATOMIC_IPC
#define os_atomic_native_trylock(ptr)    (vxAtomicSet((atomic_t*)ptr, 1) == 0)
#define os_atomic_native_unlock(ptr)     ((void)vxAtomicSet((atomic_t*)ptr, 0))
#define os_atomic_native_unlock_lk(ptr)  (*(ptr) = 0)

#endif /* OS_VXWORKS */

#endif
