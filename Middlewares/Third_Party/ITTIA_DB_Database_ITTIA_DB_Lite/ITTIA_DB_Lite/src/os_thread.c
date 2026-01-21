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

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS)

#include "os/os_lib.h"
#include "ittia/os/os_thread.h"
#include "ittia/os/os_debug.h"

#include "os/os_mockup.h"

static os_thread_finalizer_t * fhead = NULL, * ftail = NULL;

DBDLL_API int os_thread_add_finalizer(os_thread_finalizer_t* fini,
                                      os_thread_finalizer_proc_t fn,
                                      void * arg)
{
#ifdef DB_DEBUG
    os_thread_finalizer_t * p;
    for(p = fhead; p != NULL; p = p->next) {
        DB_ASSERT( p != fini );
    }
#endif

    DB_REQUIRE( fini->prior == NULL, DB_EINVAL );
    DB_REQUIRE( fini->next == NULL, DB_EINVAL );
    DB_REQUIRE( fini->fn == NULL, DB_EINVAL );
    DB_REQUIRE( fini->arg == NULL, DB_EINVAL );
    DB_REQUIRE( fn != NULL, DB_EINVAL );

    fini->fn = fn;
    fini->arg = arg;

    if (ftail == NULL)
        fhead = ftail = fini;
    else {
        fini->prior = ftail;
        ftail->next = fini;
        ftail = fini;
    }
    return DB_NOERROR;
}

DBDLL_API int os_thread_remove_finalizer(os_thread_finalizer_t* fini)
{
    if (fini == NULL || fini->fn == NULL)
        return set_db_error( DB_EINVAL );

#ifdef DB_DEBUG
    {
        os_thread_finalizer_t * p;
        for(p = fhead; p != NULL && p != fini; p = p->next)
            ;
        DB_ASSERT( p != NULL );
    }
#endif

    DB_REQUIRE( fini->fn != NULL, DB_EINVAL );
    fini->fn = NULL;
    fini->arg = NULL;
    if (fini->prior == NULL) {
        DB_ASSERT( fhead == fini );
        fhead = fini->next;
    } else {
        DB_ASSERT( fhead != fini );
        fini->prior->next = fini->next;
    }

    if (fini->next == NULL) {
        DB_ASSERT( ftail == fini );
        ftail = fini->prior;
    } else {
        DB_ASSERT( ftail != fini );
        fini->next->prior = fini->prior;
    }
    return DB_NOERROR;
}

void os_thread_call_finalizers()
{
    os_thread_finalizer_t * p;
    for(p = fhead; p != NULL; p = p->next)
        p->fn( p->arg );
}

#endif /* HAVE_THREADS */
