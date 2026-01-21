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

#ifndef GENERIC_MUTEX_H
#define GENERIC_MUTEX_H

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) && defined(HAVE_GENERIC_MUTEX)
/* Generic mutex based on semaphores.
 */

#include "ittia/os/os_sem.h"
#include "ittia/os/os_thread.h"

C_HEADER_BEGIN

typedef struct {
	os_sem_t sem;
	// ID of thread that currently owns the mutex.
	uintptr_t owner;
	// Number of times the mutex has been recursively locked by the current owner.
	uint32_t rec_lock_count;
} os_mutex_t;

C_HEADER_END

#endif

#endif /* GENERIC_MUTEX_H */
