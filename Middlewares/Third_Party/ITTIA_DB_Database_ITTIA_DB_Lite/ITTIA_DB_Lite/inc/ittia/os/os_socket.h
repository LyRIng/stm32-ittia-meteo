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

#ifndef OS_SOCKET_H
#define OS_SOCKET_H

#include "ittia/os/os_config.h"

#if defined(HAVE_WIN32_SOCKET)
#   include "ittia/os/win32/win32_socket.h"
#elif defined(HAVE_POSIX_SOCKET)
#   include "ittia/os/posix/posix_socket.h"
#endif

#include "ittia/os/os_wait_time.h"

C_HEADER_BEGIN

DBDLL_API int os_socket_startup();
DBDLL_API int os_socket_cleanup();


/* socket connection api */

#define OS_SOCK_REUSEADDR       0x00001u
#define OS_SOCK_OOBINLINE       0x00002u
#define OS_SOCK_KEEPALIVE       0x00004u
#define OS_SOCK_NONBLOCK        0x00008u

#define OS_SOCK_TCP_NODELAY     0x10000u

typedef u_long  Ip4Host;
typedef u_short Ip4Port;

typedef struct
{
    Ip4Host host;
    Ip4Port port;
} Ip4Addr;

DBDLL_API int os_tcp4_listen( os_socket_t * sock, Ip4Host, Ip4Port, int flags );
DBDLL_API int os_tcp4_accept( os_socket_t sock, os_socket_t * rsock, Ip4Addr * peer_addr, int flags );

DBDLL_API int os_tcp4_connect(os_socket_t * sock,
                              const char * host_name, Ip4Port host_port,
                              const Ip4Addr * bind,
                              int flags,
                              os_wait_time_t time_wait,
                              os_wait_time_t * time_left);
/* socket stream */
DBDLL_API int os_socket_close( os_socket_t );
DBDLL_API int os_socket_recv( os_socket_t, void *, int size, os_wait_time_t, os_wait_time_t * );
DBDLL_API int os_socket_send( os_socket_t, const void *, int size );
DBDLL_API int os_socket_alive( os_socket_t );

C_HEADER_END

#endif /* OS_SOCKET_H */

