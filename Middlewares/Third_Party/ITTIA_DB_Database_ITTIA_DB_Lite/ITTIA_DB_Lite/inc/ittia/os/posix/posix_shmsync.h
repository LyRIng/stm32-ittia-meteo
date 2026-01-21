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

/* specialize shmsync for posix */

#include "ittia/os/os_config.h"

#ifdef HAVE_POSIX_SHM

#if defined(HAVE_MMAP)
#   define OS_SHMSYNC_MMAP
#elif defined(HAVE_SHMGET)
#   define OS_SHMSYNC_SHM
#endif

#if defined(OS_SHMSYNC_MMAP) || defined(OS_SHMSYNC_SHM)
#   define OS_SHMSYNC   1
#endif

#ifdef OS_SHMSYNC

#include "ittia/os/std/io.h"    /* unistd.h */
#include "ittia/os/std/semaphore.h"
#include "ittia/os/std/errno.h"
#include "ittia/os/std/time.h" /* timespec */
#include <signal.h> /* kill */


#ifdef OS_SHMSYNC_MMAP

#   ifdef HAVE_SYS_MMAN_H
#       include <sys/mman.h>
#   endif

#elif defined(OS_SHMSYNC_SHM)

#   ifdef HAVE_SYS_SHM_H
#       include <sys/shm.h>
#   endif

#endif

/** Process **********************************/

struct sm_Process       /* local memory */
{
    sm_Pid pid;
};

/** Events ***********************************/
struct sm_EventInfo /* shared memory struct */
{
    sm_Atomic state;
    sem_t     sem;
};

struct sm_Event     /* local memory */
{
    struct sm_EventInfo * ptr;
    sm_AtomicDomain * dom;
};

/** Semaphore ********************************/

struct sm_SemInfo   /* shared memory struct */
{
    sem_t sem;
};

struct sm_Sem       /* local memory */
{
    struct sm_SemInfo * sem_info;
    sm_AtomicDomain * sm_domain;
};

/** Shared Memory ****************************/

struct sm_Memory /* local memory */
{
#ifdef OS_SHMSYNC_MMAP
    pid_t        owner;
    int          fd;    /* shared file */
    db_fname_t * name;  /* server side only */
    ssize_t      size;
#elif defined(OS_SHMSYNC_SHM)
    int shmid;  /* SUSv5 shared memory */
    int fd;     /* at server side - key file */
#endif
};

#endif /* OS_SHMSYNC */

#endif /* HAVE_POSIX_SHM */
