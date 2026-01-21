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

#ifndef GENERIC_CV_H
#define GENERIC_CV_H

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) && defined(HAVE_GENERIC_CONDVAR)

#include "ittia/os/os_sem.h"
#include "ittia/os/os_mutex.h"

C_HEADER_BEGIN

typedef struct os_cond_t os_cond_t;

struct os_cond_t
{
    int             state;
    struct os_cond_t * self;

    os_mutex_t leave_lock;
    os_sem_t   entry_lock;
    os_sem_t   queue;

    /* counters */
    int        gone;
    int        sleeping;
    int        wakeup;
};

C_HEADER_END

#endif

#endif /* GENERIC_CV_H */
