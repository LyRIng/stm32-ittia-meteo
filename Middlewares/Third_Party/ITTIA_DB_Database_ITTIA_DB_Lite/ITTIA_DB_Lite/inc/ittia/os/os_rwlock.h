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
 * Read/write locks
 */

#ifndef OS_RWLOCK_H
#define OS_RWLOCK_H

#include "ittia/os/os_config.h"

#ifdef HAVE_THREADS

#if defined(HAVE_POSIX_RWLOCK)
#   include "ittia/os/posix/posix_rwlock.h"
#elif defined(HAVE_VXWORKS_RWLOCK)
#   include "ittia/os/vxworks/vxworks_rwlock.h"
#elif defined(HAVE_GENERIC_RWLOCK)
#   include "ittia/os/generic/generic_rwlock.h"
#endif 

C_HEADER_BEGIN

DBDLL_API int os_rwlock_init(os_rwlock_t *);
DBDLL_API int os_rwlock_destroy(os_rwlock_t *);

DBDLL_API int os_rwlock_rdlock(os_rwlock_t *);
DBDLL_API int os_rwlock_tryrdlock(os_rwlock_t *);

DBDLL_API int os_rwlock_wrlock(os_rwlock_t *);
DBDLL_API int os_rwlock_trywrlock(os_rwlock_t * lock);

DBDLL_API int os_rwlock_unlock(os_rwlock_t *);

C_HEADER_END

#endif /* HAVE_THREADS */

#endif /* OS_RWLOCK_H */
