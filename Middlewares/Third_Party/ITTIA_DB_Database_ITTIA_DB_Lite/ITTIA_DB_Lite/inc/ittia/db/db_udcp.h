/**************************************************************************/
/*                                                                        */
/*      Copyright (c) 2018-2025 by ITTIA L.L.C. All rights reserved.      */
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

#include <ittia/db/db_environment.h>
#include <ittia/db/db_index_storage.h>

C_HEADER_BEGIN

/** User-defined connection protocol plugin handle. */
DB_DECLARE_HANDLE(db_udcp_handle_t);

typedef struct db_udcp_poll_handle_s
{
    db_udcp_handle_t    handle;
    short               events;
    short               revents;

    /* events & revents */
#define DB_UDCP_POLLIN      0x0001u
#define DB_UDCP_POLLPRI     0x0002u
#define DB_UDCP_POLLOUT     0x0004u

    /* revents only */
#define DB_UDCP_POLLERR     0x0100u
#define DB_UDCP_POLLHUP     0x0200u
#define DB_UDCP_POLLNVAL    0x0400u
} db_udcp_poll_handle_t;

typedef struct db_udcp_info_s
{
    const db_fname_t *  udcp_proto_name;
    int                 udcp_capabilities;
#define DB_UDCP_USER_AUTH    1

    int (*udcp_initialize)(void);
    int (*udcp_finalize)(void);

    int (*udcp_close)( db_udcp_handle_t );

    int (*udcp_listen)(db_udcp_handle_t*, void * udcp_param);
    int (*udcp_accept)(db_udcp_handle_t, db_udcp_handle_t *);
    int (*udcp_cancel)(db_udcp_handle_t);

    int (*udcp_connect)( db_udcp_handle_t*, void * udcp_param, const db_fname_t * host, const db_fname_t * port,
        const db_fname_t * user_name, const db_fname_t * password);

    int (*udcp_recv)( db_udcp_handle_t, void * buffer, size_t buffer_size, int timeout, size_t * bytes_recvd, int * time_rest );
    int (*udcp_send)( db_udcp_handle_t, const void * buffer, size_t buffer_size, size_t * bytes_sent );

    int (*udcp_poll)(db_udcp_poll_handle_t *, size_t nfds, int * timeout );
    int (*udcp_alive)(db_udcp_handle_t);

    /* reserved pointers. Should be set to NULL */
    void * udcp_reserved[4];    /* int (*udcp_wait_handle)(db_udcp_handle_t, uintptr_t *); */

} db_udcp_info_t;

typedef struct
{
    const char  * udcp_proto_name;
    void        * udcp_param;
} db_udcp_listen_t;

DBDLL_API db_result_t               db_udcp_register(const db_udcp_info_t *);
DBDLL_API db_result_t               db_udcp_unregister(const db_fname_t * udcp_proto_name);
DBDLL_API const db_udcp_info_t *    db_udcp_find(const db_fname_t * udcp_proto_name);

C_HEADER_END
