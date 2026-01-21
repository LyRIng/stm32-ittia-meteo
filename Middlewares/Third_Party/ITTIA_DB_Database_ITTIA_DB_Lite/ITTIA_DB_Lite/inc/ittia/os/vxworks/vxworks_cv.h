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

#ifndef VXWORKS_CV_H
#define VXWORKS_CV_H

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) && defined(HAVE_VXWORKS_CONDVAR)

/* VxWorks condition variables can be implemented with native
 * mutexes and binary semaphores (semBCreate).  But for now, VxWorks
 * uses generic_cv.h */

#include "ittia/os/std/pthread.h"
#include "ittia/os/os_mutex.h"
#include "ittia/os/os_wait_time.h"

C_HEADER_BEGIN

typedef pthread_cond_t os_cond_t;

DBDLL_API int os_cond_init(os_cond_t*);
DBDLL_API int os_cond_destroy(os_cond_t*);
DBDLL_API int os_cond_wait(os_cond_t*, os_mutex_t*, os_wait_time_t time);
DBDLL_API int os_cond_signal(os_cond_t*);
DBDLL_API int os_cond_broadcast(os_cond_t*);

C_HEADER_END

#endif

#endif /* VXWORKS_CV_H */
