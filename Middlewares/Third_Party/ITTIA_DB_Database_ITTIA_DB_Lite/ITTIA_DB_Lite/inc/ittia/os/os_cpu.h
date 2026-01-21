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

#ifndef ITTIA_OS_CPU_H
#define ITTIA_OS_CPU_H

/* detect target CPU */

/*
 *	currently we support following CPUs:
 *
 *	x86 (gcc, msvc)
 *		updated to gcc 4.4 and msvs 9
 *
 *	arm (gcc)
 *		updated to gcc 4.4
 *
 */

#include "ittia/os/os_arm_arch.h"
#include "ittia/os/os_x86_arch.h"

#if !defined(OS_CPU_ARM) \
	&& !defined(OS_CPU_X86)

#   define OS_CPU_GENERIC	1

#endif

#if defined(OS_CPU_ARM) + defined(OS_CPU_X86) + defined(OS_CPU_GENERIC) != 1
#   error "cleanup CPU support"
#endif

#endif /* ITTIA_OS_CPU_H */
