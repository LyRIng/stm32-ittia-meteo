/**************************************************************************/
/*                                                                        */
/*      Copyright (c) 2005-2024 by ITTIA L.L.C. All rights reserved.      */
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

#ifndef IDC_AGENT_H
#define IDC_AGENT_H

/** @file idc_agent.h
 * Public API for the ITTIA Data Connect agent.
 */

#include <ittia/db/db_environment.h>
#include <ittia/db/db_stream.h>

C_HEADER_BEGIN

//                                | Accepted inputs      | Initial data | Data storage |
enum idc_relation_type_t
{                      // | -------------------- | ------------ | ------------ |
    kTable,            // | Insert/update/delete | Stateful     | Persistent   |
    kRealTimeStream,   // | Append-only/tagged   | Stateless    | Temporary    |
    kRealTimeView,     // | Read-only            | Stateless    | Temporary    |
    kMaterializedView, // | Read-only            | Stateful     | Temporary    |
};

/**
 * @brief A table or stream accessed through ITTIA Data Connect.
 */
typedef struct idc_synchronized_relation_s
{
    /** Name of the data model */
    const char *model_name;
    /** Name of the stream or table */
    const char *relation_name;

    /** Timestamp (UTC) when the Data Stream was created. */
    db_timestamp_usec_t created_time;

    /** The type of relation in the local environment. */
    enum idc_relation_type_t relation_type;

    // Possible output event combinations of a synchronized relation:
    // - insert/update/delete - tables; group cselect; filter or window without "for system_time all"
    // - insert/update only   - stream with primary key; cselect no filter or window
    // - insert/delete only   - filtered/windowed "for system_time all" cselect
    // - insert only          - stream without primary key; "for system_time all" cselect without filter/window/group

    /** @brief Minimum time interval between updates to the same key.
     * When the update rate is unlimited, min_interval is zero.
     */
    db_interval_t update_interval;

    /** @brief Maximum number of unique key values that can exist in the relation.
     * simultaneously. When the number of key values is unlimited, max_key_cardinality is zero. */
    uint32_t max_key_cardinality;
} idc_synchronized_relation_t;

/**
 * @brief A unique data model instance owned by the current device.
 */
typedef struct idc_data_model_instance_s
{
    /** A number that has been uniquely assigned to the current device, or @c IDC_INSTANCE_ID_ASSIGN_AT_TRANSMISSION. */
    int32_t instance_id;
    /** The name of the instanced data model. */
    const char *data_model_name;
    /** A label that describes the instance purpose or location. */
    const char *instance_label;
} idc_data_model_instance_t;

/** @brief A special value for ITTIA Data Connect instance ID that is replaced by
 * a unique positive value assigned by the IDC controller. */
#define IDC_INSTANCE_ID_ASSIGN_AT_TRANSMISSION 0

/**
 * @brief Run the ITTIA Data Connect agent.
 *
 * @param stream_env
 *   Stream environment from which to read data.
 * @param relation_array
 *   An array of synchronized relations available in @a stream_env.
 * @param relation_count
 *   The number of elements in @a relation_array.
 * @param data_model_instance_array
 *   An array of data model instances owned by this device.
 * @param data_model_instance_count
 *   The number of elements in @a data_model_instance_array.
 * @param listener,
 *   The User-Defined Connection Protocol (UDCP) listener on which to accept connections.
 * @param proto_name
 *   Protocol name as a string.
 * @param proto_param
 *   Protocol-specific parameters.
 *
 * @par Description
 * @parblock
 * Listen for connections from the ITTIA Data Connect controller. Data from the
 * synchronized relations is communicated to the controller if requested.
 *
 * When a connection is established, a unique instance identifier number is
 * assigned to @a *instance_id.
 *
 * The idc_run_agent_tcp function blocks until the network is interrupted. Call
 * this function in a background thread to access the tables and streams in
 * parallel.
 * @endparblock
 *
 * @returns DB_NOERROR on success
 * @returns a negative number on failure
 *
 * @see db_stream_create_graph,<br>
 * db_stream_create_row_output, Errors
 */
dbstatus_t idc_run_agent(
    db_stream_environment_t stream_env,
    const idc_synchronized_relation_t *relation_array,
    size_t relation_count,
    idc_data_model_instance_t *data_model_instance_array,
    size_t data_model_instance_count,
    const char *proto_name,
    void *proto_param);

/**
 * @brief Run the ITTIA Data Connect agent on TCP/IP v4.
 *
 * @param stream_env
 *   Stream environment from which to read data.
 * @param relation_array
 *   An array of synchronized relations available in @a stream_env.
 * @param relation_count
 *   The number of elements in @a relation_array.
 * @param data_model_instance_array
 *   An array of data model instances owned by this device.
 * @param data_model_instance_count
 *   The number of elements in @a data_model_instance_array.
 * @param bind_addr
 *   The TCP network address of interface on which to listen for connections.
 *   Use @c 0 to listen on all available interfaces.
 * @param bind_port
 *   The TCP network port on which to listen for connections.
 * @param instance_id
 *   A pointer to an @c int32_t that will be assigned a unique instance identifier.
 *
 * @par Description
 * @parblock
 * Listen for connections from the ITTIA Data Connect controller. Data from the
 * synchronized relations is communicated to the controller if requested.
 *
 * When a connection is established, a unique instance identifier number is
 * assigned to @a *instance_id.
 *
 * The idc_run_agent_tcp function blocks until the network is interrupted. Call
 * this function in a background thread to access the tables and streams in
 * parallel.
 * @endparblock
 *
 * @returns DB_NOERROR on success
 * @returns a negative number on failure
 *
 * @see db_stream_create_graph,<br>
 * db_stream_create_row_output, Errors
 */
dbstatus_t idc_run_agent_tcp4(
    db_stream_environment_t stream_env,
    const idc_synchronized_relation_t *relation_array,
    size_t relation_count,
    idc_data_model_instance_t *data_model_instance_array,
    size_t data_model_instance_count,
    uint32_t bind_addr,
    uint16_t bind_port);

// (NEW) stop agent header function
dbstatus_t idc_stop_agent();


C_HEADER_END

#endif /* IDC_AGENT_H */
