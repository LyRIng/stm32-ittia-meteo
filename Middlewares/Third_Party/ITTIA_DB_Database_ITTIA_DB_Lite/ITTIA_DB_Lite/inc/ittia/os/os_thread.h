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

/* @file
 *
 * Thread control
 */

#ifndef OS_THREAD_H
#define OS_THREAD_H

#include "ittia/os/os_config.h"

#ifdef HAVE_THREADS

/* include specific thread prototypes */

#if defined(HAVE_WIN32_THREADS)
#   include "ittia/os/win32/win32_thread.h"
#elif defined(HAVE_POSIX_THREADS)
#   include "ittia/os/posix/posix_thread.h"
#elif defined(HAVE_FREERTOS_THREADS)
#   include "ittia/os/freertos/ittia_freertos_thread.h"
#elif defined(HAVE_THREADX_THREADS)
#   include "ittia/os/threadx/threadx_thread.h"
#elif defined(HAVE_UCOS_II_THREADS)
#   include "ittia/os/ucos_ii/ucos_ii_thread.h"
#elif defined(HAVE_UCOS_III_THREADS)
#   include "ittia/os/ucos_iii/ucos_iii_thread.h"
#endif

C_HEADER_BEGIN

/* declare common thread prototypes */

#ifndef HAVE_FREERTOS_THREADS
typedef struct os_thread_t os_thread_t;
#endif

#define NO_THREAD   ((os_thread_t*)0)

typedef void (*os_thread_proc_t)(void*);

/* stack sizes */

#define TINY_STACK_SIZE    4*1024
#define SMALL_STACK_SIZE   16*1026
#define MEDIUM_STACK_SIZE  128*1024
#define LARGE_STACK_SIZE   1024*1024

#define DEFAULT_STACK_SIZE 0

/* thread flags */
#define OS_THREAD_JOINABLE 1  /**< #os_thread_join can be used to wait for the
                                   thread to complete. */
#define OS_THREAD_DETACHED 0  /**< Another thread cannot wait for thread to
                                   complete. */
#define OS_THREAD_JOIN_MASK (OS_THREAD_DETACHED | OS_THREAD_JOINABLE)

/** 
 * Spawn a new thread, executing the function @a proc with the argument @a arg
 * in the new thread. The calling thread will continue to run in parallel with
 * the new thread.
 *
 * @param func
 *   Function to execute in the new thread.
 * @param arg
 *   Argument to @a func.
 * @param stack_size  
 *   Stack size, in bytes, or DEFAULT_STACK_SIZE.
 * @param flags
 *  - #OS_THREAD_JOINABLE: The calling thread must wait for the new thread to
 *    finish by calling #os_thread_join at a later time.
 *  - #OS_THREAD_DETACHED: The calling thread cannot wait for the new thread to
 *    finish.
 * @param [out] h
 *   Thread handle.
 * 
 * @returns DB_NOERROR or an error code.
 */
DBDLL_API int os_thread_spawn(os_thread_proc_t proc, void * arg, int stack_size, int flags, os_thread_t ** h);
/** 
 * @returns The handle for the current thread.
 */
DBDLL_API os_thread_t * os_thread_self(void);

/** @returns os thread identity. information purposes only */
DBDLL_API uintptr_t os_thread_id(void);

/** Set the name of a thread, if possible (may be truncated to 15 characters). */
DBDLL_API void os_thread_set_name(os_thread_t * h, const char * thread_name);

/** 
 * Join with another thread by waiting for that thread to complete.
 * 
 * @param h  Handle of the thread to join with.
 *
 * @pre The thread indicated by @a h must have been spawned with the
 * OS_THREAD_JOINABLE flag.
 */
DBDLL_API int os_thread_join(os_thread_t * h);
DBDLL_API int os_thread_dup(os_thread_t*);
DBDLL_API int os_thread_close(os_thread_t* h);

/* these two are os specific ones, attached threads cannot be joined */
DBDLL_API int os_thread_attach(void);
DBDLL_API int os_thread_detach(void);

/* === thread finalizers === */
typedef void (*os_thread_finalizer_proc_t)(void *);
typedef struct os_thread_finalizer_t
{
    /* Opaque structure, members are subject to change.
     * No need to initialize
     */
    os_thread_finalizer_proc_t fn;
    void * arg;
    struct os_thread_finalizer_t * next, * prior;
} os_thread_finalizer_t;

#define OS_THREAD_FINALIZER_INIT {0, 0, 0, 0}

/* these are not thread-safe, use with caution */
DBDLL_API int os_thread_add_finalizer(os_thread_finalizer_t*, os_thread_finalizer_proc_t, void *);
DBDLL_API int os_thread_remove_finalizer(os_thread_finalizer_t*);

#if defined(OS_HAVE_NATIVE_THREADID) && !defined(os_native_threadid)
DBDLL_API void os_native_threadid(os_native_threadid_t*, int clear);
#endif

C_HEADER_END

#endif /* HAVE_THREADS */

#endif /* OS_THREAD_H */
