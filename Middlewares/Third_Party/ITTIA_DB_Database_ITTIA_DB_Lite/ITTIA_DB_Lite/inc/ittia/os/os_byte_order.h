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
 * Byte order settings
 *
 * Including this file defines HOST_BYTE_ORDER as one of:
 * - BIG_ENDIAN
 * - LITTLE_ENDIAN
 *
 * If BYTE_ORDER or HOST_BYTE_ORDER is already set, that value is used for
 * HOST_BYTE_ORDER instead.
 */

#ifndef OS_BYTE_ORDER_H
#define OS_BYTE_ORDER_H

#include "ittia/os/os_config.h"
#include "ittia/os/std/param.h"

#ifndef BIG_ENDIAN
#define BIG_ENDIAN	4321
#endif /* BIG_ENDIAN */

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN	1234
#endif /* LITTLE_ENDIAN */

/*
 * byte order selection
 */

/* GHS and QNX define __BIG_ENDIAN__ and __LITTLE_ENDIAN__ */
#if defined(__BIG_ENDIAN__)
#define HOST_BYTE_ORDER BIG_ENDIAN
#elif defined(__LITTLE_ENDIAN__)
#define HOST_BYTE_ORDER LITTLE_ENDIAN
#endif

/* Android defines _BYTE_ORDER */
#if defined(_BYTE_ORDER) && defined(_BIG_ENDIAN) && defined(_LITTLE_ENDIAN)
#if _BYTE_ORDER == _LITTLE_ENDIAN
#define HOST_BYTE_ORDER LITTLE_ENDIAN
#endif
#if _BYTE_ORDER == _BIG_ENDIAN
#define HOST_BYTE_ORDER BIG_ENDIAN
#endif
#endif

#if !defined(HOST_BYTE_ORDER) && defined(BYTE_ORDER)
#define HOST_BYTE_ORDER BYTE_ORDER
#endif /* BYTE_ORDER */

#if !defined(HOST_BYTE_ORDER) && ( \
    defined(_M_IX86) \
    || defined(_X86_) \
    || defined(i386) \
    || defined(__i386) \
    || defined(__i386__) \
    || defined(__386) \
    || defined(ARMEL) \
    || defined(__ADSPBLACKFIN__) \
    )

#define HOST_BYTE_ORDER LITTLE_ENDIAN
#endif /* _M_IX86 */

#if !defined(HOST_BYTE_ORDER) && ( \
    defined(ARMEB) \
    )

#define HOST_BYTE_ORDER BIG_ENDIAN
#endif /* ARMEB */

#ifndef HOST_BYTE_ORDER
#error unknown host byte order
#endif

#if HOST_BYTE_ORDER == LITTLE_ENDIAN
#define BYTE_ORDER_STRUCT(h, l) struct { l; h; }
#else
#define BYTE_ORDER_STRUCT(h, l) struct { h; l; }
#endif

#endif /* OS_BYTE_ORDER_H */
