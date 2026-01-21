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

#ifndef ATOMIC_X86_GCC_H
#define ATOMIC_X86_GCC_H

/* native gcc x86 support */

#if defined(OS_CPU_X86) && defined(__GNUC__)

#if (OS_X86_ARCH < 3) || (OS_X86_ARCH > 6)
#   error "CPU is not supported, please add support here"
#endif

#ifdef OS_DISABLE_SMP
#	define OS_X86_SMP_PREFIX	""
#else
#	define OS_X86_SMP_PREFIX	"lock; "
#endif

#if (OS_X86_ARCH >= 3)

#ifndef OS_HAVE_ATOMIC
#	define OS_HAVE_ATOMIC
#endif

#ifndef OS_HAVE_ATOMIC_VALUE
#	define OS_HAVE_ATOMIC_VALUE	1
typedef int os_AtomicValue;
#define os_atomic_native_fetch_lk(ptr)		(*(ptr))
#define os_atomic_native_store_lk(ptr, val)	(*(ptr) = (val))
#endif

#ifndef OS_HAVE_ATOMIC_FETCH
#   define OS_HAVE_ATOMIC_FETCH		ATOMIC_IPC

C_INLINE_DECL os_AtomicValue
os_atomic_native_fetch(const os_AtomicValue * ptr)
{
	os_AtomicValue curval;
	__asm__ __volatile__(
		"mov %1, %0\n\t"
		: "=r" (curval)
		: "m" (*ptr)
		: "memory");
	return curval;
}
#endif

#ifndef OS_HAVE_ATOMIC_STORE
#   define OS_HAVE_ATOMIC_STORE		ATOMIC_IPC

C_INLINE_DECL void
os_atomic_native_store(os_AtomicValue * ptr, os_AtomicValue newval)
{
	__asm__ __volatile__(
		"mov %1, %0\n\t"
		: /* no outputs */
		: "m" (*ptr), "r" (newval)
		: "memory");
}
#endif

#ifndef OS_HAVE_ATOMIC_EXCH
#   define OS_HAVE_ATOMIC_EXCH  ATOMIC_IPC

C_INLINE_DECL os_AtomicValue
os_atomic_native_exch( os_AtomicValue * ptr,
                       os_AtomicValue newval )
{
	/* xchg locks automatically */
	__asm__ __volatile__(
		"xchg %1, %0\n\t"
		: "=r" (newval)
		: "m" (*ptr), "0" (newval)
		: "memory");
	return newval;
}
#endif

#if (OS_X86_ARCH >= 4)

#ifndef OS_HAVE_ATOMIC_CAS
#   define OS_HAVE_ATOMIC_CAS   ATOMIC_IPC

C_INLINE_DECL db_bool_t
os_atomic_native_cas(os_AtomicValue * ptr,
                     os_AtomicValue oldval,
                     os_AtomicValue newval)
{
	os_AtomicValue curval;
	__asm__ __volatile__(
		OS_X86_SMP_PREFIX "cmpxchg %2, %1\n\t"
		: "=a" (curval)
		: "m" (*ptr), "r" (newval), "0"(oldval)
		: "memory", "cc");

	return curval == oldval;
}
#endif

#ifndef OS_HAVE_ATOMIC_CAS_PTR
#   define OS_HAVE_ATOMIC_CAS_PTR   ATOMIC_IPC

C_INLINE_DECL db_bool_t
os_atomic_native_cas_ptr(void ** ptr,
                     void * oldval,
                     void * newval)
{
	void * curval;
	__asm__ __volatile__(
		OS_X86_SMP_PREFIX "cmpxchg %2, %1\n\t"
		: "=a" (curval)
		: "m" (*ptr), "r" (newval), "0"(oldval)
		: "memory", "cc");

	return curval == oldval;
}

#endif

#ifndef OS_HAVE_ATOMIC_ADD
#   define OS_HAVE_ATOMIC_ADD ATOMIC_IPC

C_INLINE_DECL os_AtomicValue
os_atomic_native_add(os_AtomicValue * ptr,
                     os_AtomicValue addval)
{
	os_AtomicValue curval;
	__asm__ __volatile__(
		OS_X86_SMP_PREFIX "xadd %1, %0\n\t"
		: "+m" (*ptr), "=r" (curval)
		: "1" (addval)
		: "memory", "cc");

	return curval + addval;
}
#endif

#ifndef OS_HAVE_ATOMIC_INC
#   define OS_HAVE_ATOMIC_INC ATOMIC_IPC

C_INLINE_DECL os_AtomicValue
os_atomic_native_inc(os_AtomicValue * ptr)
{
	os_AtomicValue curval;
	__asm__ __volatile__(
		OS_X86_SMP_PREFIX "xadd %1, %0\n\t"
		: "+m" (*ptr), "=r" (curval)
		: "1" (1)
		: "memory", "cc");

	return curval + 1;
}
#endif

#ifndef OS_HAVE_ATOMIC_DEC
#   define OS_HAVE_ATOMIC_DEC ATOMIC_IPC

C_INLINE_DECL os_AtomicValue
os_atomic_native_dec(os_AtomicValue * ptr)
{
	os_AtomicValue curval;
	__asm__ __volatile__(
		OS_X86_SMP_PREFIX "xadd %1, %0\n\t"
		: "+m" (*ptr), "=r" (curval)
		: "1" (-1)
		: "memory", "cc");

	return curval - 1;
}
#endif

#if 0 /* mfence requires sse2 and doesn't seem to be very useful just now. Rely on emulation */
#if !defined(OS_HAVE_ATOMIC_DMB) && defined(OS_X86_SSE) && OS_X86_SSE >= 2
#   define OS_HAVE_ATOMIC_DMB   ATOMIC_IPC
C_INLINE_DECL void
os_atomic_native_dmb()
{
	__asm__ __volatile__(
		"mfence\n\t"
		: /* no outputs */
		: /* no inputs */
		: "memory");
}
#endif
#endif
#endif

/* void functions */

#ifndef OS_HAVE_ATOMIC_VADD
#   define OS_HAVE_ATOMIC_VADD  ATOMIC_IPC
C_INLINE_DECL void
os_atomic_native_vadd(os_AtomicValue * ptr, os_AtomicValue addval)
{
	__asm__ __volatile__(
		OS_X86_SMP_PREFIX "addl %1, %0 \n\t"
		: /* no outputs */
		: "m" (*ptr), "r"(addval)
		: "memory", "cc");
}
#endif

#ifndef OS_HAVE_ATOMIC_VINC
#   define OS_HAVE_ATOMIC_VINC ATOMIC_IPC

C_INLINE_DECL void
os_atomic_native_vinc(os_AtomicValue * ptr)
{
	__asm__ __volatile__(
		OS_X86_SMP_PREFIX "incl %0\n\t"
		: /* no outputs */
		: "m" (*ptr)
		: "memory", "cc");
}
#endif

#ifndef OS_HAVE_ATOMIC_VDEC
#   define OS_HAVE_ATOMIC_VDEC  ATOMIC_IPC
C_INLINE_DECL void
os_atomic_native_vdec(os_AtomicValue * ptr)
{
	__asm__ __volatile__(
		OS_X86_SMP_PREFIX "decl %0\n\t"
		: /* no outputs */
		: "m" (*ptr)
		: "memory", "cc");
}
#endif

#endif /* OS_X86_ARCH >= 3 */

#endif /* defined(OS_CPU_X86) && defined(__GNUC__) */

#endif /* ATOMIC_X86_GCC_H */
