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

#ifndef WIN32_SHMSYNC_H
#define WIN32_SHMSYNC_H

/*********************************************************************/

#ifdef OS_WIN32

/* ==== specialize shmsync for WIN32 ==== */

#include "ittia/os/std/windows.h"

/** Process **********************************/

struct sm_Process       /* local memory */
{
    HANDLE hprocess;
};

/** Events ***********************************/
struct sm_EventInfo /* shared memory struct */
{
    HANDLE  hshared;
};

struct sm_Event     /* local memory */
{
    HANDLE  hlocal;
};

#define sm_event_handle(ev)     ((ev)->hlocal)

/** Semaphore ********************************/

struct sm_SemInfo   /* shared memory struct */
{
    HANDLE  hshared;
};

struct sm_Sem       /* local memory */
{
    HANDLE  hlocal;
};

/** Shared Memory ****************************/

struct sm_Memory /* local memory */
{
    HANDLE hmap;
};

/* no size constraints */
#define sm_mem_size(x)  (x)

#endif /* OS_WIN32 */

#endif /* WIN32_SHMSYNC_H */
