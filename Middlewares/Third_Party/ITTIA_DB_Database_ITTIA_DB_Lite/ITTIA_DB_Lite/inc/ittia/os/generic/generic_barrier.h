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

#ifndef GENERIC_BARRIER_H
#define GENERIC_BARRIER_H

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) && defined(HAVE_GENERIC_BARRIER)

#include "ittia/os/os_sem.h"
#include "ittia/os/os_atomic.h"

C_HEADER_BEGIN

typedef struct os_barrier_t os_barrier_t;
struct os_barrier_t
{
    os_sem_t entry_mutex, leave_mutex;
    os_atomic_t cur_threads, max_threads;
};

DBDLL_API int os_barrier_init(os_barrier_t*, int max_threads);
DBDLL_API int os_barrier_destroy(os_barrier_t*);
DBDLL_API int os_barrier_wait(os_barrier_t*);
/*DBDLL_API int os_barrier_release(os_barrier_t*);*/

C_HEADER_END

#endif /* HAVE_GENERIC_BARRIER */

#endif /* GENERIC_BARRIER_H */
