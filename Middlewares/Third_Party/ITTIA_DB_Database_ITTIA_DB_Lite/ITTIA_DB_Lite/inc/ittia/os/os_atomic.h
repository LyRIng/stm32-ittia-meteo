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

/* @file
 *
 * Atomic operations
 */

#ifndef OS_ATOMIC_H
#define OS_ATOMIC_H

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS)

#ifdef HAVE_WIN32_ATOMIC
#   include "ittia/os/win32/win32_atomic.h"
#endif

#ifdef HAVE_GENERIC_ATOMIC
#   include "ittia/os/generic/generic_atomic.h"
#endif

#ifdef HAVE_VXWORKS_ATOMIC
#   include "ittia/os/vxworks/vxworks_atomic.h"
#endif

#else

/* single threaded wrapper */

#if defined(OS_WIN32)

typedef long os_atomic_t;

#else

typedef int os_atomic_t;

#endif

#define os_atomic_fetch(x)   (*(x))
#define os_atomic_store(x,v) ((*(x)) = (v))
#define os_atomic_inc(x)     (++(*(x)))
#define os_atomic_dec(x)     (--(*(x)))
#define os_atomic_add(x,v)   ((*x) += (v))
#define os_atomic_cas(x,c,v) ((*(x) == (c)) ? (*(x) = (v)), 1 : 0)
#define os_atomic_cas_ptr(x,c,v) os_atomic_cas(x,c,v)

#endif /* HAVE_THREADS */

#endif /* OS_ATOMIC_H */
