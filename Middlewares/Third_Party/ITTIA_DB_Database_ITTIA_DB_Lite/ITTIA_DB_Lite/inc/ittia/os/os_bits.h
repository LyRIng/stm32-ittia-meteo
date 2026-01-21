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

#ifndef OS_BITS_H
#define OS_BITS_H

#include <ittia/os/os_config.h>
#include <ittia/os/std/windows.h>
#include <ittia/os/std/limits.h>

#if defined(_MSC_VER) && _MSC_VER >= 1500 && \
    (defined(_M_IX86) || defined(_M_AMD64) || defined(_M_IA64) || defined(_M_X64))
#   pragma intrinsic(_BitScanReverse)
#   ifdef _M_X64
#       pragma intrinsic(_BitScanReverse64)
#   endif
#   define HAVE_MS_BIT_SCAN
#endif


#if defined(__GNUC__) && !defined(__cplusplus)
#   define WEAK_TYPES_COMPATIBLE(type1, type2)   (__builtin_types_compatible_p(type1, type2))
#else
#   define WEAK_TYPES_COMPATIBLE(type1, type2)   (sizeof(type1) == sizeof(type2))
#endif

/* returns MSB non-zero bit position (0..31) */
C_INLINE_DECL int
os_highbit(uintptr_t x)
{
#if defined(__GNUC__) && defined(__i386__) && !defined(OS_VXWORKS)
#define FAST_HIGHBIT
    size_t bit;

    if (x == 0)
        return -1;
	else if (WEAK_TYPES_COMPATIBLE(uintptr_t, unsigned int))
		bit = sizeof(x) * CHAR_BIT - __builtin_clz((unsigned int)x) - 1;
	else if (WEAK_TYPES_COMPATIBLE(uintptr_t, unsigned long))
		bit = sizeof(x) * CHAR_BIT - __builtin_clzl((unsigned long)x) - 1;
	else if (WEAK_TYPES_COMPATIBLE(uintptr_t, unsigned long long))
		bit = sizeof(x) * CHAR_BIT - __builtin_clzll((unsigned long long)x) - 1;
	else {
		DB_ASSERT(0);
		return 0;
	}
	return (int)bit;

#elif defined(HAVE_MS_BIT_SCAN)
#define FAST_HIGHBIT

    unsigned long index;

    if (x == 0)
        return -1;
    else if (sizeof x == sizeof(unsigned long))
        _BitScanReverse( &index, x );
#ifdef _M_X64
    else if (sizeof x == sizeof(unsigned __int64))
        _BitScanReverse64( &index, x );
#endif
	else {
		DB_ASSERT(0);
		return 0;
	}

    return (int)index;
	
#else
    /* msvc got an _BitScanReverse intrinsic */
	/* find the high bit set (naive way for awhile) */
	int bit = 0;
    if (x == 0)
        return -1;
	while (x > 0xffffu) { x >>= 16; bit += 16; }
	if (x > 0x00ffu)    { x >>= 8;  bit += 8; }
	if (x > 0x000fu)    { x >>= 4;  bit += 4; }
	if (x > 0x0003u)    { x >>= 2;  bit += 2; }
	if (x > 0x0001u)    {           bit += 1; }
	return bit;
#endif
}

/* returns LSB non-zero bit position (0..31) */
C_INLINE_DECL int
os_lowbit(uintptr_t x)
{
#if defined(__GNUC__) && defined(__i386__) && !defined(OS_VXWORKS)

#define FAST_LOWBIT
    size_t bit;

    if (x == 0)
        return -1;
	else if (WEAK_TYPES_COMPATIBLE(uintptr_t, unsigned int))
		bit = __builtin_ctz((unsigned int)x);
	else if (WEAK_TYPES_COMPATIBLE(uintptr_t, unsigned long))
		bit = __builtin_ctzl((unsigned long)x);
	else if (WEAK_TYPES_COMPATIBLE(uintptr_t, unsigned long long))
		bit = __builtin_ctzll((unsigned long long)x);
	else {
		DB_ASSERT(0);
		return 0;
	}

    return bit;
	
#elif defined(HAVE_MS_BIT_SCAN)
#define FAST_LOWBIT

    unsigned long index;

    if (x == 0)
        return -1;
    else if (sizeof(x) == sizeof(unsigned long))
        _BitScanForward( &index, x );
#ifdef _M_X64 
    else if (sizeof(x) == sizeof(unsigned __int64))
        _BitScanForward64( &index, x );
#endif
	else {
		DB_ASSERT(0);
		return 0;
	}

    return (int)index;

#else
	int bit = 0;
	
    if (x == 0)
        return -1;
	while ((x & 0xffffu) == 0) { bit += 16; x >>= 16; }
	if ((x & 0x00ffu) == 0) { bit +=  8; x >>=  8; }
	if ((x & 0x000fu) == 0) { bit +=  4; x >>=  4; }
	if ((x & 0x0003u) == 0) { bit +=  2; x >>=  2; }
	if ((x & 0x0001u) == 0) { bit +=  1;           }

    return bit;
#endif
}

C_INLINE_DECL uintptr_t
os_lowbit_mask(size_t nbits)
{
    return nbits == 0 ? 0 : (~(uintptr_t)0) >> (sizeof(uintptr_t) * CHAR_BIT - nbits);
}

C_INLINE_DECL uintptr_t
os_highbit_mask(size_t nbits)
{
    return nbits == 0 ? 0 : (~(uintptr_t)0) << (sizeof(uintptr_t) * CHAR_BIT - nbits);
}

#endif /* OS_BITS_H */
