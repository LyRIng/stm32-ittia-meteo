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

#ifndef GENERIC_RWLOCK_H
#define GENERIC_RWLOCK_H

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) && defined(HAVE_GENERIC_RWLOCK)

/* rwlocks using mutex and sems. As fair as mutex is. */
#include "ittia/os/os_mutex.h"
#include "ittia/os/os_sem.h"
#include "ittia/os/os_atomic.h"

#ifdef DB_DEBUG
#	include "ittia/os/win32/win32_thread.h"
#endif

/** TODO: temporary fix to DB_DEBUG code being written for Win32 only */ 
#ifndef OS_WIN32
/*#	undef DB_DEBUG*/
#endif

C_HEADER_BEGIN

/* rdlock():
 * Results are undefined if the calling thread holds a write
 * lock on rwlock at the time the call is made.
 * A thread may hold multiple concurrent read locks on rwlock
 * (that is, successfully call the pthread_rwlock_rdlock()
 * function n times). If so, the thread must perform matching
 * unlocks (that is, it must call the pthread_rwlock_unlock()
 * function n times). 
 *
 * wrlock():
 * Results are undefined if the calling thread holds the
 * read-write lock (whether a read or write lock) at the
 * time the call is made. 
 */

typedef struct os_rwlock_t os_rwlock_t;

struct os_rwlock_t
{
    os_mutex_t mutex;
    os_sem_t   sem;
    os_atomic_t   nlocks;
};


C_HEADER_END

#endif

#endif /* GENERIC_RWLOCK_H */
