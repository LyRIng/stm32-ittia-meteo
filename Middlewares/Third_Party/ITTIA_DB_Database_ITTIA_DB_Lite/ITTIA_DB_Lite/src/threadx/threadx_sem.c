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

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) && defined(HAVE_THREADX_THREADS)

#include "ittia/os/os_error.h"
#include "ittia/os/os_sem.h"
#include "os/threadx/threadx.h"

#include "os/os_mockup.h"


DBDLL_API int os_sem_init(os_sem_t * sem, int navailable)
{
    UINT status;
    if ((status = tx_semaphore_create(sem, "", navailable)) != TX_SUCCESS)
        return os_threadx_error(status);
    return DB_NOERROR;
}

DBDLL_API int os_sem_destroy(os_sem_t * sem)
{
    UINT status;
    if ((status = tx_semaphore_delete(sem)) != TX_SUCCESS)
        return os_threadx_error(status);
    return DB_NOERROR;
}

DBDLL_API int os_sem_wait(os_sem_t * sem, os_wait_time_t time)
{
    UINT status;
    if ((status = tx_semaphore_get(sem, time)) != TX_SUCCESS)
        return os_threadx_error(status);
    return DB_NOERROR;
}

DBDLL_API int os_sem_post(os_sem_t * sem, int how_many)
{
    while( how_many-- > 0 ) {
        UINT status;
        if ((status = tx_semaphore_put(sem)) != TX_SUCCESS)
            return os_threadx_error(status);
    }
    
    return DB_NOERROR;
}

#endif /* HAVE_THREADX_THREADS */
