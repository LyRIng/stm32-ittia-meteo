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

#include "ittia/os/os_error.h"
#include "os/os_lib.h"

#ifdef HAVE_THREADS

#include "ittia/os/os_tls.h"

static os_tls_key_t os_error_key;

#endif

static dbstatus_t db_error = 0;

#include "os/os_mockup.h"

dbstatus_t init_db_error()
{
#ifdef HAVE_THREADS
    int rc = os_tls_init(&os_error_key);
    if (DB_FAILED(rc))
        return rc;
#endif
    db_error = 0;
    return DB_NOERROR;
}

dbstatus_t done_db_error()
{
#ifdef HAVE_THREADS
    os_tls_destroy(&os_error_key);
#else
#endif
    return DB_NOERROR;
}

dbstatus_t DBDLL_API set_db_error(dbstatus_t rc)
{
#ifdef HAVE_THREADS
    int tls_rc = os_tls_set(os_error_key, (os_tls_value_t)(intptr_t)rc);
    if (rc == DB_ESTATE) {
        /* OS isn't running; save the error in a global variable instead. */
        db_error = rc;
    }
    else if (DB_FAILED(tls_rc)) {
        return tls_rc;
    }
#else
    db_error = rc;
#endif
    return rc;
}

/** Get error code */
dbstatus_t DBDLL_API get_db_error()
{
#ifdef HAVE_THREADS
    os_tls_value_t value;
    int rc = os_tls_get(os_error_key, &value);
    if (rc == DB_ESTATE) {
        /* OS isn't running; get the error from a global variable instead. */
        return db_error;
    }
    if (DB_FAILED(rc))
        return rc;
    return (int)(intptr_t)value;
#else
    return db_error;
#endif
}

/** Clear error code */
dbstatus_t DBDLL_API clear_db_error()
{
    return set_db_error(DB_NOERROR);
}

void DBDLL_API save_db_error(ErrorInfo* error_info)
{
    error_info->error_code = get_db_error();
}

void DBDLL_API restore_db_error(const ErrorInfo* error_info)
{
    set_db_error(error_info->error_code);
}
