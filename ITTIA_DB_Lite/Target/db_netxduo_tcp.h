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

/** @file db_netxduo_tcp.h
 *
 * NetX Duo TCP plugin for ITTIA DB.
 *
 * This plugin provides a user-defined connection protocol (UDCP) for ITTIA DB
 * that uses the NetX Duo TCP interface.
 */

#ifndef DB_NETXDUO_TCP_H
#define DB_NETXDUO_TCP_H

#include <ittia/db/db_iot_storage.h>
#include <ittia/db/db_udcp.h>

#include "nx_api.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Options for ITTIA plugin for NetX Duo TCP.
 */
typedef struct
{
    size_t             size; /* set size of sizeof(db_netxduo_tcp_options_t) */

    NX_PACKET_POOL *   netxduo_packet_pool;
    NX_IP *            netxduo_tcp_interface;
    uint16_t           netxduo_tcp_port;
} db_netxduo_tcp_options_t;

const db_udcp_info_t * db_netxduo_tcp_udcp_info(void);

/** Register statically linked plugin. */
#define DB_NETXDUO_TCP_REGISTER()               (db_udcp_register(db_netxduo_tcp_udcp_info()))
/** Unregister plugin. */
#define DB_NETXDUO_TCP_UNREGISTER()             (db_udcp_unregister(db_netxduo_tcp_udcp_info()->udcp_proto_name))

#ifdef __cplusplus
} /*extern "C" */
#endif

#endif /* DB_NETXDUO_TCP_H */
