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

#ifndef OS_STD_STDDEF_H
#define OS_STD_STDDEF_H

/* null, offsetof, size_t, etc... */

#include <stddef.h>

#if (defined(offsetof) || defined(_MSC_VER) || defined(__GNUC__)) && !defined(__ICCARM__)
#   define db_offsetof(struct_name, member_name) offsetof(struct_name, member_name)
#else
#   define db_offsetof(struct_name, member_name) ((size_t)&(((struct_name*)NULL)->member_name))
#endif

#endif /* OS_STD_STDDEF_H */
