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

#ifndef ITTIA_OS_UCOS_III_WAIT_TIME_H
#define ITTIA_OS_UCOS_III_WAIT_TIME_H

#include "ittia/os/os_config.h"

#ifdef HAVE_UCOS_III_TIME

C_HEADER_BEGIN

/* time useful in multithreading functions */

typedef uint32_t os_wait_time_t;

#ifndef WAIT_FOREVER
#define WAIT_FOREVER 65535
#endif
#define OS_WAIT_TIME_PREC OS_TICKS_PER_SEC

#define WAIT_MILLISEC(x) (os_wait_time_t)(((x) * OS_TICKS_PER_SEC) / 1000)
#define WAIT_MICROSEC(x) (os_wait_time_t)(((x) * OS_TICKS_PER_SEC) / 1000000)

DBDLL_API void os_yield(os_wait_time_t time);
DBDLL_API os_wait_time_t os_current_time(void);

C_HEADER_END

#endif

#endif /* ITTIA_OS_UCOS_III_WAIT_TIME_H */
