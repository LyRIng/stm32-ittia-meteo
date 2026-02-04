/**************************************************************************/
/*                                                                        */
/*      Copyright (c) 2018-2019 by ITTIA L.L.C. All rights reserved.      */
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
/*  http://www.ittia.com                                                  */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

/** @file db_netxduo_tcp.c
 *
 * NetX Duo TCP plugin for ITTIA DB.
 *
 * This plugin provides a user-defined connection protocol (UDCP) for ITTIA DB
 * that uses the NetX Duo TCP interface.
 */

#include "db_netxduo_tcp.h"
#include "tx_api.h"

#include <ittia/os/os_debug.h>

#include <string.h>

#define x_netxduo_tcp_malloc    malloc
#define x_netxduo_tcp_realloc   realloc
#define x_netxduo_tcp_calloc    calloc
#define x_netxduo_tcp_free      free
#define x_netxduo_tcp_strdup    strdup
#define x_netxduo_tcp_new(type) ((type*)x_netxduo_tcp_malloc(sizeof(type)))
#define x_netxduo_tcp_delete    x_netxduo_tcp_free

#define NETXDUO_TCP_DEFAULT_PORT              (16534)
#define NETXDUO_TCP_SERVER_MAX_SOCKETS        (1)
#define NETXDUO_TCP_SERVER_EVENT_QUEUE_DEPTH  (10)

/* Keepalive will be sent every 2 seconds */
#define TCP_SERVER_KEEP_ALIVE_INTERVAL      (2)
/* Retry 5 times */
#define TCP_SERVER_KEEP_ALIVE_PROBES        (5)
/* Initiate keepalive check after 5 seconds of silence on a tcp socket */
#define TCP_SERVER_KEEP_ALIVE_TIME          (5)

#define TCP_CLIENT_CONNECT_TIMEOUT          (500)
#define TCP_CLIENT_RECEIVE_TIMEOUT          (300)

enum db_netx_handle_type_t {
    SERVER_LISTEN,
    SERVER_SOCKET
};

typedef struct db_netxduo_tcp_handle_s
{
    enum db_netx_handle_type_t handle_type;

    NX_PACKET_POOL * packet_pool;
    NX_IP * 		 server_ip;
    NX_TCP_SOCKET *  server_socket;
    TX_SEMAPHORE     server_semaphore;
    TX_THREAD * 	 wait_thread_ptr;
    UINT 			 server_port;
    NX_PACKET        *received_packet;
    ULONG            received_offset;
} db_netxduo_tcp_handle_t;

static int db_netxduo_tcp_initialized = 0;

#define db_netxduo_tcp_sock_up(db_udcp_handle)      ((db_netxduo_tcp_handle_t*)db_udcp_handle)
#define db_netxduo_tcp_sock_down(netxduo_tcp_handle)  ((db_udcp_handle_t)netxduo_tcp_handle)


static int
db_netxduo_tcp_initialize()
{
    if (!db_netxduo_tcp_initialized) {
        db_netxduo_tcp_initialized = 1;
    }

    return DB_NOERROR;
}

static int
db_netxduo_tcp_finalize()
{
    if (db_netxduo_tcp_initialized) {
        db_netxduo_tcp_initialized = 0;
    }

    return DB_NOERROR;
}

static int
db_netxduo_tcp_close(db_udcp_handle_t h)
{
    db_netxduo_tcp_handle_t * netxduo_tcp_handle = db_netxduo_tcp_sock_up(h);
    UINT status;

    if (NULL != netxduo_tcp_handle->received_packet) {
		status = nx_packet_release(netxduo_tcp_handle->received_packet);
		netxduo_tcp_handle->received_packet = NULL;
    }

    if(netxduo_tcp_handle->handle_type == SERVER_LISTEN) {

    	status = nx_tcp_server_socket_unlisten(netxduo_tcp_handle->server_ip, netxduo_tcp_handle->server_port);
    	if(status == NX_ENTRY_NOT_FOUND) {
    		return DB_ENOENT;
    	}
		status = nx_tcp_socket_disconnect(netxduo_tcp_handle->server_socket, NX_WAIT_FOREVER);
		if(status == NX_NOT_CONNECTED) {
			return DB_ESOCKETNOTCONN;
		}
		status = nx_tcp_socket_delete(netxduo_tcp_handle->server_socket);
		if(status == NX_NOT_CREATED) {
			return DB_EINTERNAL;
		}

		status = nx_ip_delete(netxduo_tcp_handle->server_ip);
    } else {
    	DB_ASSERT(netxduo_tcp_handle->handle_type == SERVER_SOCKET);
        status = nx_tcp_socket_disconnect(netxduo_tcp_handle->server_socket, NX_WAIT_FOREVER);
    	status = nx_tcp_server_socket_unaccept(netxduo_tcp_handle->server_socket);
        status = nx_tcp_server_socket_relisten(netxduo_tcp_handle->server_ip, netxduo_tcp_handle->server_port, netxduo_tcp_handle->server_socket);
    }

    x_netxduo_tcp_delete(netxduo_tcp_handle);
    return DB_NOERROR;
}

static int
db_netxduo_tcp_poll(db_udcp_poll_handle_t * handles, size_t nhandles, int * timeout )
{
    /* Note: this function is only used if send or recv return with 0 bytes
     * sent or received. That should never happen with WICED. */
    return 0;
}

typedef enum
{
    SOCKET_ERROR_EVENT,
    SOCKET_CONNECT_EVENT,
    SOCKET_DISCONNECT_EVENT,
    SOCKET_PACKET_RECEIVED_EVENT,
    SERVER_STOP_EVENT,
} http_server_event_t;

typedef struct
{
    NX_TCP_SOCKET* socket;
    http_server_event_t event_type;
} server_event_message_t;


static void port_connect_request(NX_TCP_SOCKET *socket_ptr, UINT port)
{
    TX_SEMAPHORE * semaphore = (TX_SEMAPHORE*)socket_ptr->nx_tcp_socket_reserved_ptr;
	/* Simply set the semaphore to wake up the server thread. */
    tx_semaphore_put(semaphore);
}

static int
db_netxduo_tcp_listen(db_udcp_handle_t * h, void * netxduo_tcp_param)
{
    db_netxduo_tcp_handle_t * netxduo_tcp_handle = NULL;
    uint16_t netx_port;
    NX_IP * netx_ip;
    db_netxduo_tcp_options_t * options = (db_netxduo_tcp_options_t *)netxduo_tcp_param;
    UINT status;

    DB_ASSERT( db_netxduo_tcp_initialized );

    netxduo_tcp_handle = x_netxduo_tcp_new(db_netxduo_tcp_handle_t);
	if (netxduo_tcp_handle == NULL) {
		return DB_ENOMEM;
	}

    if (options != NULL && options->netxduo_tcp_interface != NULL && options->netxduo_packet_pool != NULL) {
    	netx_ip = options->netxduo_tcp_interface;
        netx_port = options->netxduo_tcp_port;

        if (netx_port == 0) {
        	netx_port = NETXDUO_TCP_DEFAULT_PORT;
        }
    }
    else {
        /* Bind address is required */
        return DB_EINVAL;
    }

    netxduo_tcp_handle->server_port = netx_port;
    netxduo_tcp_handle->received_packet = NULL;
    netxduo_tcp_handle->received_offset = 0;
    netxduo_tcp_handle->packet_pool = options->netxduo_packet_pool;

    status = tx_semaphore_create(&netxduo_tcp_handle->server_semaphore, NULL, 1);

    /* Create a TCP socket to use for our TLS session.  */
    netxduo_tcp_handle->server_socket = (NX_TCP_SOCKET*)malloc(sizeof(NX_TCP_SOCKET));
	status =  nx_tcp_socket_create(	netx_ip,
									netxduo_tcp_handle->server_socket, NULL,
									NX_IP_NORMAL, NX_FRAGMENT_OKAY,
									NX_IP_TIME_TO_LIVE, 8192,
									NX_NULL, NX_NULL);

	if (status != NX_SUCCESS) {
		/* Cancel was requested */
		return DB_ESOCKET;
	}

	netxduo_tcp_handle->server_socket->nx_tcp_socket_reserved_ptr = (void*)&netxduo_tcp_handle->server_semaphore;
	/* Setup server listening on port 12. */
	status = nx_tcp_server_socket_listen(netx_ip, netxduo_tcp_handle->server_port, netxduo_tcp_handle->server_socket, 5, &port_connect_request);

    netxduo_tcp_handle->handle_type = SERVER_LISTEN;
    netxduo_tcp_handle->server_ip  = netx_ip;

    *h = db_netxduo_tcp_sock_down(netxduo_tcp_handle);

    return DB_NOERROR;
}

static int
db_netxduo_tcp_accept(db_udcp_handle_t h, db_udcp_handle_t * res)
{
    db_netxduo_tcp_handle_t * netxduo_tcp_connection_handle = x_netxduo_tcp_new(db_netxduo_tcp_handle_t);// db_netxduo_tcp_sock_up(h);
    db_netxduo_tcp_handle_t * netxduo_tcp_handle_listner_handle = db_netxduo_tcp_sock_up(h);
    UINT status;

    DB_ASSERT( db_netxduo_tcp_initialized );

    netxduo_tcp_handle_listner_handle->wait_thread_ptr = tx_thread_identify();

	status = tx_semaphore_get(&netxduo_tcp_handle_listner_handle->server_semaphore, TX_WAIT_FOREVER);

	status = nx_tcp_server_socket_accept( netxduo_tcp_handle_listner_handle->server_socket, NX_WAIT_FOREVER);
	if(status == NX_NOT_LISTEN_STATE) {
		return DB_ESOCKETNOTCONN;
	}

	netxduo_tcp_connection_handle->handle_type = SERVER_SOCKET;
	netxduo_tcp_connection_handle->server_socket = netxduo_tcp_handle_listner_handle->server_socket;
	netxduo_tcp_connection_handle->packet_pool = netxduo_tcp_handle_listner_handle->packet_pool;
	/* Copy member variables required to call nx_tcp_server_socket_relisten after the connection is closed. */
	netxduo_tcp_connection_handle->server_ip = netxduo_tcp_handle_listner_handle->server_ip;
	netxduo_tcp_connection_handle->server_port = netxduo_tcp_handle_listner_handle->server_port;
	netxduo_tcp_connection_handle->received_packet = NULL;
	netxduo_tcp_connection_handle->received_offset = 0;

    *res = db_netxduo_tcp_sock_down(netxduo_tcp_connection_handle);

    netxduo_tcp_handle_listner_handle->wait_thread_ptr = NULL;
    return DB_NOERROR;

}

/* stop accepting requests */
static int
db_netxduo_tcp_cancel(db_udcp_handle_t h)
{
    db_netxduo_tcp_handle_t * netxduo_tcp_handle = db_netxduo_tcp_sock_up(h);
    if(netxduo_tcp_handle->wait_thread_ptr != NULL) {
    	tx_thread_wait_abort(netxduo_tcp_handle->wait_thread_ptr);
    }
    return DB_NOERROR;
}


static int
db_netxduo_tcp_connect(db_udcp_handle_t * res, void * netxduo_tcp_param,
            const db_fname_t * host, const db_fname_t * port,
            const db_fname_t * user_name, const db_fname_t * password)
{
    return DB_ENOTIMPL;
}

static int
db_netxduo_tcp_recv( db_udcp_handle_t h, void * buffer, size_t buffer_size, int timeout, size_t * bytes_recvd, int * time_rest )
{
    db_netxduo_tcp_handle_t * netxduo_tcp_handle = db_netxduo_tcp_sock_up(h);
    uint32_t netx_timeout;
    NX_PACKET *received_packet;
    UINT status;

    if (netxduo_tcp_handle == NULL
        || bytes_recvd == NULL
        || (buffer == NULL && buffer_size > 0)
        || buffer_size < 0) {
        return DB_EINVAL;
    }

    ULONG offset;
    if (NULL == netxduo_tcp_handle->received_packet) {
		netx_timeout = timeout < 0 ? NX_WAIT_FOREVER : (uint32_t)timeout;

		netxduo_tcp_handle->wait_thread_ptr = tx_thread_identify();

		status = nx_tcp_socket_receive(netxduo_tcp_handle->server_socket, &received_packet, netx_timeout);
		netxduo_tcp_handle->wait_thread_ptr = NULL;
		if(status == NX_WAIT_ABORTED) {
			return DB_ESOCKET;
		}
		else if (status == NX_NOT_BOUND || status == NX_NOT_CONNECTED) {
			return DB_ESOCKETNOTCONN;
		}
		else if (status != NX_SUCCESS) {
			return DB_ESOCKETRECV;
		}
		offset = 0;
    }
    else {
    	// Continue to read from previously received packet
    	received_packet = netxduo_tcp_handle->received_packet;
    	offset = netxduo_tcp_handle->received_offset;
    	netxduo_tcp_handle->received_packet = NULL;
    }

    ULONG packet_length;
    ULONG bytes_copied;
    status = nx_packet_length_get(received_packet, &packet_length);
    if (status != NX_SUCCESS) {
    	(void)nx_packet_release(received_packet);
    	return DB_ESOCKETRECV;
    }

    status = nx_packet_data_extract_offset(received_packet, offset, buffer, buffer_size, &bytes_copied);
    if (status != NX_SUCCESS) {
    	(void)nx_packet_release(received_packet);
    	return DB_ESOCKETRECV;
    }

    offset += bytes_copied;
    if (offset < packet_length) {
        netxduo_tcp_handle->received_packet = received_packet;
        netxduo_tcp_handle->received_offset = offset;
    }
    else {
    	(void)nx_packet_release(received_packet);
    }

    *bytes_recvd = bytes_copied;
    return DB_NOERROR;
}

static int
db_netxduo_tcp_send( db_udcp_handle_t h, const void * buffer, size_t buffer_size, size_t * bytes_sent )
{
	db_netxduo_tcp_handle_t * netxduo_tcp_handle = db_netxduo_tcp_sock_up(h);

	if (netxduo_tcp_handle == NULL
	        || bytes_sent == NULL
	        || (buffer == NULL && buffer_size > 0)
	        || buffer_size < 0) {
	        return DB_EINVAL;
	}

	NX_PACKET * netxduo_packet;
	UINT status;

	netxduo_tcp_handle->wait_thread_ptr = tx_thread_identify();

	/* Allocate a packet for the buffer message */
	status = nx_packet_allocate(netxduo_tcp_handle->packet_pool, &netxduo_packet, NX_TCP_PACKET, NX_WAIT_FOREVER);
	if(status != NX_SUCCESS) {
		return DB_EINVAL;
	}
	/* Place buffer in the packet. */
	status = nx_packet_data_append(netxduo_packet, (void*)buffer, buffer_size, netxduo_tcp_handle->packet_pool, NX_WAIT_FOREVER);
	if(status != NX_SUCCESS) {
		/* Error, release the packet. */
		nx_packet_release(netxduo_packet);
		return DB_EINVAL;
	}

	/* Send buffer to client. */
	status = nx_tcp_socket_send(netxduo_tcp_handle->server_socket, netxduo_packet, 200);
	netxduo_tcp_handle->wait_thread_ptr = NULL;
	if(status != NX_SUCCESS) {
		nx_packet_release(netxduo_packet);
		return DB_ESOCKETSEND;
	}

    *bytes_sent = buffer_size;
    return DB_NOERROR;
}

static int
db_netxduo_tcp_alive(db_udcp_handle_t h)
{
	db_netxduo_tcp_handle_t * netxduo_tcp_handle = db_netxduo_tcp_sock_up(h);

	if (netxduo_tcp_handle == NULL) {
		return DB_EINVAL;
	}

    UINT status;

    ULONG tcp_socket_state;
    status = nx_tcp_socket_info_get(netxduo_tcp_handle->server_socket,
    		NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			&tcp_socket_state,
			NULL,
			NULL,
			NULL);

    if (status != TX_SUCCESS) {
    	return DB_ESOCKET;
    }

    return tcp_socket_state == NX_TCP_ESTABLISHED || tcp_socket_state == NX_TCP_LISTEN_STATE;
}

static db_udcp_info_t udcp_info =
{
    /* udcp_proto_name      => */   "idb+tcp",
    /* udcp_capabilities    => */   0, // DB_UDCP_AUTH,
    /* udcp_initialize      => */   &db_netxduo_tcp_initialize,
    /* udcp_finalize        => */   &db_netxduo_tcp_finalize,
    /* udcp_close           => */   &db_netxduo_tcp_close,
    /* udcp_listen          => */   &db_netxduo_tcp_listen,
    /* udcp_accept          => */   &db_netxduo_tcp_accept,
    /* udcp_cancel          => */   &db_netxduo_tcp_cancel,
    /* udcp_connect         => */   &db_netxduo_tcp_connect,
    /* udcp_recv            => */   &db_netxduo_tcp_recv,
    /* udcp_send            => */   &db_netxduo_tcp_send,
    /* udcp_poll            => */   &db_netxduo_tcp_poll,
    /* udcp_alive           => */   &db_netxduo_tcp_alive,
};


const db_udcp_info_t *
db_netxduo_tcp_udcp_info()
{
    return &udcp_info;
}
