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

#ifndef OS_ARM_ARCH_H
#define OS_ARM_ARCH_H

/* detect major architecture version */

#if defined(__GNUC__) && defined(__arm__) && !defined(__ghs__)

#ifndef OS_CPU_ARM
#   define OS_CPU_ARM	1
#endif

#ifdef OS_CPU_LEN
/* use supplied setting */
#elif defined(__aarch64__)
#   define OS_CPU_LEN 64
#else
#   define OS_CPU_LEN 32
#endif

#if defined(OS_ARM_ARCH)
/* use supplied setting */
#elif defined(__ARM_ARCH_2__)
#	define OS_ARM_ARCH	2
#elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
#	define OS_ARM_ARCH	3
#elif defined(__ARM_ARCH_4__) || defined(__ARM_ARCH_4T__)
#	define OS_ARM_ARCH	4
#elif defined(__ARM_ARCH_5__) || defined(__ARM_ARCH_5T__) || defined(__ARM_ARCH_5TE__)
#	define OS_ARM_ARCH	5
#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6T2__) || defined(__ARM_ARCH_6M__)
#	define OS_ARM_ARCH	6
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
#	define OS_ARM_ARCH	7
#elif defined(__ARM_ARCH)
#	define OS_ARM_ARCH	__ARM_ARCH
#else

#error "unknown ARM architecture, please investigate"

#endif /* ARM arch */

#elif defined(__ghs__) && defined(__ARM__)

#ifndef OS_CPU_ARM
#   define OS_CPU_ARM	1
#endif

#if defined(OS_ARM_ARCH)

#elif defined(__ARM7tm__) || defined(__ARM8__) || defined(__ARM9__) || defined(__StrongARM__)
#	define OS_ARM_ARCH	4
#elif defined(__ARM9E__) || defined(__ARM10__) || defined(__XScale__) || defined(__ARM11__) || defined(__ARM11T2) || defined(__CortexR4) || defined(__CORE_CORTEXR5__) || defined(__CORE_CORTEXR7__) || defined(__CortexA5)
#	define OS_ARM_ARCH	5
#elif defined(__CortexM0) || defined(__CORE_CORTEXM0PLUS__) || defined(__CortexM1) || defined(__PJ4v6)
#	define OS_ARM_ARCH	6
#elif defined(__PJ4) || defined(__CortexM3) || defined(__CortexM4) || defined(__CORE_CORTEXM4F__) || defined(__CORE_CORTEXR4F__) || defined(__CORE_CORTEXA7__) || defined(__CortexA8) || defined(__CortexA9) || defined(__CORE_CORTEXA12__) || defined(__CortexA15) || defined(__CORE_CORTEXA17__)
#	define OS_ARM_ARCH	7
#elif defined(__CORE_CORTEXA53__) || defined(__CORE_CORTEXA57__)
#	define OS_ARM_ARCH	8
#else

#error "unknown ARM architecture, please investigate"

#endif /* ARM arch */

#elif defined(__IAR_SYSTEMS_ICC__) && defined(__ICCARM__) && __ICCARM__ == 1

#ifndef OS_CPU_ARM
#   define OS_CPU_ARM	1
#endif

#ifndef OS_CPU_LEN
#   define OS_CPU_LEN 32
#endif

#if defined(OS_ARM_ARCH)

#elif __CORE__ == __ARM4M__ || __CORE__ == __ARM4TM__
#	define OS_ARM_ARCH	4
#elif __CORE__ == __ARM5__ || __CORE__ == __ARM5E__
#	define OS_ARM_ARCH	5
#elif __CORE__ == __ARM6__ || __CORE__ == __ARM6M__ || __CORE__ == __ARM6SM__
#	define OS_ARM_ARCH	6
#elif __CORE__ == __ARM7M__ || __CORE__ == __ARM7EM__ || __CORE__ == __ARM7A__ || __CORE__ == __ARM7R__
#	define OS_ARM_ARCH	7
#elif __CORE__ == __ARM8A__ || __CORE__ == __ARM8M_BASELINE__ || __CORE__ == __ARM8M_MAINLINE__ || __CORE__ == __ARM8R__ || __CORE__ == __ARM8EM_MAINLINE__
#	define OS_ARM_ARCH	8
#else

#error "unknown ARM architecture, please investigate"

#endif /* ARM arch */

#elif defined(_WIN32_WCE) && defined(ARM)

#ifndef OS_CPU_ARM
#   define OS_CPU_ARM	1
#endif

#ifndef OS_CPU_LEN
#   define OS_CPU_LEN 32
#endif

#endif /* compiler */

#endif /* OS_ARM_ARCH_H */
