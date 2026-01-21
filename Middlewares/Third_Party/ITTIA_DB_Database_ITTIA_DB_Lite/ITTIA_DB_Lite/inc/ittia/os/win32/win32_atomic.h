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

#ifndef WIN32_ATOMIC_H
#define WIN32_ATOMIC_H

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) && defined(HAVE_WIN32_ATOMIC)

#define OS_HAVE_ATOMIC
#define OS_HAVE_ATOMIC_VALUE

#include "ittia/os/std/windows.h"

C_HEADER_BEGIN

typedef LONG os_atomic_t;

/**
 * Some windows versions declare InterlockedCompareExchange with
 * PVOID some with LONG. So we have to support both. The same with
 * InterlockedCompareExchangePointer, which is undefined in MSVC 6.0
 * headers.
 */
typedef LONG   (WINAPI * WIN32_CAS_INT)(LONG  volatile * p, LONG n, LONG c);
typedef PVOID  (WINAPI * WIN32_CAS_PTR)(PVOID volatile * p, PVOID n, PVOID c);

/**
 * Compare and swap if equal, returning true if equal.
 *
 * perform following:
 * <pre>
 * save = *p;
 * if (*p == compare) *p = new_value;
 * return save == compare;
 * </pre>
 */

DBDLL_API int os_atomic_cas(os_atomic_t * p, os_atomic_t compare, os_atomic_t new_value);

DBDLL_API int os_atomic_cas_ptr(void ** p, void * compare, void * new_value);

/**
 * add increment value to variable, return result of operation.
 */
DBDLL_API os_atomic_t os_atomic_add(os_atomic_t * p, os_atomic_t add);
/*
C_INLINE_SPECIFIER os_atomic_t
os_atomic_add(os_atomic_t * p, os_atomic_t add)
{
    return (os_atomic_t)InterlockedExchangeAdd(p, add) + add;
}
*/

/**
 * set variable to given value and return previous one
 */
DBDLL_API os_atomic_t os_atomic_exchange(os_atomic_t * p, os_atomic_t new_value);
#define os_atomic_exchange(p, new_value) (os_atomic_t)InterlockedExchange(p, new_value)

/*
 * meaning of the InterlockedIncrement and InterlockedDecrement has
 * changed since win95/NT 3.51.
 * Despite these are quite outdated now, we don't want to run into
 * troubles.
 */

/**
 * increment value by one and return resulting value.
 */
DBDLL_API os_atomic_t os_atomic_inc(os_atomic_t *);
#define os_atomic_inc(p) os_atomic_add(p, 1)

/**
 * decrement value by one and return resulting value.
 */
DBDLL_API os_atomic_t os_atomic_dec(os_atomic_t * p);
#define os_atomic_dec(p) os_atomic_add(p, -1)

DBDLL_API os_atomic_t os_atomic_fetch(const os_atomic_t * p);
#define os_atomic_fetch(p) (*(p))

DBDLL_API void os_atomic_store(os_atomic_t * p, os_atomic_t value);
#define os_atomic_store(p, value) (*(p) = (value))

C_HEADER_END

#endif /* HAVE_WIN32_ATOMIC */

#endif /* WIN32_ATOMIC_H */
