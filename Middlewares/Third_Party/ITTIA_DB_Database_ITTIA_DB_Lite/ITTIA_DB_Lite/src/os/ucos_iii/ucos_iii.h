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

#ifndef OS_UCOS_III_H
#define OS_UCOS_III_H

#include "ittia/os/os_config.h"

#if defined(OS_UCOS_III)

#include <os.h>
#include "ittia/os/os_tls.h"

C_HEADER_BEGIN

struct timespec;

#if defined(HAVE_UCOS_III_THREADS)

extern os_tls_key_t os_ucos_iii_self;

#endif

int os_ucos_iii_error(int);

C_HEADER_END

#endif /* OS_UCOS_III */

#endif /* OS_UCOS_III_H */
