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

#ifndef VXWORKS_H
#define VXWORKS_H

#include "ittia/os/os_config.h"

#if defined(OS_VXWORKS)

#include <vxWorks.h>
#include <sysLib.h>
#include <memLib.h>
#include <objLib.h>
#include <semLib.h>
#include "ittia/os/os_wait_time.h"

#ifdef HAVE_POSIX_TIME
#include <time.h>
#endif

C_HEADER_BEGIN

struct timespec;

int os_vxworks_error(void);

int os_vxworks_init(void);
int os_vxworks_done(void);

C_HEADER_END

#endif /* OS_VXWORKS */

#endif /* VXWORKS_H */
