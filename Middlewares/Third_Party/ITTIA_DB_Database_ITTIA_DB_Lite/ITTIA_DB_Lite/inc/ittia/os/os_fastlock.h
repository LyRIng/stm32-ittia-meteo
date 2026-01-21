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
 */

#ifndef OS_FAST_LOCK_H
#define OS_FAST_LOCK_H

#include "ittia/os/os_config.h"

#ifdef HAVE_THREADS

#if defined(HAVE_WIN32_THREADS)
#   include "ittia/os/win32/win32_fastlock.h"
#elif defined(HAVE_POSIX_FASTLOCK)
#   include "ittia/os/posix/posix_fastlock.h"
#elif defined(HAVE_VXWORKS_FASTLOCK)
#   include "ittia/os/vxworks/vxworks_fastlock.h"
#elif defined(HAVE_GENERIC_FASTLOCK)
#   include "ittia/os/generic/generic_fastlock.h"
#endif

C_HEADER_BEGIN

DBDLL_API int os_fastlock_init(os_fastlock_t*);
DBDLL_API int os_fastlock_destroy(os_fastlock_t*);
DBDLL_API int os_fastlock_lock(os_fastlock_t*);
DBDLL_API int os_fastlock_trylock(os_fastlock_t*);

#ifndef os_fastlock_unlock
DBDLL_API int os_fastlock_unlock(os_fastlock_t*);
#endif

C_HEADER_END

#endif /* HAVE_THREADS */

#endif /* OS_FAST_LOCK_H */
