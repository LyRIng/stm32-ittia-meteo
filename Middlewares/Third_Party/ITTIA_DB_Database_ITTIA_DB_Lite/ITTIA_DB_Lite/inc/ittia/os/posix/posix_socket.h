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

#ifndef POSIX_SOCKET_H
#define POSIX_SOCKET_H

#include "ittia/os/os_config.h"

#if defined(HAVE_POSIX_SOCKET)

#include <sys/types.h>
#include <sys/socket.h>
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef int os_socket_t;
#define OS_INVALID_SOCKET -1

#ifdef OS_VXWORKS
#include <sockLib.h>
#if 0
// Not available for NTSIM target?
#include <wrapper/wrapperHostLib.h>
#endif
#endif

#endif /* HAVE_POSIX_SOCKET */

#endif /* POSIX_SOCKET_H */
