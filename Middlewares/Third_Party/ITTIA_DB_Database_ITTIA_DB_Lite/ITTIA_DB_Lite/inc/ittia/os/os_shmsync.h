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

#ifndef OS_SHMSYNC_H
#define OS_SHMSYNC_H

#include "ittia/os/os_config.h"
#include "ittia/os/os_wait_time.h"
#include "ittia/os/atomic/atomic_shm.h"

/* shared memory synchronization primitives. */

C_HEADER_BEGIN

#ifdef OS_WIN32
typedef unsigned long sm_Pid;
#define SM_NOPID    (sm_Pid)(-1)

#elif defined(OS_POSIX)
typedef int sm_Pid;
#define SM_NOPID    (sm_Pid)(0) /* on unix processes start with 1 */
#endif

typedef unsigned long sm_Size;

/** Timeout **********************************/
/* TODO: get rid of sm specific timeouts */
typedef os_wait_time_t sm_Timeout;
#define SM_INFINITE OS_WAIT_FOREVER

/** Process **********************************/
typedef struct sm_Process sm_Process;      /* local memory */

DBDLL_API int sm_ps_open(sm_Process *, sm_Pid);
DBDLL_API int sm_ps_close(sm_Process *);

/* check if process is still alive */
DBDLL_API db_bool_t sm_ps_alive(sm_Process*);
DBDLL_API sm_Pid sm_getpid();

/** Events ***********************************/

typedef struct sm_EventInfo sm_EventInfo; /* shared memory */
typedef struct sm_Event sm_Event;     /* local memory */

DBDLL_API int sm_event_create( sm_AtomicDomain*, sm_EventInfo *, db_bool_t set );
DBDLL_API int sm_event_destroy( sm_AtomicDomain*, sm_EventInfo * );

DBDLL_API int sm_event_open( sm_AtomicDomain*, /* OUT */ sm_Event * local_event, /* IN */ sm_EventInfo * orig_event, /* IN */ sm_Process * orig_ps );
DBDLL_API int sm_event_close( sm_Event * );

DBDLL_API int sm_event_wait( sm_Event *, sm_Timeout msec );
DBDLL_API int sm_event_set( sm_Event * );
DBDLL_API int sm_event_reset( sm_Event * );

/** Semaphore ********************************/

typedef struct sm_SemInfo sm_SemInfo;   /* shared memory */
typedef struct sm_Sem sm_Sem;       /* local memory */

DBDLL_API int sm_sem_create( sm_AtomicDomain*, sm_SemInfo *, long count );
DBDLL_API int sm_sem_destroy( sm_AtomicDomain*, sm_SemInfo * );

DBDLL_API int sm_sem_open( sm_AtomicDomain*, /* OUT */ sm_Sem * local_event, /* IN */ sm_SemInfo * orig_event, /* IN */ sm_Process * orig_ps );
DBDLL_API int sm_sem_close( sm_Sem * );

DBDLL_API int sm_sem_wait( sm_Sem*, sm_Timeout msec );
DBDLL_API int sm_sem_post( sm_Sem* );

/** Shared Memory ****************************/

typedef struct sm_Memory sm_Memory; /* local memory */

DBDLL_API int sm_mem_create( sm_Memory *, const db_fname_t * name, sm_Size mem_size, int flags );
DBDLL_API int sm_mem_open( sm_Memory *, const db_fname_t * name, int flags );
DBDLL_API int sm_mem_dup( sm_Memory * target, sm_Memory * source );
DBDLL_API int sm_mem_close( sm_Memory * );

DBDLL_API void * sm_mem_attach( sm_Memory * );
DBDLL_API void   sm_mem_detach( sm_Memory *, void * );
DBDLL_API size_t sm_mem_page_size();
DBDLL_API size_t sm_mem_size(size_t);

C_HEADER_END

/* specialize */
#if defined(HAVE_WIN32_SHM)
#include "ittia/os/win32/win32_shmsync.h"
#elif defined(HAVE_POSIX_SHM)
#include "ittia/os/posix/posix_shmsync.h"
#endif

#endif /* OS_SHMSYNC_H */
