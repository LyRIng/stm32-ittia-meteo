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

#ifndef GENERIC_ATOMIC_H
#define GENERIC_ATOMIC_H

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) && defined(HAVE_GENERIC_ATOMIC)

#include "ittia/os/atomic/atomic.h"

C_HEADER_BEGIN

#if defined(OS_HAVE_ATOMIC) \
    && defined(OS_HAVE_ATOMIC_FETCH)    \
    && defined(OS_HAVE_ATOMIC_STORE)    \
    && defined(OS_HAVE_ATOMIC_CAS)      \
    && defined(OS_HAVE_ATOMIC_INC)      \
    && defined(OS_HAVE_ATOMIC_DEC)      \
    && defined(OS_HAVE_ATOMIC_ADD)      \
    && defined(OS_HAVE_ATOMIC_EXCH)

typedef os_AtomicValue os_atomic_t;

#define os_atomic_fetch(x)          os_atomic_native_fetch(x)
#define os_atomic_store(x,v)        os_atomic_native_store(x,v)
#define os_atomic_inc(x)            os_atomic_native_inc(x)
#define os_atomic_dec(x)            os_atomic_native_dec(x)
#define os_atomic_add(x,v)          os_atomic_native_add(x,v)
#define os_atomic_cas(x,c,v)        os_atomic_native_cas(x,c,v)
#define os_atomic_exchange(x,v)     os_atomic_native_exch(x,v)

#else

typedef int os_atomic_t;

/**
 * Compare and swap if equal, returning true if values were swapped.
 */

DBDLL_API int os_atomic_cas(os_atomic_t * p, os_atomic_t old_value, os_atomic_t new_value);

/**
 * add increment value to variable, return result of operation.
 */
DBDLL_API os_atomic_t os_atomic_add(os_atomic_t * p, os_atomic_t add);

/**
 * set variable to given value and return previous one
 */
DBDLL_API os_atomic_t os_atomic_exchange(os_atomic_t * p, os_atomic_t new_value);

/**
 * increment value by one and return resulting value.
 */
DBDLL_API os_atomic_t os_atomic_inc(os_atomic_t *);

/**
 * decrement value by one and return resulting value.
 */
DBDLL_API os_atomic_t os_atomic_dec(os_atomic_t * p);

/**
 * fetch value of variable
 */
DBDLL_API os_atomic_t os_atomic_fetch(const os_atomic_t * p);

/**
 * store value in the variable
 */
DBDLL_API void os_atomic_store(os_atomic_t * p, os_atomic_t value);

#endif /* OS_HAVE_ATOMIC */

#if defined(OS_HAVE_ATOMIC) && defined(OS_HAVE_ATOMIC_CAS_PTR)
#define os_atomic_cas_ptr(x,c,v)    os_atomic_native_cas_ptr(x,c,v)
#else
DBDLL_API int os_atomic_cas_ptr(void ** p, void * old_value, void * new_value);
#endif

C_HEADER_END

#endif /* HAVE_GENERIC_ATOMIC */

#endif /* GENERIC_ATOMIC_H */
