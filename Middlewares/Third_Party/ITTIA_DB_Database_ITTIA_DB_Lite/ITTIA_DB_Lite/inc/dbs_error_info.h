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
/*  https://www.ittia.com               */
/*                                                                        */
/*                                                                        */
/**************************************************************************/


#ifndef DBS_ERROR_INFO_H
#define DBS_ERROR_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

/// Database error information.
typedef struct dbs_error_info_t {
    /// @brief Error code listed in ittia/db/db_error.h or ittia/os/os_error.h
    int code;
    /// @brief Error name, with @c DB_ prefix.
    const char * name;
    /// @brief Error description.
    const char * description;

} dbs_error_info_t;

/// Get database error information by error code
///
/// @param error_code
///   Error code listed in ittia/db/db_error.h or ittia/os/os_error.h
/// @returns
///   Database error information
///
/// If no known error matches the error code, a generic error name and
/// description are returned with the given error code.
dbs_error_info_t dbs_get_error_info(int error_code);

/// Get database error information by error name
///
/// @param name
///   Error code listed in ittia/db/db_error.h or ittia/os/os_error.h,
///   with @c DB_ prefix
/// @returns
///   Database error information, or NULL if not found.
const dbs_error_info_t * dbs_find_error(const char * name);


#ifdef __cplusplus
}
#endif

#endif
