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

#ifndef VXWORKS_MUTEX_H
#define VXWORKS_MUTEX_H

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) && defined(HAVE_VXWORKS_MUTEX)

#include "ittia/os/std/semaphore.h"

C_HEADER_BEGIN

typedef SEM_ID os_mutex_t;

DBDLL_API int os_mutex_init(os_mutex_t * mutex);
DBDLL_API int os_mutex_destroy(os_mutex_t * mutex);
DBDLL_API int os_mutex_trylock(os_mutex_t * mutex);
DBDLL_API int os_mutex_lock(os_mutex_t * mutex);
DBDLL_API int os_mutex_unlock(os_mutex_t * mutex);

C_HEADER_END

#endif

#endif /* VXWORKS_MUTEX_H */
