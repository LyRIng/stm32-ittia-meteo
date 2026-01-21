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
 */

#ifndef OS_TLS_H
#define OS_TLS_H

#include "ittia/os/os_config.h"

#ifdef HAVE_THREADS

#ifdef HAVE_WIN32_THREADS
#   include "ittia/os/win32/win32_tls.h"
#elif defined(HAVE_POSIX_TLS)
#   include "ittia/os/posix/posix_tls.h"
#elif defined(HAVE_UCOS_II_TLS)
#   include "ittia/os/ucos_ii/ucos_ii_tls.h"
#elif defined(HAVE_UCOS_III_TLS)
#   include "ittia/os/ucos_iii/ucos_iii_tls.h"
#elif defined(HAVE_GENERIC_TLS)
#   include "ittia/os/generic/generic_tls.h"
#endif /* HAVE_POSIX_TLS */

C_HEADER_BEGIN

DBDLL_API int os_tls_init(os_tls_key_t * key);
DBDLL_API int os_tls_destroy(os_tls_key_t * key);
DBDLL_API int os_tls_set(os_tls_key_t, os_tls_value_t);
DBDLL_API int os_tls_get(os_tls_key_t, os_tls_value_t *);

C_HEADER_END

#endif /* HAVE_THREADS */

#endif /* OS_TLS_H */
