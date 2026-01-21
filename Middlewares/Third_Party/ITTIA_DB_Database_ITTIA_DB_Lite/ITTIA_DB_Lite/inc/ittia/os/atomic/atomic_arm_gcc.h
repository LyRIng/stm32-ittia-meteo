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

#ifndef ATOMIC_ARM_GCC_H
#define ATOMIC_ARM_GCC_H

#ifndef _os_asm_label
#   define _os_asm_label(text) __asm__ __volatile__( "@" text ";\n\t" )
#endif

#if defined(OS_CPU_ARM) && defined(__GNUC__)

#if (OS_ARM_ARCH >= 7)
#define OS_HAVE_ATOMIC_DMB
C_INLINE_DECL void
os_atomic_native_dmb()
{
    __asm__ __volatile__("@ os_atomic_native_dmb\n\t"
                         "dmb\n");
}

#else

#define OS_HAVE_ATOMIC_DMB
C_INLINE_DECL void
os_atomic_native_dmb()
{
    int dest = 0;
    __asm__ __volatile__("mcr p15,0,%0,c7,c10,5" :"=&r"(dest) : : "memory");
}

#endif

#if OS_ARM_ARCH >= 6

#define OS_HAVE_ATOMIC
#define OS_HAVE_ATOMIC_VALUE

typedef int os_AtomicValue;
#define OS_ATOMIC_NATIVE_INIT(x) (x)
#define OS_ATOMIC_NATIVE_CTOR(x) (x)

/*
 * LDREX and STREX available in ARMv6.
 * CLREX available in ARMv6K.
 *
 * more information is available at:
 *
 * http://www.keil.com/support/man/docs/armasm/armasm_cihbghef.htm
 * http://www.keil.com/support/man/docs/armasm/armasm_cjacahbf.htm
 *
 */
/*
 * LDREX{cond} Rt, [Rn {, #offset}]
 *
 * Rt
 *   is the register to load or store.
 *
 * Rn
 *   is the register on which the memory address is based.
 *
 * offset
 *   is an optional offset applied to the value in Rn. offset is
 *   allowed only in Thumb-2 instructions. If offset is omitted,
 *   an offset of 0 is assumed.
 */

/* _os_ldrex(int * ptr, int & val) */
#define _os_ldrex(ptr, tmp)                     \
    __asm__ __volatile__ ("ldrex  %0, [%1]\n\t" \
                          : "=r" (tmp)          \
                          : "r" (ptr)           \
                          : "cc" )


/*
 * STREX{cond} Rd, Rt, [Rn {, #offset}]
 *
 * Rd
 *   is the destination register for the returned status.
 *   = 0 is store is successfull
 *   = 1 if store is cancelled
 *
 * Rt
 *   is the register to load or store.
 *
 * Rn
 *   is the register on which the memory address is based.
 *
 * offset
 *   is an optional offset applied to the value in Rn. offset is
 *   allowed only in Thumb-2 instructions. If offset is omitted,
 *   an offset of 0 is assumed.
 *
 */

/* _os_strex(int * ptr, int newval, int& status) */

#define _os_strex(ptr, newval, status)                  \
    __asm__ __volatile__( "strex   %0, %2, [%3]\n\t"    \
                          : "=&r" (status), "+m" (*ptr) \
                          : "r" (newval), "r" (ptr)     \
                          : "cc" );

/* _os_clrex() */
#if OS_ARM_ARCH >= 7
#define _os_clrex(ptr, oldval)        \
    __asm__ __volatile__ ("clrex\n\t" \
                          :           \
                          :           \
                          : "cc" )
#else

/* it's unclear if we can just abandon exclusive request after ldrex.
 * so clear access using strex - just trying to update it to the
 * previous value and ignore the result. Update would fail 
 * if concur with other process. it's okay.
 */
#define _os_clrex(ptr, oldval)                          \
    ({ int status;                                      \
    __asm__ __volatile__( "strex   %0, %2, [%3]\n\t"    \
                          : "=&r" (status), "+m" (*ptr) \
                          : "r" (oldval), "r" (ptr)     \
                          : "cc" );                     \
    })

#endif


#define OS_HAVE_ATOMIC_FETCH    ATOMIC_IPC

C_INLINE_DECL os_AtomicValue
os_atomic_native_fetch( const os_AtomicValue * ptr )
{
    return *ptr;
}

C_INLINE_DECL os_AtomicValue
os_atomic_native_fetch_lk( const os_AtomicValue * ptr )
{
    return *(os_AtomicValue*)ptr;
}

#define OS_HAVE_ATOMIC_STORE    ATOMIC_IPC
C_INLINE_DECL void
os_atomic_native_store( os_AtomicValue * ptr,
                        os_AtomicValue newval)
{
    int tmp, status;
    _os_asm_label("os_atomic_native_store");
    do {
        _os_ldrex( ptr, tmp );
        _os_strex( ptr, newval, status );
    } while( status );
    _os_asm_label("end of os_atomic_native_store");
}

C_INLINE_DECL void
os_atomic_native_store_lk( os_AtomicValue * ptr,
                           os_AtomicValue newval)
{
    *(os_AtomicValue*)ptr = newval;
}

#define OS_HAVE_ATOMIC_CAS      ATOMIC_IPC
C_INLINE_DECL db_bool_t
os_atomic_native_cas( os_AtomicValue * ptr,
                      os_AtomicValue oldval,
                      os_AtomicValue newval )
{
    int curval, status;
    _os_asm_label("os_atomic_native_cas");
    do {
        _os_ldrex( ptr, curval );
        if (curval != oldval) {
            /* clear and return */
            /* TODO cas might be implemented using strexeq */
            _os_clrex( ptr, curval );
            return DB_FALSE;
        }
        _os_strex( ptr, newval, status );
    } while( status );
    _os_asm_label("end of os_atomic_native_cas");
    return DB_TRUE;
}

#define OS_HAVE_ATOMIC_TRYLOCK      ATOMIC_IPC
C_INLINE_DECL db_bool_t
os_atomic_native_trylock( os_AtomicValue * ptr )
{
    int curval, status;
    _os_asm_label("os_atomic_native_trylock");

    do {
        _os_ldrex( ptr, curval );
        if (curval != 0) {
            /* clear and return */
            /* TODO cas might be implemented using strexeq */
            _os_clrex( ptr, curval );
            return DB_FALSE;
        }
        _os_strex( ptr, 1, status );
    } while( status );
    _os_asm_label("end of os_atomic_native_trylock");
    return DB_TRUE;
}

C_INLINE_DECL void
os_atomic_native_unlock( os_AtomicValue * ptr )
{
    int tmp, status;

    os_atomic_native_dmb();

    _os_asm_label("os_atomic_native_unlock");
    do {
        _os_ldrex( ptr, tmp );
        _os_strex( ptr, 0, status );
    } while( status );
    _os_asm_label("end of os_atomic_native_unlock");
}

C_INLINE_DECL void
os_atomic_native_unlock_lk( os_AtomicValue * ptr )
{
    *(os_AtomicValue*)ptr = 0;
}

#define OS_HAVE_ATOMIC_ADD      ATOMIC_IPC
C_INLINE_DECL os_AtomicValue
os_atomic_native_add( os_AtomicValue * ptr,
                os_AtomicValue addval )
{
    int curval, status;
    
    _os_asm_label("os_atomic_native_add");
    do {
        _os_ldrex( ptr, curval );
        _os_strex( ptr, curval + addval, status );
    } while( status );
    _os_asm_label("end of os_atomic_native_add");
    return curval + addval;
}

#define OS_HAVE_ATOMIC_INC      ATOMIC_IPC
C_INLINE_DECL os_AtomicValue
os_atomic_native_inc( os_AtomicValue * ptr )
{
    int curval, status;
    
    _os_asm_label("os_atomic_native_inc");
    do {
        _os_ldrex( ptr, curval );
        curval++;
        _os_strex( ptr, curval, status );
    } while( status );
    _os_asm_label("end of os_atomic_native_inc");
    return curval;
}

#define OS_HAVE_ATOMIC_DEC      ATOMIC_IPC
C_INLINE_DECL os_AtomicValue
os_atomic_native_dec( os_AtomicValue * ptr )
{
    int curval, status;
    
    _os_asm_label("os_atomic_native_dec");
    do {
        _os_ldrex( ptr, curval );
        curval--;
        _os_strex( ptr, curval, status );
    } while( status );
    _os_asm_label("end of os_atomic_native_dec");
    return curval;
}

#define OS_HAVE_ATOMIC_EXCH     ATOMIC_IPC
C_INLINE_DECL os_AtomicValue
os_atomic_native_exch( os_AtomicValue * ptr,
                 os_AtomicValue newval )
{
    int curval, status;
    _os_asm_label("os_atomic_native_exch");
    do {
        _os_ldrex( ptr, curval );
        _os_strex( ptr, newval, status );
    } while( status );
    _os_asm_label("end of os_atomic_native_exch");
    return curval;
}

#undef _os_ldrex
#undef _os_strex
#undef _os_clrex


#else /* OS_ARM_ARCH >= 6 */
/* pre v6 core */

#ifndef OS_HAVE_ATOMIC_VALUE
#define OS_HAVE_ATOMIC_VALUE
#define OS_ATOMIC_NATIVE_INIT(x) (x)
#define OS_ATOMIC_NATIVE_CTOR(x) (x)
typedef int os_AtomicValue;
#define os_atomic_native_fetch_lk(ptr) (*(os_AtomicValue*)(ptr))
#define os_atomic_native_store_lk(ptr, newval) (*(os_AtomicValue*)(ptr) = newval)
#endif

#define OS_HAVE_ATOMIC_FETCH        ATOMIC_IPC
C_INLINE_DECL os_AtomicValue
os_atomic_native_fetch( const os_AtomicValue * ptr )
{
    return *ptr;
}

#define OS_HAVE_ATOMIC_STORE        ATOMIC_IPC
C_INLINE_DECL void
os_atomic_native_store( os_AtomicValue * ptr,
                        os_AtomicValue value )
{
    *ptr = value;
}

#define OS_HAVE_ATOMIC_EXCH      ATOMIC_IPC
C_INLINE_DECL db_bool_t
os_atomic_native_exch( os_AtomicValue * ptr, os_AtomicValue newval )
{
    int curval;

    __asm__ __volatile__("@ os_atomic_native_exch\n\t"
                         "swp %0, %1, [%2]\n\t"
                         : "=&r" (curval)
                         : "r"(newval), "r"(ptr)
                         : "memory" );

    return curval;
}

#define OS_HAVE_ATOMIC_TRYLOCK      ATOMIC_IPC
C_INLINE_DECL db_bool_t
os_atomic_native_trylock( os_AtomicValue * ptr )
{
    int curval;

    __asm__ __volatile__("@ os_atomic_native_trylock\n\t"
                         "swp %0, %1, [%2]\n\t"
                         : "=&r" (curval)
                         : "r"(1), "r"(ptr)
                         : "memory" );

    return curval == 0;
}

C_INLINE_DECL void
os_atomic_native_unlock( os_AtomicValue * ptr )
{
    os_atomic_native_dmb();
    *ptr = 0;
}

#define os_atomic_native_unlock_lk(ptr) (*(os_AtomicValue*)(ptr) = 0)


#endif /* OS_ARM_ARCH */

#endif /* OS_CPU_ARM */

#endif /* ATOMIC_ARM_GCC_H */

/* TODO: needs to investigate
 * ARM SWP instruction contains full memory barrier.
 * It seems LDREX/STREX contains no barrier.
 */
