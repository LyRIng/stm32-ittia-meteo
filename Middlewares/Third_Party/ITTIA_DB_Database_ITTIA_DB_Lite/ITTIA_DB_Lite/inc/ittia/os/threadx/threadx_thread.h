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

#ifndef THREADX_THREAD_H
#define THREADX_THREAD_H

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) \
	&& defined(HAVE_THREADX_THREADS)

/* ThreadX might include system headers inside an extern "C" block. */
#include "ittia/os/std/string.h"
#include "ittia/os/std/malloc.h"

#include "tx_api.h"

#endif /* HAVE_THREADX_THREADS */

#endif /* THREADX_THREAD_H */
