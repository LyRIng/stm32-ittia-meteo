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

#ifndef GENERIC_FASTLOCK_H
#define GENERIC_FASTLOCK_H

#if defined(HAVE_THREADS) && defined(HAVE_GENERIC_FASTLOCK)

/* Use mutexes instead of spinlocks, since we need recursive behavior */
#include "ittia/os/os_mutex.h"

C_HEADER_BEGIN

typedef os_mutex_t os_fastlock_t;

#define os_fastlock_init     os_mutex_init
#define os_fastlock_destroy  os_mutex_destroy
#define os_fastlock_trylock  os_mutex_trylock
#define os_fastlock_lock     os_mutex_lock
#define os_fastlock_unlock   os_mutex_unlock

C_HEADER_END

#endif

#endif
