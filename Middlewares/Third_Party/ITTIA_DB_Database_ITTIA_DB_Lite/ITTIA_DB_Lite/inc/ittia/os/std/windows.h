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

#ifndef OS_STD_WINDOWS_H
#define OS_STD_WINDOWS_H

#include "ittia/os/os_env.h"

#if defined(OS_WIN32) || defined(OS_WINNT)
#
#   define WIN32_LEAN_AND_MEAN
#
#   if !defined(WINVER) && !defined(_WIN32_WINNT)
#       define WINVER 0x0500
#       define _WIN32_WINNT 0x0500
#   endif
#
#   /* cope with argument name clash in MSVC 6 */
#   define Array _array
#
#   if defined(_MSC_VER) && _MSC_VER >= 1500
#       include <intrin.h>
#   endif
#
#   ifdef NOMINMAX
#       include <windows.h>
#   else
#       define NOMINMAX
#       include <windows.h>
#       undef NOMINMAX
#   endif
#
#   undef  Array
#
#endif /* OS_WIN32 */

#endif /* OS_STD_WINDOWS_H */
