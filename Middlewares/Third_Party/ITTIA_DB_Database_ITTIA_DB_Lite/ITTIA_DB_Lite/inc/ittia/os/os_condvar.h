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
 * Condition variables
 */

#ifndef OS_CONDVAR_H
#define OS_CONDVAR_H

#include "ittia/os/os_config.h"

#ifdef HAVE_THREADS

#if defined(HAVE_VXWORKS_CONDVAR)
#   include "ittia/os/vxworks/vxworks_cv.h"
#elif defined(HAVE_POSIX_CONDVAR)
#   include "ittia/os/posix/posix_cv.h"
#elif defined(HAVE_GENERIC_CONDVAR)
#   include "ittia/os/generic/generic_cv.h"
#endif 


C_HEADER_BEGIN

DBDLL_API int os_cond_init(os_cond_t*);
DBDLL_API int os_cond_destroy(os_cond_t*);
DBDLL_API int os_cond_wait(os_cond_t*, os_mutex_t*, os_wait_time_t time);
DBDLL_API int os_cond_signal(os_cond_t*);
DBDLL_API int os_cond_broadcast(os_cond_t*);

C_HEADER_END

#endif /* HAVE_THREADS */

#endif /* OS_CONDVAR_H */
