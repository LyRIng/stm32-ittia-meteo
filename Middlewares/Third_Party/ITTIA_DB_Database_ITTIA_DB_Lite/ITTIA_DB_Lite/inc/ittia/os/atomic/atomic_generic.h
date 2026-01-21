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

#ifndef ATOMIC_GENERIC_H
#define ATOMIC_GENERIC_H

/* complete atomic implementation with generic support */
/* we consider these native since they use only native CPU features to complement */

#ifdef OS_HAVE_ATOMIC

/*** os_atomic_native_exch **********************************************/
#if !defined(OS_HAVE_ATOMIC_EXCH) \
        && (OS_HAVE_ATOMIC_CAS >= ATOMIC_IPC)

#   define OS_HAVE_ATOMIC_EXCH ATOMIC_IPC

C_INLINE_DECL os_AtomicValue
os_atomic_native_exch(os_AtomicValue * ptr,
                       os_AtomicValue newval)
{
    os_AtomicValue oldval;
    do {
        oldval = os_atomic_native_fetch(ptr);
    } while( !os_atomic_native_cas(ptr, oldval, newval) );
    return oldval;

}

#endif

/*** os_atomic_native_add ***********************************************/
#if !defined(OS_HAVE_ATOMIC_ADD) \
        && defined(OS_HAVE_ATOMIC_CAS) \
        && (OS_HAVE_ATOMIC_CAS >= ATOMIC_IPC)

#   define OS_HAVE_ATOMIC_ADD      ATOMIC_IPC

C_INLINE_DECL os_AtomicValue
os_atomic_native_add( os_AtomicValue * ptr,
                os_AtomicValue addval )
{
    os_AtomicValue curval;
    os_AtomicValue newval;
    do {
        curval = os_atomic_native_fetch(ptr);
        newval = curval + addval;
    } while ( !os_atomic_native_cas(ptr, curval, newval) );
    return newval;
}

#endif


/*** os_atomic_native_inc ***********************************************/
#if !defined(OS_HAVE_ATOMIC_INC) \
        && defined(OS_HAVE_ATOMIC_ADD) \
        && (OS_HAVE_ATOMIC_ADD >= ATOMIC_IPC)

#   define OS_HAVE_ATOMIC_INC      OS_HAVE_ATOMIC_ADD

C_INLINE_DECL os_AtomicValue
os_atomic_native_inc( os_AtomicValue * ptr )
{
    return os_atomic_native_add( ptr, 1 );
}

#endif

/*** os_atomic_native_dec ***********************************************/
#if !defined(OS_HAVE_ATOMIC_DEC) \
        && defined(OS_HAVE_ATOMIC_ADD) \
        && (OS_HAVE_ATOMIC_ADD >= ATOMIC_IPC)

#   define OS_HAVE_ATOMIC_DEC      OS_HAVE_ATOMIC_ADD

C_INLINE_DECL os_AtomicValue
os_atomic_native_dec( os_AtomicValue * ptr )
{
    return os_atomic_native_add(ptr, - 1);
}

#endif

/*** os_atomic_native_vinc **********************************************/

#ifndef OS_HAVE_ATOMIC_VINC
#   if defined(OS_HAVE_ATOMIC_VADD) && (OS_HAVE_ATOMIC_VADD >= ATOMIC_IPC)

#       define OS_HAVE_ATOMIC_VINC OS_HAVE_ATOMIC_VADD
#       define os_atomic_native_vinc(ptr) os_atomic_native_vadd(ptr, 1)

#   elif defined(OS_HAVE_ATOMIC_ADD) && OS_HAVE_ATOMIC_ADD >= ATOMIC_IPC

#       define OS_HAVE_ATOMIC_VINC OS_HAVE_ATOMIC_ADD
#       define os_atomic_native_vinc(ptr) (void)os_atomic_native_add(ptr, 1)

#   endif
#endif

/*** os_atomic_native_vdec **********************************************/

#ifndef OS_HAVE_ATOMIC_VDEC
#   if defined(OS_HAVE_ATOMIC_VADD) && OS_HAVE_ATOMIC_VADD >= ATOMIC_IPC

#       define OS_HAVE_ATOMIC_VDEC OS_HAVE_ATOMIC_VADD
#       define os_atomic_native_vdec(ptr) os_atomic_native_vadd(ptr, -1)

#   elif defined(OS_HAVE_ATOMIC_ADD) && OS_HAVE_ATOMIC_ADD >= ATOMIC_IPC

#       define OS_HAVE_ATOMIC_VDEC OS_HAVE_ATOMIC_ADD
#       define os_atomic_native_vdec(ptr) (void)os_atomic_native_add(ptr, -1)

#   endif
#endif

/*** os_atomic_native_vadd **********************************************/

#ifndef OS_HAVE_ATOMIC_VADD
#   if defined(OS_HAVE_ATOMIC_ADD) && OS_HAVE_ATOMIC_ADD >= ATOMIC_IPC

#       define OS_HAVE_ATOMIC_VADD  OS_HAVE_ATOMIC_ADD
#       define os_atomic_native_vadd(ptr, add_val) (void)os_atomic_native_add(ptr, add_val)

#   endif
#endif

/*** os_atomic_native_trylock *******************************************/

#ifndef OS_HAVE_ATOMIC_TRYLOCK
#   if defined(OS_HAVE_ATOMIC_EXCH) && OS_HAVE_ATOMIC_EXCH >= ATOMIC_IPC

#       define OS_HAVE_ATOMIC_TRYLOCK           OS_HAVE_ATOMIC_EXCH
#       define os_atomic_native_trylock(ptr)    (os_atomic_native_exch( ptr, 1 ) == 0)
#       define os_atomic_native_unlock(ptr)     os_atomic_native_exch( ptr, 0 )
#       define os_atomic_native_unlock_lk(ptr)  (*(ptr) = 0)

#   elif defined(OS_HAVE_ATOMIC_CAS) && OS_HAVE_ATOMIC_CAS >= ATOMIC_IPC

#       define OS_HAVE_ATOMIC_TRYLOCK           OS_HAVE_ATOMIC_CAS
#       define os_atomic_native_trylock(ptr)    os_atomic_native_cas( ptr, 0, 1 )
#       define os_atomic_native_unlock(ptr)     os_atomic_native_cas( ptr, 1, 0 )
#       define os_atomic_native_unlock_lk(ptr)  (*(ptr) = 0)

#   endif
#endif

/*** os_atomic_native_dmb ***********************************************/

#ifndef OS_HAVE_ATOMIC_DMB

#   if defined(OS_HAVE_ATOMIC_CAS) && OS_HAVE_ATOMIC_CAS >= ATOMIC_IPC
    C_INLINE_DECL void
    os_atomic_native_dmb()
    {
        os_AtomicValue temp = 0;
    
        os_atomic_native_cas(&temp, 0, 0);
    }
#   endif

#endif

/*** end ****************************************************************/

#endif /* OS_HAVE_ATOMIC */

#endif /* ATOMIC_GENERIC_H */
