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

#include "ittia/os/os_mutex.h"
#include "ittia/os/os_error.h"
#include "os/threadx/threadx.h"
#include "ittia/os/os_debug.h"

DBDLL_API int os_mutex_init(os_mutex_t * mutex)
{
    UINT status;

    /* TODO: should TX_INHERIT be used to prevent deadlocks? */
    if ((status = tx_mutex_create(&mutex->mtx, "", TX_NO_INHERIT)) != TX_SUCCESS)
        return os_threadx_error(status);

    return DB_NOERROR;
}

DBDLL_API int os_mutex_destroy(os_mutex_t * mutex)
{
    UINT status;

    if ((status = tx_mutex_delete(&mutex->mtx)) != TX_SUCCESS)
        return os_threadx_error(status);
    return DB_NOERROR;
}

DBDLL_API int os_mutex_trylock(os_mutex_t * mutex)
{
    UINT status;
    if ((status = tx_mutex_get(&mutex->mtx, TX_NO_WAIT)) != TX_SUCCESS)
        return os_threadx_error(status);
    return DB_NOERROR;
}

DBDLL_API int os_mutex_lock(os_mutex_t * mutex)
{
    UINT status;
    if ((status = tx_mutex_get(&mutex->mtx, TX_WAIT_FOREVER)) != TX_SUCCESS)
        return os_threadx_error(status);
    return DB_NOERROR;
}

DBDLL_API int os_mutex_unlock(os_mutex_t * mutex)
{
    UINT status;
    if ((status = tx_mutex_put(&mutex->mtx)) != TX_SUCCESS)
        return os_threadx_error(status);
    return DB_NOERROR;
}

#endif /* HAVE_THREADX_THREADS */
