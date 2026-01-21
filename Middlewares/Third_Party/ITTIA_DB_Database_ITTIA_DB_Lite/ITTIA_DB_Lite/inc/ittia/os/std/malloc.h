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

#ifndef OS_STD_MALLOC_H
#define OS_STD_MALLOC_H

#include "ittia/os/os_env.h"

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#ifdef OS_VXWORKS_55
#include <alloc.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#if defined(OS_VXWORKS) && defined(__GNUC__) && !defined(OS_MAPUSOFT)
#   define alloca   __builtin_alloca
#endif

/* On platforms that do not support locales limit multi-byte characters to 1 byte. */
#ifndef MB_CUR_MAX
#define MB_CUR_MAX 1
#endif

#endif /* OS_STD_MALLOC_H */
