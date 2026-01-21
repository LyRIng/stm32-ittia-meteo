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
 * Semaphores
 */

#ifndef OS_SEM_H
#define OS_SEM_H

#include "ittia/os/os_config.h"

#ifdef HAVE_THREADS

#if defined(HAVE_WIN32_THREADS)
#   include "ittia/os/win32/win32_sem.h"
#elif defined(HAVE_VXWORKS_SEM)
#   include "ittia/os/vxworks/vxworks_sem.h"
#elif defined(HAVE_POSIX_SEM)
#   include "ittia/os/posix/posix_sem.h"
#elif defined(HAVE_FREERTOS_THREADS)
#   include "ittia/os/freertos/ittia_freertos_sem.h"
#elif defined(HAVE_THREADX_THREADS)
#   include "ittia/os/threadx/threadx_sem.h"
#elif defined(HAVE_UCOS_II_THREADS)
#   include "ittia/os/ucos_ii/ucos_ii_sem.h"
#elif defined(HAVE_UCOS_III_THREADS)
#   include "ittia/os/ucos_iii/ucos_iii_sem.h"
#else
#   include "ittia/os/generic/generic_sem.h"
#endif

#include "ittia/os/os_wait_time.h"

C_HEADER_BEGIN

DBDLL_API int os_sem_init(os_sem_t * sem, int navailable);
DBDLL_API int os_sem_destroy(os_sem_t * sem);
DBDLL_API int os_sem_wait(os_sem_t * sem, os_wait_time_t time);
DBDLL_API int os_sem_post(os_sem_t * sem, int how_many);

C_HEADER_END

#endif /* HAVE_THREADS */

#endif /* OS_SEM_H */
