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

#ifndef ATOMIC_SU_H
#define ATOMIC_SU_H

#ifdef DB_ST

#define OS_HAVE_ATOMIC
#define OS_HAVE_ATOMIC_VALUE

#define OS_ATOMIC_NATIVE_INIT(x) (x)
#define OS_ATOMIC_NATIVE_CTOR(x) (x)
typedef int os_AtomicValue;

#define OS_HAVE_ATOMIC_FETCH    ATOMIC_LOCAL

#define os_atomic_native_fetch( ptr ) (*(os_AtomicValue*)(ptr))
#define os_atomic_native_fetch_lk( ptr ) (*(os_AtomicValue*)(ptr))

#define OS_HAVE_ATOMIC_STORE    ATOMIC_LOCAL
#define os_atomic_native_store( ptr, value)    (*(os_AtomicValue*)(ptr) = value)
#define os_atomic_native_store_lk( ptr, value) (*(os_AtomicValue*)(ptr) = value)

#define OS_HAVE_ATOMIC_CAS      ATOMIC_LOCAL
C_INLINE_DECL db_bool_t
os_atomic_native_cas( os_AtomicValue * ptr, os_AtomicValue oldval, os_AtomicValue newval )
{
	os_AtomicValue curval = *ptr;
	if (*ptr == oldval) {
		*ptr = newval;
		return true;
	}
	return false;
}

#define OS_HAVE_ATOMIC_INC      ATOMIC_LOCAL
#define os_atomic_native_inc( ptr )   (++(os_AtomicValue*)(*ptr))

#define OS_HAVE_ATOMIC_DEC      ATOMIC_LOCAL
#define os_atomic_native_dec( ptr )   (--(os_AtomicValue*)(*ptr))

#define OS_HAVE_ATOMIC_ADD      ATOMIC_LOCAL
C_INLINE_DECL os_AtomicValue
os_atomic_native_add( os_AtomicValue * ptr, os_AtomicValue addval )
{
	os_AtomicValue newval = *ptr + addval;
	*ptr = newval;
	return newval;
}

#define OS_HAVE_ATOMIC_EXCH     ATOMIC_LOCAL

C_INLINE_DECL os_AtomicValue
os_atomic_native_exch( os_AtomicValue * ptr, os_AtomicValue newval )
{
	os_AtomicValue curval = *ptr;
	*ptr = newval;
	return curval;
}


#define OS_HAVE_ATOMIC_TRYLOCK  ATOMIC_LOCAL
#define os_atomic_native_trylock(ptr)    (os_atomic_native_exch(ptr, 1) == 0)
#define os_atomic_native_unlock(ptr)     ((void)(*(os_AtomicValue*)(ptr) = 0))
#define os_atomic_native_unlock_lk(ptr)  os_atomic_native_unlock(ptr)

#endif

#endif /* ATOMIC_SU_H */
