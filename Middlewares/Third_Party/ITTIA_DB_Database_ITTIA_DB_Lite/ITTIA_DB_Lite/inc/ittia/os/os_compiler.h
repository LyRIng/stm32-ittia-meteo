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

#ifndef ITTIA_OS_COMPILER_H
#define ITTIA_OS_COMPILER_H

/** @file
 *
 * Compiler specific information. 
 */

#include "ittia/os/os_config.h"
 
#ifdef _MSC_VER
/*
 *   MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
 *   MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
 *   MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
 *   MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
 *   MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
 *   MSVC++ 7.1  _MSC_VER == 1310 (Visual Studio 2003)
 *   MSVC++ 7.0  _MSC_VER == 1300
 *   MSVC++ 6.0  _MSC_VER == 1200
 *   MSVC++ 5.0  _MSC_VER == 1100
 */

#   define DB_MSC_VER_5       1100
#   define DB_MSC_VER_6       1200
#   define DB_MSC_VER_7       1300
#   define DB_MSC_VER_7_1     1310
#   define DB_MSC_VER_8       1400
#   define DB_MSC_VER_9       1500
#   define DB_MSC_VER_10      1600
#   define DB_MSC_VER_11      1700
#   define DB_MSC_VER_12      1800

#   define DB_MSVS_VER_2003    1310
#   define DB_MSVS_VER_2005    1400
#   define DB_MSVS_VER_2008    1500
#   define DB_MSVS_VER_2010    1600
#   define DB_MSVS_VER_2012    1700
#   define DB_MSVS_VER_2013    1800
#   define DB_MSVS_VER_2015    1900

#   if (_MSC_VER < 1100)
#       error "Check MSVC configuration and update definitions accordingly"
#   endif

#   define DB_MSC                              1
#   define DB_MSC_CURRENT_VERSION              _MSC_VER
#   define DB_MSC_MAKE_VERSION( major, minor ) (DB_MSC_##major##_VER + (minor % 10) * 10)

#   define DB_MSC_VERSION( major, minor )      (DB_MSC_CURRENT_VERSION >= DB_MSC_MAKE_VERSION(major, minor))
#   define DB_MSC_VERSION_LT( major, minor )   (DB_MSC_CURRENT_VERSION <  DB_MSC_MAKE_VERSION(major, minor))

#   define DB_MSVS_VERSION( major )            (DB_MSC_CURRENT_VERSION >= DB_MSVS_VER_##major)
#   define DB_MSVS_VERSION_LT( major )         (DB_MSC_CURRENT_VERSION <  DB_MSVS_VER_##major)

#else  /* !_MSC_VER */

#   define DB_MSC_VERSION( major, minor )    0
#   define DB_MSC_VERSION_LT( major, minor ) 0
#   define DB_MSVS_VERSION( major )          0
#   define DB_MSVS_VERSION_LT( major )       0

#endif /* _MSC_VER */

#ifdef __GNUC__

#   ifdef __GNUC_PATCHLEVEL__ // __GNUC_PATCHLEVEL__ introduced in gcc 3.0
#       define DB_GNUC_PATCHLEVEL     __GNUC_PATCHLEVEL__
#   else
#       define DB_GNUC_PATCHLEVEL     0
#   endif

#   define DB_GNUC_MAKE_VERSION(major, minor, patch)   ((major * 100000) + (minor % 100) * 1000 + (patch % 1000))
#   define DB_GNUC_CURRENT_VERSION                     DB_GNUC_MAKE_VERSION(__GNUC__, __GNUC_MINOR__, DB_GNUC_PATCHLEVEL)

#   define DB_GNUC                                  1
#   define DB_GNUC_VERSION(major, minor, patch)     (DB_GNUC_CURRENT_VERSION >= DB_GNUC_MAKE_VERSION(major, minor, patch))
#   define DB_GNUC_VERSION_LT(major, minor, patch)  (DB_GNUC_CURRENT_VERSION <  DB_GNUC_MAKE_VERSION(major, minor, patch))

#else

#   define DB_GNUC_VERSION(major, minor, patch)     0
#   define DB_GNUC_VERSION_LT(major, minor, patch)  0

#endif


#endif /* ITTIA_OS_COMPILER_H */
