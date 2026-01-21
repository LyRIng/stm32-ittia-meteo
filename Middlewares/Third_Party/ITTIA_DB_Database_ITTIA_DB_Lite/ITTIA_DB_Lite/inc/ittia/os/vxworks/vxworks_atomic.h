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

#ifndef VXWORKS_ATOMIC_H
#define VXWORKS_ATOMIC_H

#include "ittia/os/os_config.h"

#ifdef HAVE_VXWORKS_ATOMIC

#include <vxWorks.h>
#include <vxAtomicLib.h>

C_HEADER_BEGIN

/* Currently we rely on the fact that atomic_t and atomicVal_t in vxWorks
 * are essentially the same. In the future we would transition to
 * another atomic back-end (merge with sm atomic).
 */
typedef atomic_t os_atomic_t;

/**
 * Compare and swap if equal, returning previous value.
 *
 * perform following:
 * <pre>
 * save = *p;
 * if (*p == compare) *p = new_value;
 * return save;
 * </pre>
 */

DBDLL_API int os_atomic_cas(os_atomic_t * p, os_atomic_t compare, os_atomic_t new_value);
#define os_atomic_cas(p, compare, new_value) vxAtomicCas(p, compare, new_value)

DBDLL_API int os_atomic_cas_ptr(void ** p, void * compare, void * new_value);

/**
 * add increment value to variable, return result of operation.
 */
DBDLL_API os_atomic_t os_atomic_add(os_atomic_t * p, os_atomic_t add);
/*
C_INLINE_SPECIFIER os_atomic_t
os_atomic_add(os_atomic_t * p, os_atomic_t add)
{
    return vxAtomicAdd(p, add) + add;
}
*/

/**
 * set variable to given value and return previous one
 */
DBDLL_API os_atomic_t os_atomic_exchange(os_atomic_t * p, os_atomic_t new_value);
#define os_atomic_exchange(p, new_value) vxAtomicSet(p, new_value)

/**
 * increment value by one and return resulting value.
 */
DBDLL_API os_atomic_t os_atomic_inc(os_atomic_t *);
#define os_atomic_inc(p) (vxAtomicInc(p) + 1)

/**
 * increment value by one and return resulting value.
 */
DBDLL_API os_atomic_t os_atomic_dec(os_atomic_t * p);
#define os_atomic_dec(p) (vxAtomicDec(p) - 1)

DBDLL_API os_atomic_t os_atomic_fetch(const os_atomic_t * p);
#define os_atomic_fetch(p) (vxAtomicGet((os_atomic_t *)p))

DBDLL_API void os_atomic_store(os_atomic_t * p, os_atomic_t value);
#define os_atomic_store(p, value) ((void)vxAtomicSet(p, value))

C_HEADER_END

#endif

#endif
