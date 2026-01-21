/**************************************************************************/
/*                                                                        */
/*      Copyright (c) 2005-2025 by ITTIA L.L.C. All rights reserved.      */
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

 
/* Do not edit! This file generated automatically from tools/errors. */

/* @file
 *
 * Operating system error codes
 */

#ifndef OS_ERROR_H
#define OS_ERROR_H

#include "ittia/os/os_config.h"

/** @addtogroup Errors
 * @{ */

C_HEADER_BEGIN

#if 0 && defined(DB_DEBUG)

/* catch error code validity */
typedef struct {} * dbstatus_t;

#else

/**
 * Error code type: indicates success, warning, or error condition.
 *
 * @see Errors
 */
typedef int dbstatus_t;

#endif

#define _DB_ERROR(n)           ((dbstatus_t) -n)
#define _DB_WARNING(n)         ((dbstatus_t) (1 + n))

/** @name  Status Codes */
 /** @{ */
#define DB_NOERROR             0 /**< No error occurred. */
#define DB_FAILURE            -1 /**< Operation failed. */
/** @} */

/** @name  Operating System/File System Errors */
 /** @{ */
#define DB_EBADF              -2 /**< Invalid file handle. */
#define DB_EEXIST             -3 /**< File already exists. */
#define DB_ENOENT             -4 /**< File doesn't exist. */
#define DB_ENOSPACE           -5 /**< No space available. */
#define DB_EIO                -6 /**< I/O error. */
#define DB_EACCESS            -7 /**< File cannot be opened due to invalid access mode or insufficient file system privileges. */
#define DB_EEXTEND            -8 /**< File operation is out of file size. */
#define DB_EDEADLOCK          -9 /**< Unable to lock - deadlock condition detected. */
#define DB_ELOCKED           -10 /**< Unable to acquire lock. */
/** @} */

/** @name  API Argument Errors */
 /** @{ */
#define DB_EINVAL            -11 /**< Invalid argument. */
#define DB_ERANGE            -12 /**< Argument is out of range. */
#define DB_ESTATE            -13 /**< Object state is not compatible with the method called or arguments. */
#define DB_ENOTIMPL          -14 /**< The requested functionality is not implemented in the current library package. */
#define DB_ENOMEM            -15 /**< [HY001] No memory available for allocation. */
#define DB_EOSERROR          -16 /**< General operating system error. */
#define DB_ENOTAPPLICABLE    -17 /**< Not applicable. */
#define DB_ENOTHREAD         -18 /**< No such thread. */
#define DB_EINTERNAL         -19 /**< Internal error detected. */
#define DB_EINVPATH          -20 /**< Invalid path encoding. */
#define DB_EINVFS            -21 /**< Invalid file system requested. */
#define DB_EDBGFAILURE       -22 /**< Simulated failure for debugging. */
#define DB_ENLSSPACE         -23 /**< Not enough space for NLS conversion. */
#define DB_EINVCHAR          -24 /**< Invalid character for NLS conversion. */
#define DB_EINVENC           -25 /**< Invalid encoding for NLS conversion. */
#define DB_ENOSYS            -26 /**< A required function is not supported on this platform. */
#define DB_EINVLOCALE        -27 /**< Invalid or unknown locale name. */
/** @} */

/** @name  Socket Errors */
 /** @{ */
#define DB_ESOCKETSTARTUP    -50 /**< Socket startup error. */
#define DB_ESOCKETOPEN       -51 /**< Socket open error. */
#define DB_ESOCKET           -52 /**< General socket error. */
#define DB_ESOCKETHOSTNAME   -53 /**< Host name error. */
#define DB_ESOCKETCONNECTION -54 /**< Connection error. */
#define DB_ESOCKETRECV       -55 /**< Socket data receive error. */
#define DB_ESOCKETSEND       -56 /**< Socket data send error. */
#define DB_ESOCKETNOTCONN    -57 /**< Socket is not connected. */
#define DB_ESSLCIPHERKEY     -60 /**< Private key is not valid. */
#define DB_ESSLDHPARAMS      -61 /**< Diffie-Hellmap parameteres are not valid. */
#define DB_ESSLCERTIFICATE   -62 /**< Certificate chain is not valid. */
#define DB_ESSLCALOCATION    -63 /**< CA file is not valid. */
/** @} */


/**
 * Error information
 * @see Errors
 */
typedef struct ErrorInfo
{
    dbstatus_t error_code;
} ErrorInfo;

/* Error routines */

/** @name Error test routines
 * @{ */
#define DB_SUCCESS(x)  ((int32_t)(x) >= DB_NOERROR)
#define DB_FAILED(x)   ((int32_t)(x) < DB_NOERROR)
/** @} */

/** @name Error support routines
 * @{ */

#define DB_PUSH_ERROR(__error_info__)  save_db_error(__error_info__)
#define DB_POP_ERROR(__error_info__)   restore_db_error(__error_info__)

#define DB_SET_ERROR(x)   set_db_error(x)
#define DB_RAISE_ERROR(x) return DB_SET_ERROR(x)
#define DB_THROW_ERROR(x) do { int __rc__ = (x); if (DB_FAILED(__rc__)) DB_RAISE_ERROR(__rc__); } while (false)
#define DB_ONERROR_GOTO(x, lab) do { int __rc__ = (x); if (DB_FAILED(__rc__)) goto lab; } while (false)
#define DB_ONERROR_RET(x, result) do { int __rc__ = (x); if (DB_FAILED(__rc__)) return result; } while (false)

/**
 * Set error code.
 *
 * @note Do not call this function directly. Use #DB_SET_ERROR instead.
 *
 * @param rc_t  error code
 *
 * @see DB_THROW_ERROR, DB_RAISE_ERROR, DB_SET_ERROR
 */
DBDLL_API dbstatus_t set_db_error(dbstatus_t rc_t);
/**
 * Get the most recent error code.
 *
 * @note This value is not cleared by any API calls except #clear_db_error. To
 * check for an error in any of a series of calls, first call #clear_db_error,
 * then call each API function, and finally use #get_db_error to check the
 * result.
 *
 * @returns the last error code that was set.
 *
 * @see clear_db_error
 */
DBDLL_API dbstatus_t get_db_error(void);
/** Clear the current error code. @see get_db_error */
DBDLL_API dbstatus_t clear_db_error(void);

/** Save the current error code to an ErrorInfo. */
DBDLL_API void save_db_error(ErrorInfo*);
/** Restore the current error code from an ErrorInfo. */
DBDLL_API void restore_db_error(const ErrorInfo*);

/** @} */

/** 
 * @def DB_SUCCESS()
 * Test whether an error code represents a successful result.
 * 
 * @param x  Error code to be tested.
 * 
 * @returns true when the operation that set @a x was successful.
 *
 * @see DB_FAILED, Errors
 */
/** 
 * @def DB_FAILED()
 * Test whether an error code represents a failed result.
 * 
 * @param x  Error code to be tested.
 * 
 * @returns true when the operation that set @a x was not successful.
 *
 * @see DB_SUCCESS, Errors
 */
/**
 * @def DB_PUSH_ERROR()
 * Store the current error code.
 *
 * @param __error_info__  Destination for the error code.
 */
/**
 * @def DB_POP_ERROR()
 * Restore the current error code.
 *
 * @param __error_info__  Source of the error code.
 */
/**
 * @def DB_SET_ERROR()
 * Set the given error code.
 *
 * @param x  the error code.
 *
 * @see DB_THROW_ERROR, DB_RAISE_ERROR
 */
/**
 * @def DB_RAISE_ERROR()
 * Set the given error code and return it to the caller.
 *
 * @param x  the error code.
 *
 * @see DB_THROW_ERROR, DB_SET_ERROR
 */
/**
 * @def DB_THROW_ERROR()
 * If @a x is an error, throw it to the caller. Otherwise, continue.
 *
 * @param x  the error code.
 *
 * @see DB_RAISE_ERROR, DB_SET_ERROR, DB_ONERROR_GOTO
 */
/**
 * @def DB_ONERROR_GOTO()
 * If @a x is an error, go to @a lab. Otherwise, continue.
 *
 * @param x   the error code.
 * @param lab label to go to in case of error.
 *
 * @see DB_RAISE_ERROR, DB_SET_ERROR, DB_THROW_ERROR
 */

C_HEADER_END

/**  @} */

#endif /* OS_ERROR_H */
