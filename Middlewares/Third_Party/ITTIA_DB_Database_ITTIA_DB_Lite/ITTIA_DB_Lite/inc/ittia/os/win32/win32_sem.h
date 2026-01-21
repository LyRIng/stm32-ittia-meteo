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

#ifndef WIN32_SEM_H
#define WIN32_SEM_H

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) && defined(HAVE_WIN32_THREADS)

#include "ittia/os/std/windows.h"
#include "ittia/os/os_fastlock.h"

C_HEADER_BEGIN

#define USE_QSEM

#ifdef USE_QSEM

typedef struct
{
    int             sem_state;
    os_fastlock_t   fastlock;
    LONG            avail;  /* >0    number of resource available,
                             * <= 0  number of waiters pending
                             */
    HANDLE          hwait;
} os_sem_t;

#else

typedef struct
{
    int             sem_state;
    HANDLE          hwait;
} os_sem_t;

#endif

C_HEADER_END

#endif /* HAVE_WIN32_THREADS */

#endif /* WIN32_SEM_H */
