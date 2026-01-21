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

#ifndef OS_X86_ARCH_H
#define OS_X86_ARCH_H

/* detect major architecture version */

#ifdef __GNUC__

#if __GNUC__ < 2
#   error "your compiler version is not supported"
#endif

#ifdef __i386__

#ifndef OS_CPU_X86
#   define OS_CPU_X86	1
#endif

#ifndef OS_CPU_LEN
#   define OS_CPU_LEN 32
#endif

/* currently we classify these over these classes:
 *	3 = 386
 *	4 = 486
 *	5 = pentium
 *	6 = pentium pro and later
 * various variarion would be defined using extra macros
 */

#if defined(OS_X86_ARCH)
	/* use supplied settings */
#elif defined( __i686__ ) || defined(__tune_i686__)
#	define OS_X86_ARCH	6
#elif defined(__i586__) || defined(__tune_i586__)
#	define OS_X86_ARCH	5
#elif defined(__i486__) || defined(__tune_i485__)
#	define OS_X86_ARCH	4
#elif defined(__tune__core2__)         \
	|| defined( __tune_geode__ ) \
	|| defined( __tune_amdfam10__ ) \
	|| defined( __tune_k8__ ) \
	|| defined( __tune_athlon__ ) \
	|| defined( __tune_k6__ ) \
	|| defined( __tune_nocona__ ) \
	|| defined( __tune_pentium4__ ) \
	|| defined( __tune_corei3__ ) \
	|| defined( __tune_corei5__ ) \
	|| defined( __tune_corei7__ ) \
	|| defined( __tune_atom__ )

#	define OS_X86_ARCH	6

#elif defined(__i486__) || defined(__tune_i486__)

#	define OS_X86_ARCH	4

#elif ((__GNUC__ >= 3) && defined(__tune_i386__)) \
    || ((__GNUC__ == 2) && defined(__i386__)) \
    || (defined(__QNX__) && defined(__i386__))

#	define OS_X86_ARCH	3

#else
 /* __i386__ is always defined, besides we should better
  * add new targets to the list above */

#	error "unknown CPU type, investigate"

#endif /* x86 arch */

#elif defined(__x86_64__) /* __i386__ */

#ifndef OS_CPU_X86
#   define OS_CPU_X86	1
#endif
#define OS_X86_ARCH	6

#ifndef OS_CPU_LEN
#   define OS_CPU_LEN 64
#endif

#endif /* __x86_64__ */

#endif /* __GNUC__ */


#if defined(_MSC_VER) && defined(_M_IX86)

#ifndef OS_CPU_X86
#   define OS_CPU_X86	1
#endif

#if defined(OS_X86_ARCH)
#elif _M_IX86 == 600
#	define OS_X86_ARCH	6
#elif _M_IX86 == 500
#	define OS_X86_ARCH	5
#elif _M_IX86 == 400
#	define OS_X86_ARCH	4
#elif _M_IX86 == 300
#	define OS_X86_ARCH	3
#else
#error "unknown CPU architecture"
#endif

#endif /* _MSC_VER && defined(_M_IX86) */

#if defined(_MSC_VER) && defined(_M_X64)

#ifndef OS_CPU_X86
#   define OS_CPU_X86	1
#endif
#define OS_X86_ARCH	6

#ifndef OS_CPU_LEN
#   define OS_CPU_LEN 64
#endif

#endif /* _MSC_VER && defined(_M_X64) */

#endif /* OS_X86_ARCH_H */
