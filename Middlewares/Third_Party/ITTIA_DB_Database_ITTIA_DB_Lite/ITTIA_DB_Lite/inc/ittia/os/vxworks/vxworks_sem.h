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

#ifndef VXWORKS_SEM_H
#define VXWORKS_SEM_H

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) && defined(HAVE_VXWORKS_SEM)

#include "ittia/os/std/semaphore.h"
#include "ittia/os/os_wait_time.h"

C_HEADER_BEGIN

typedef SEM_ID os_sem_t;

DBDLL_API int os_sem_init(os_sem_t * sem, int navailable);
DBDLL_API int os_sem_destroy(os_sem_t * sem);
DBDLL_API int os_sem_wait(os_sem_t * sem, os_wait_time_t time);
DBDLL_API int os_sem_post(os_sem_t * sem, int how_many);

C_HEADER_END

#endif

#endif /* VXWORKS_SEM_H */
