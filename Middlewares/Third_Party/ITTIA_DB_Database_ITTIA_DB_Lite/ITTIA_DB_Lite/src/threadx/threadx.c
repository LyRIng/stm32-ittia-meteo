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

#if defined(OS_THREADX)

#include "ittia/os/os_error.h"
#include "ittia/os/os_debug.h"
#include "ittia/os/std/windows.h"
#include "os/threadx/threadx.h"
#include "ittia/os/std/errno.h"
#include "ittia/os/os_dump.h"

#include "os/os_mockup.h"

static int _os_threadx_error(UINT status)
{
	switch(status) {
    case TX_SUCCESS:
      return DB_NOERROR;
    case TX_THREAD_ERROR:
      return DB_ENOTHREAD;
    case TX_NO_INSTANCE: /* Could not obtain semaphore instance */
    case TX_NOT_AVAILABLE: /* Could not obtain mutex ownership */
      return DB_ELOCKED;
      
    case TX_PTR_ERROR:
    case TX_SIZE_ERROR:
    case TX_PRIORITY_ERROR:
    case TX_THRESH_ERROR:
    case TX_START_ERROR:
    case TX_CALLER_ERROR:
    case TX_INHERIT_ERROR:
    case TX_MUTEX_ERROR:
    case TX_DELETED:
    case TX_WAIT_ABORTED:
    case TX_WAIT_ERROR:
    case TX_NOT_OWNED:
    case TX_SEMAPHORE_ERROR:
	default:
	  return DB_EOSERROR;
	}
}

C_LINKAGE int os_threadx_error(UINT status)
{
    return set_db_error(_os_threadx_error(status));
}

/** @todo initialize a critical section */

int os_threadx_init()
{
    return DB_NOERROR;
}

int os_threadx_done()
{
    return DB_NOERROR;
}

#endif /* OS_THREADX */
