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

/** @file
 *
 * Entry point for the ITTIA Data Stream C API. Part of the ITTIA DB Lite
 * Embedded C API.
 */

#ifndef DB_STREAM_H
#define DB_STREAM_H

#include <ittia/db/db_environment.h>
#include <ittia/db/db_coltype.h>

C_HEADER_BEGIN

/** @addtogroup CAPIStream
 *  @{ */ 

/**
 * @brief db_stream_environment_t is a handle to a namespace of registered Data Streams.
 */
typedef struct db_stream_environment_s* db_stream_environment_t;

/**
 * @brief db_stream_graph_t is a handle to a graph of stream processing nodes.
 */
typedef struct db_stream_graph_s* db_stream_graph_t;

/**
 * @brief db_stream_node_t is a handle to a node in a stream processing graph.
 */
typedef struct db_stream_node_s* db_stream_node_t;

/**
 * @brief db_stream_callback_t describes the callback function for a @ db_stream_node_t.
 */
typedef void (*db_stream_callback_t)(db_stream_node_t, void *);

/**
 * Aggregate functions.
 */
enum db_aggregate_t {
    DB_AGGREGATE_GROUP_BY,
    DB_AGGREGATE_END_TIMESTAMP,
    DB_AGGREGATE_COUNT,
    DB_AGGREGATE_SUM,
    DB_AGGREGATE_AVERAGE,
};

/**
 * Aggregate field definition.
 */
typedef struct {
    db_fieldno_t source_fieldno;
    enum db_aggregate_t aggregate;
    const char * field_name;
} db_aggregate_field_t;

/**
 * @brief db_stream_create_graph allocates an empty stream processing graph
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 *
 * @param [out] graph
 *   Destination for the stream graph handle
 * @param mem
 *   Memory buffer in which to create the graph and its nodes
 * @param size
 *   Size of memory buffer in bytes
 * 
 * @par Description
 * @parblock
 * The stream processing environment must be initialized before creating any continuous queries.
 * @code
 * (void)db_init_ex(DB_API_VER, NULL);
 * @endcode
 * 
 * An application first creates an empty stream, represented as a directed graph. An instance of #db_stream_graph_t is used to handle the streaming graph:
 * 
 * @code
 * dbstatus_t status;
 * db_stream_graph_t graph;
 * static int32_t buffer[1024 * 1024];
 * status = db_stream_create_graph(&graph, buffer, sizeof buffer);
 * if (DB_FAILED(status)) {
 *     fprintf(stderr, "Database init error: %d\n", status);
 * }
 * // ...
 * // Use stream to process data
 * // ...
 * @endcode
 * 
 * Use this function to create a graph in @a mem of @a size bytes. The stream graph is returned through a reference to @a graph.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 *
 * @see db_stream_free_graph <br>
 */
dbstatus_t db_stream_create_graph(db_stream_graph_t * graph, void * mem, size_t size);

/**
 * @brief db_stream_free_graph frees a graph previously created.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param graph
 *   Stream graph handle
 * @param [out] peak_memory_size
 *   Actual number of bytes used by the graph and its nodes, or <tt>NULL</tt>
 * 
 * @par Description
 * @parblock
 * An application first creates an empty stream, represented as a directed graph. An instance of #db_stream_graph_t is used to handle the streaming graph.
 * To release the resources used by the stream graph call #db_stream_free_graph. Example:
 * 
 * @code
 * dbstatus_t status;
 * db_stream_graph_t graph;
 * static int32_t buffer[1024 * 1024];
 * status = db_stream_create_graph(&graph, buffer, sizeof buffer);
 * if (DB_FAILED(status)) {
 *     fprintf(stderr, "Database init error: %d\n", status);
 * }
 * // ...
 * // Use stream to process data
 * // ...
 * db_stream_free_graph(graph, NULL);
 * @endcode
 * 
 * #db_stream_free_graph can also be optionally used to get the size of the graph and its nodes being deleted.
 * @code
 * size_t peak_memory_size;
 * db_stream_free_graph(graph, &peak_memory_size);
 * @endcode
 * 
 * Use this function to free the memory used by @a graph. The graph handle and all associated nodes can not be used after this function is called.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_graph <br>
 */
void db_stream_free_graph(db_stream_graph_t graph, size_t * peak_memory_size);

/**
 * @brief db_stream_create_row_input creates an input row into a streaming graph.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Destination node for row output
 * @param graph
 *   Graph in which to create the node and its dependents
 * @param field_list
 *   Schema to use for input row
 * @param field_count
 *   Number of fields in @a field_list
 * @param timestamp_field
 *   Index of field to use as the transaction timestamp
 * @param key_field
 *   Index of field to use as a key
 * 
 * @par Description
 * @parblock
 * This function requires a stream @a graph to be initialized previously to this call.
 * Use this function to create an input node to a data stream, the created node will be
 * returned through a reference to @a output.
 * 
 * A stream with four fields is defined below: a sensor ID number, a sensor type attribute, a sample timestamp, and a sample value:
 * @code
 * // Define fields for each sensor data record
 * enum {
 *     kSensorId,
 *     kSensorType,
 *     kSampleTime,
 *     kSampleValue,
 * };
 * static const db_fielddef_t fields[] = {
 *     { kSensorId, "sensor_id", DB_COLTYPE_UINT64, 0, 0, DB_NOT_NULL, NULL, 0 },
 *     { kSensorType, "sensor_type", DB_COLTYPE_UINT64, 0, 0, DB_NOT_NULL, NULL, 0 },
 *     { kSampleTime, "sample_time", DB_COLTYPE_TIMESTAMP, 0, 0, DB_NOT_NULL, NULL, 0 },
 *     { kSampleValue, "sample_value", DB_COLTYPE_FLOAT64, 0, 0, DB_NOT_NULL, NULL, 0 },
 * };
 * 
 * static size_t nfields = DB_ARRAY_DIM(fields);
 * @endcode
 * 
 * The following example demonstrates how to set up the stream environment shown below:
 * @code
 * dbstatus_t status;
 * db_stream_graph_t graph;
 * static int32_t buffer[1024 * 1024];
 * db_stream_node_t s0, s1, s2;
 * int32_t row_count = 0;
 * 
 * status = db_stream_create_graph(&graph, buffer, sizeof buffer);
 * 
 * if (DB_SUCCESS(status)) {
 *     // Create input node
 *        status = db_stream_create_row_input(&s0, graph, fields, nfields, kSampleTime, -1);
 * }
 * 
 * if (DB_SUCCESS(status)) {
 *     // Create filter node with equality operation
 *     status = db_stream_create_filter_uint64(&s1, s0, kSensorId, DB_SEEK_EQUAL, 1);
 * }
 * 
 * if (DB_SUCCESS(status)) {
 *     // Create output node
 *     status = db_stream_create_row_output(&s2, s1, &process_filtered_sensor_data, &row_count);
 * }
 * @endcode
 * @endparblock
 * 
 * The @a field_list contains the schema for an input row to the node.
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_graph,<br>
 * db_stream_create_row_output, Errors
 */
dbstatus_t db_stream_create_row_input(db_stream_node_t * output, db_stream_graph_t graph, const db_fielddef_t * field_list, const db_len_t field_count, db_fieldno_t timestamp_field, db_fieldno_t key_field);

/**
 * @brief db_stream_create_row_input creates an input row into a streaming graph.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Destination node for row output
 * @param graph
 *   Graph in which to create the node and its dependents
 * @param field_list
 *   Schema to use for input row
 * @param field_count
 *   Number of fields in @a field_list
 * @param timestamp_field
 *   Index of field to use as the transaction timestamp
 * @param key_field_list
 *   Array of field indexes to use as a key
 * @param key_field_count
 *   Number of index fields to use in the key
 * 
 * @par Description
 * @parblock
 * This function requires a stream @a graph to be initialized previously to this call.
 * Use this function to create an input node to a data stream, the created node will be
 * returned through a reference to @a output.
 * @endparblock
 * 
 * The @a field_list contains the schema for an input row to the node.
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_graph,<br>
 * db_stream_create_row_output, Errors
 */
dbstatus_t db_stream_create_row_input_compound_key(db_stream_node_t * output, db_stream_graph_t graph, const db_fielddef_t * field_list, const db_len_t field_count, db_fieldno_t timestamp_field, const db_fieldno_t * key_field_list, const db_len_t key_field_count);

/**
 * @brief db_stream_set_null sets a data stream field to null.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   Stream node of field to be set
 * @param field
 *   Destination field to set
 * 
 * @par Description
 * @parblock
 * Use this function to clear the current value for the @a field of the stream node @a stream. The field must have been defined with the DB_NULLABLE flag.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_sint32, db_stream_set_uint32,<br>
 * db_stream_set_uint64, db_stream_set_sint64, db_stream_set_float32, db_stream_set_float64 
 */
dbstatus_t db_stream_set_null(db_stream_node_t stream, db_fieldno_t field);


/**
 * @brief db_stream_set_sint32 sets a signed 32 bit integer field of data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   Stream node of field to be set
 * @param field
 *   Destination field to set
 * @param value
 *   Value of stream to set
 * 
 * @par Description
 * @parblock
 *  Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_null, db_stream_set_uint32,<br>
 * db_stream_set_uint64, db_stream_set_sint64, db_stream_set_float32, db_stream_set_float64
 */
dbstatus_t db_stream_set_sint32(db_stream_node_t stream, db_fieldno_t field, int32_t value);

/**
 * @brief db_stream_set_uint32 sets an unsigned 32 bit integer field of data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   Stream node of field to be set
 * @param field
 *   Destination field to set
 * @param value
 *   Value of stream to set
 * 
 * @par Description
 * @parblock
 *  Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_null, db_stream_set_sint32,<br>
 * db_stream_set_uint64, db_stream_set_sint64, db_stream_set_float32, db_stream_set_float64
 */
dbstatus_t db_stream_set_uint32(db_stream_node_t stream, db_fieldno_t field, uint32_t value);

/**
 * @brief db_stream_set_uint64 sets a signed 64 bit integer field of data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   Stream node of field to be set
 * @param field
 *   Destination field to set
 * @param value
 *   Value of stream to set
 * 
 * @par Description
 * @parblock
 *  Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_null, db_stream_set_sint32, db_stream_set_uint32,<br>
 * db_stream_set_uint64, db_stream_set_float32, db_stream_set_float64
 */
dbstatus_t db_stream_set_sint64(db_stream_node_t stream, db_fieldno_t field, int64_t value);

/**
 * @brief db_stream_set_uint64 sets an unsigned 64 bit integer field of data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   Stream node of field to be set
 * @param field
 *   Destination field to set
 * @param value
 *   Value of stream to set
 * 
 * @par Description
 * @parblock
 *  Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_null, db_stream_set_sint32, db_stream_set_uint32,<br>
 * db_stream_set_sint64, db_stream_set_float32, db_stream_set_float64
 */
dbstatus_t db_stream_set_uint64(db_stream_node_t stream, db_fieldno_t field, uint64_t value);

/**
 * @brief db_stream_set_timestamp_usec sets the timestamp field of data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   Stream node of field to be set
 * @param field
 *   Destination field to set
 * @param value
 *   Value of stream to set
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @see db_stream_set_timestamp_usec, db_stream_set_sint32, db_stream_set_float32,<br>
 * db_stream_process
 */
dbstatus_t db_stream_set_timestamp_usec(db_stream_node_t stream, db_fieldno_t field, db_timestamp_usec_t value);

/**
 * @brief db_stream_set_float32 sets a 32 bit float field of data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   Stream node of field to be set
 * @param field
 *   Destination field to set
 * @param value
 *   Value of stream to set
 * 
 * @par Description
 * @parblock
 * Call db_stream_set_float32 to prepare the data according to the fields schema
 * Assuming that we have a field definition array that defines the schema of the the input node, and that the input node
 * is created successfully using #db_stream_create_row_input.
 * 
 * @code
 * db_stream_graph_t graph;
 * db_stream_node_t input_node;
 * db_stream_node_t registration_node;
 * static const db_fielddef_t fields[] = {
 *     { kSensorsSensorId,     "sensor_id",     DB_COLTYPE_UINT32,    0, 0, DB_NOT_NULL, NULL, 0 },
 *     { kSensorsTemperatureC, "temperature_C", DB_COLTYPE_FLOAT32,   0, 0, DB_NOT_NULL, NULL, 0 },
 *     { kSensorsPressure_kPa, "pressure_kPa",  DB_COLTYPE_FLOAT32,   0, 0, DB_NOT_NULL, NULL, 0 }
 * };
 * @endcode
 * Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @code
 * uint32_t sensor_id = 1;
 * db_float32_t temperature_value_C = 65.00;
 * db_float32_t pressure_value_kPa = 950.00;
 *     // Prepare and store data
 *     db_stream_set_uint32(input_node, kSensorsSensorId, sensor_id);
 *     db_stream_set_float32(input_node, kSensorsTemperatureC, temperature_value_C);
 *     db_stream_set_float32(input_node, kSensorsPressure_kPa, pressure_value_kPa);
 *     Status = db_stream_process(input_node);
 * @endcode
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_null, db_stream_set_sint32, db_stream_set_uint32,<br>
 * db_stream_set_sint64, db_stream_set_float64
 */
dbstatus_t db_stream_set_float32(db_stream_node_t stream, db_fieldno_t field, db_float32_t value);

/**
 * @brief db_stream_set_float64 sets a 64 bit float field of data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   Stream node of field to be set
 * @param field
 *   Destination field to set
 * @param value
 *   Value of stream to set
 * 
 * @par Description
 * @parblock
 * Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_null, db_stream_set_sint32, db_stream_set_uint32,<br>
 * db_stream_set_sint64, db_stream_set_float32
 */
dbstatus_t db_stream_set_float64(db_stream_node_t stream, db_fieldno_t field, db_float64_t value);

/**
 * @brief db_stream_set_interval_day_to_second sets an interval day-to-second field of data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   A stream input node.
 * @param field
 *   Destination field number to set.
 * @param value
 *   A number of microseconds.
 * 
 * @par Description
 * @parblock
 * Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_interval_year_to_month, db_stream_set_null
 */
dbstatus_t db_stream_set_interval_day_to_second(db_stream_node_t stream, db_fieldno_t field, db_interval_t value);

/**
 * @brief db_stream_set_interval_year_to_month sets an interval year-to-month field of data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   A stream input node.
 * @param field
 *   Destination field number to set.
 * @param value
 *   A number of months.
 * 
 * @par Description
 * @parblock
 * Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_interval_day_to_second, db_stream_set_null
 */
dbstatus_t db_stream_set_interval_year_to_month(db_stream_node_t stream, db_fieldno_t field, db_interval_t value);

/**
 * @brief db_stream_set_ansistr sets an ANSI encoded string field on a data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   A stream input node.
 * @param field
 *   Destination field number to set.
 * @param value
 *   A charater string.
 * @param len
 *   The number of characters in the @a value string.
 * 
 * @par Description
 * @parblock
 * Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_null
 */
dbstatus_t db_stream_set_ansistr(db_stream_node_t stream, db_fieldno_t field, const char * const value, db_len_t len);

/**
 * @brief db_stream_set_utf8str sets a UTF-8 encoded string field on a data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   A stream input node.
 * @param field
 *   Destination field number to set.
 * @param value
 *   A charater string.
 * @param len
 *   The number of characters in the @a value string.
 * 
 * @par Description
 * @parblock
 * Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_null
 */
dbstatus_t db_stream_set_utf8str(db_stream_node_t stream, db_fieldno_t field, const db_utf8_t * const value, db_len_t len);

/**
 * @brief db_stream_set_utf16str sets a UTF-16 encoded string field on a data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   A stream input node.
 * @param field
 *   Destination field number to set.
 * @param value
 *   A charater string.
 * @param len
 *   The number of characters in the @a value string.
 * 
 * @par Description
 * @parblock
 * Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_null
 */
dbstatus_t db_stream_set_utf16str(db_stream_node_t stream, db_fieldno_t field, const db_utf16_t * const value, db_len_t len);

/**
 * @brief db_stream_set_utf32str sets a UTF-32 encoded string field on a data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   A stream input node.
 * @param field
 *   Destination field number to set.
 * @param value
 *   A charater string.
 * @param len
 *   The number of characters in the @a value string.
 * 
 * @par Description
 * @parblock
 * Use this function to set the @a field of the stream node @a stream to the value of @a value.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_set_null
 */
dbstatus_t db_stream_set_utf32str(db_stream_node_t stream, db_fieldno_t field, const db_utf32_t * const value, db_len_t len);

/**
 * @brief db_stream_process processes the fields set currently in the stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [in] stream
 *   Stream to process
 * 
 * @par Description
 * @parblock
 * This function will process the current input(s) of the @a stream at the time of calling it. 
 * The following example prints output whenever the result of the a streaming query changes:
 * @code
 * // Handle to a previously assigned stream output node
 * db_stream_node_t s0;
 * 
 * // Initial simulation state
 * static int64_t simulation_row_count = 0;
 * static int64_t now_ms = start_timestamp_ms;
 * 
 * // Simulation state
 * uint64_t sensor_id = 1;
 * int32_t sensor_type = 1;
 * db_timestamp_usec_t sample_time;
 * db_float64_t sample_value = 0.0;
 * 
 * while (simulation_row_count < sample_count) {
 *     // Simulate input
 *     sample_time = now_ms * 1000;
 *     sensor_id = sensor_id % sensor_count + 1;
 *     sample_value = 30.0 + (simulation_row_count % 100) * 0.4;
 * 
 *     // Mutate simulation state for next sample
 *     ++simulation_row_count;
 *     now_ms += increment_timestamp_ms;
 * 
 *     // Process input
 *     db_stream_set_uint64(s0, kSensorId, sensor_id);
 *     db_stream_set_sint32(s0, kSensorType, sensor_type);
 *     db_stream_set_timestamp_usec(s0, kSampleTime, sample_time);
 *     db_stream_set_float64(s0, kSampleValue, sample_value);
 *     db_stream_process(s0);
 * }
 * @endcode
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * @returns
 *   DB_EINVAL if the node does not support processing input
 * 
 * @see db_stream_create_graph
 */
dbstatus_t db_stream_process(db_stream_node_t stream);

/**
 * @brief db_stream_create_row_output creates an output row to a streaming graph.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Destination node for output row
 * @param input
 *   Destination graph to add row to 
 * @param callback
 *   (OPtional) Reference to a callback function 
 * @param argument
 *   (Optional) Argument to callback function
 * 
 * @par Description
 * @parblock
 *  Use this function to create a node to process output from the stream @a input using the callback
 *  function (optional) @a callback provided with the optional argument @a argument to the callback function. 
 *  
 *  The output is returned via reference to the output node by @a output.
 * @endparblock
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 *  @see db_stream_process <br>
 */
dbstatus_t db_stream_create_row_output(db_stream_node_t * output, db_stream_node_t input, db_stream_callback_t callback, void * argument);

/**
 * @brief db_stream_get_tag gets the tag of an entry in a data stream.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param stream
 *   Destination stream to get tag from
 * 
 * @par Description
 * @parblock
 * A continuous query reads data from a stream of one or more fields and applies operators to produce refined output.
 * Use db_stream_get_tag to identify the type of operation to perform on the stream query result.
 * 
 * @code
 * void process_filtered_sensor_data(db_stream_node_t s, void * argument)
 * {
 *     int32_t * row_count = (int32_t*)argument;
 *     ++*row_count;
 *     char tag = db_stream_get_tag(s);
 *     uint64_t sensor_id = db_stream_get_uint64(s, kSensorId);
 *     int32_t sensor_type = db_stream_get_sint32(s, kSensorType);
 *     db_timestamp_usec_t sample_time = db_stream_get_timestamp_usec(s, kSampleTime);
 *     db_float64_t sample_value = db_stream_get_float64(s, kSampleValue);
 * }
 * @endcode
 * 
 * Use this function to get the tag from an entry in a data stream, 
 * this function can be used inside a callback function to the streaming
 * node @a stream.
 * @endparblock
 * 
 * @returns @c '+' for a new entry appended to the stream result
 * @returns @c '-' for a delete from the stream result
 * @returns @c 'U' for an update to an entry previously appended to the stream result
 * 
 * @see db_stream_create_graph
 */
char db_stream_get_tag(db_stream_node_t stream);

/**
 * @brief db_stream_get_transaction_timestamp gets the timestamp value of the most recent transaction.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param stream
 *   Destination node for output row
 * 
 * @par Description
 * @parblock
 * 
 * Use this function to return the latest transaction made from a streaming node.
 
 * @endparblock
 * 
 * @returns
 *   Timestamp value of transaction
 * 
 * @see db_stream_create_graph
 */
int64_t db_stream_get_transaction_timestamp(db_stream_node_t stream);

/**
 * @brief db_stream_is_null checks if a field has a value
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param stream
 *   Target stream to get the value from
 * @param field
 *   Field to get the target value from
 * 
 * @par Description
 * @parblock
 * 
 * Use this function to check if a #db_fieldno_t in the stream has a value
 * 
 * @endparblock
 * 
 * @return true when field is null or invalid
 * @return false when field has a value
 */
bool db_stream_is_null(db_stream_node_t stream, db_fieldno_t field);

/**
 * @brief db_stream_get_sint32 gets the value of the given field.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param stream
 *   Target stream to get the value from
 * @param field
 *   Field to get the target value from 
 * 
 * @par Description
 * @parblock
 * This function returns the value from the output node @a stream at the given field @a field.
 * @endparblock
 * 
 * @returns
 *   Signed 32 bit integer
 * 
 * @see db_stream_get_uint32, db_stream_get_sint64, db_stream_get_uint64,<br>
 * db_stream_get_float32, db_stream_get_float64
 */
int32_t db_stream_get_sint32(db_stream_node_t stream, db_fieldno_t field);

/**
 * @brief db_stream_get_uint32 gets the value of the given field.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param stream
 *   Target stream to get the value from
 * @param field
 *   Field to get the target value from
 * 
 * @par Description
 * @parblock
 * This function returns the value from the output node @a stream at the given field @a field.
 * @endparblock
 *  
 * @returns
 *   Unsigned 32 bit integer
 * 
 * @see db_stream_get_sint32, db_stream_get_sint64, db_stream_get_uint64,<br>
 * db_stream_get_float32, db_stream_get_float64
 */
uint32_t db_stream_get_uint32(db_stream_node_t stream, db_fieldno_t field);

/**
 * @brief db_stream_get_sint64 gets the value of the given field.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param stream
 *   Target stream to get the value from
 * @param field
 *   Field to get the target value from
 * 
 * @par Description
 * @parblock
 * This function returns the value from the output node @a stream at the given field @a field.
 * @endparblock  
 * 
 * @returns
 *   Signed 64 bit integer
 * 
 * @see db_stream_get_sint32, db_stream_get_uint32, db_stream_get_uint64,<br>
 * db_stream_get_float32, db_stream_get_float64
 */
int64_t db_stream_get_sint64(db_stream_node_t stream, db_fieldno_t field);

/**
 * @brief db_stream_get_uint64 gets the value of the given field.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param stream
 *   Target stream to get the value from
 * @param field
 *   Field to get the target value from
 * 
 * @par Description
 * @parblock
 * This function returns the value from the output node @a stream at the given field @a field.
 * @endparblock 
 *  
 * @returns
 *   Unsigned 64 bit integer
 * 
 * @see db_stream_get_sint32, db_stream_get_uint32, db_stream_get_sint64,<br>
 * db_stream_get_float32, db_stream_get_float64
 */
uint64_t db_stream_get_uint64(db_stream_node_t stream, db_fieldno_t field);

/**
 * @brief db_stream_get_timestamp_usec gets the timestamp of the given field.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param stream
 *   Target stream to get the timestamp from
 * @param field
 *   Field to get the target value from 
 * @returns
 *   Timestamp in microsecond format (us)
 * 
 * This function returns the value from the output node @a stream at the given field @a field.
 * @see db_stream_set_timestamp_usec
 */
db_timestamp_usec_t db_stream_get_timestamp_usec(db_stream_node_t stream, db_fieldno_t field);

/**
 * @brief db_stream_get_float32 gets the value of the given field.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param stream
 *   Target stream to get the value from
 * @param field
 *   Field to get the target value from 
 * 
 * @par Description
 * @parblock
 * This function returns the value from the output node @a stream at the given field @a field.
 * @endparblock 
 * 
 * @returns
 *   32 bit float
 *  
 * @see db_stream_get_sint32, db_stream_get_uint32, db_stream_get_sint64,<br>
 * db_stream_get_uint64, db_stream_get_float64
 */
db_float32_t db_stream_get_float32(db_stream_node_t stream, db_fieldno_t field);

/**
 * @brief db_stream_get_float64 gets the value of the given field.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param stream
 *   Target stream to get the value from
 * @param field
 *   Field to get the target value from 
 * 
 * @par Description
 * @parblock
 * This function returns the value from the output node @a stream at the given field @a field.
 * @endparblock 
 * 
 * @returns
 *   64 bit float
 * 
 * @see db_stream_get_sint32, db_stream_get_uint32, db_stream_get_sint64,<br>
 * db_stream_get_uint64, db_stream_get_float32
 */
db_float64_t db_stream_get_float64(db_stream_node_t stream, db_fieldno_t field);

/**
 * @brief db_stream_create_queue creates a buffer of stream results in a queue. 
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Target node to queue results in
 * @param input
 *   Input node that inserts rows to the queue
 * @param queue_row_count
 *   Maximum number of entries to store in the queue 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 */
dbstatus_t db_stream_create_queue(db_stream_node_t * output, db_stream_node_t input, size_t queue_row_count);

/**
 * @brief db_stream_queue_next reads the next queued value into a node
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] queue
 *   Queue node
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * @returns
 *   DB_ENOMOREDATA when the queue is empty
 */
dbstatus_t db_stream_queue_next(db_stream_node_t queue);

/**
 * @brief db_stream_create_filter_sint32 creates a filter on the input node.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Output node from filter
 * @param input
 *   Input node to supply row to filter on 
 * @param field
 *   Field of input row to apply filter to
 * @param compare_op
 *   Type of filter to apply 
 * @param value
 *   Value to use with filter
 * 
 * @par Description
 * @parblock
 * Use this function to create a filter on a streaming node for #int32_t data type. The @a compare_op will be applied on the @a input
 * node on the given @a field using the @a value.
 * @code
 * dbstatus_t status;
 * db_stream_graph_t graph;
 * static int32_t buffer[1024 * 1024];
 * db_stream_node_t s0, s1, s2;
 * int32_t row_count = 0;
 * 
 * status = db_stream_create_graph(&graph, buffer, sizeof buffer);
 * 
 * if (DB_SUCCESS(status)) {
 *     // Create input node
 *     status = db_stream_create_row_input(&s0, graph, fields, nfields, kSampleTime, -1);
 *     // Create filter node with equality operation
 *     status = db_stream_create_filter_sint32(&s1, s0, kSensorId, DB_SEEK_EQUAL, 1);
 *     // Create output node
 *     status = db_stream_create_row_output(&s2, s1, &process_filtered_sensor_data, &row_count);
 * }
 * @endcode
 *  
 * The output node containing the applied filter is returned through reference to @a output.
 * @endparblock
 *  
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_row_input, db_stream_create_row_output,<br>
 * db_stream_create_filter_sint64, db_stream_create_filter_uint32, db_stream_create_filter_uint64, db_stream_create_filter_float32, db_stream_create_filter_float64 
 */
dbstatus_t db_stream_create_filter_sint32(db_stream_node_t * output, db_stream_node_t input, db_fieldno_t field, db_seek_t compare_op, int32_t value);

/**
 * @brief db_stream_create_filter_uint32 creates a filter on the input node.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Output node from filter
 * @param input
 *   Input node to supply row to filter on 
 * @param field
 *   Field of input row to apply filter to
 * @param compare_op
 *   Type of filter to apply 
 * @param value
 *   Value to use with filter 
 * 
 * @par Description
 * @parblock
 * Use this function to create a filter on a streaming node for #uint32_t data type. The @a compare_op will be applied on the @a input
 * node on the given @a field using the @a value.
 * @code
 * dbstatus_t status;
 * db_stream_graph_t graph;
 * static int32_t buffer[1024 * 1024];
 * db_stream_node_t s0, s1, s2;
 * int32_t row_count = 0;
 * 
 * status = db_stream_create_graph(&graph, buffer, sizeof buffer);
 * 
 * if (DB_SUCCESS(status)) {
 *     // Create input node
 *     status = db_stream_create_row_input(&s0, graph, fields, nfields, kSampleTime, -1);
 *     // Create filter node with equality operation
 *     status = db_stream_create_filter_uint32(&s1, s0, kSensorId, DB_SEEK_EQUAL, 1);
 *     // Create output node
 *     status = db_stream_create_row_output(&s2, s1, &process_filtered_sensor_data, &row_count);
 * }
 * @endcode
 *  
 * The output node containing the applied filter is returned through reference to @a output.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_row_input, db_stream_create_row_output,<br>
 * db_stream_create_filter_sint64, db_stream_create_filter_sint32, db_stream_create_filter_uint64, db_stream_create_filter_float32, db_stream_create_filter_float64 
 */
dbstatus_t db_stream_create_filter_uint32(db_stream_node_t * output, db_stream_node_t input, db_fieldno_t field, db_seek_t compare_op, uint32_t value);

/**
 * @brief db_stream_create_filter_sint64 creates a filter on the input node.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Output node from filter
 * @param input
 *   Input node to supply row to filter on 
 * @param field
 *   Field of input row to apply filter to
 * @param compare_op
 *   Type of filter to apply 
 * @param value
 *   Value to use with filter 
 * 
 * @par Description
 * @parblock
 * Use this function to create a filter on a streaming node for #int64_t data type. The @a compare_op will be applied on the @a input
 * node on the given @a field using the @a value.
 * @code
 * dbstatus_t status;
 * db_stream_graph_t graph;
 * static int32_t buffer[1024 * 1024];
 * db_stream_node_t s0, s1, s2;
 * int32_t row_count = 0;
 * 
 * status = db_stream_create_graph(&graph, buffer, sizeof buffer);
 * 
 * if (DB_SUCCESS(status)) {
 *     // Create input node
 *     status = db_stream_create_row_input(&s0, graph, fields, nfields, kSampleTime, -1);
 *     // Create filter node with equality operation
 *     status = db_stream_create_filter_sint64(&s1, s0, kSensorId, DB_SEEK_EQUAL, 1);
 *     // Create output node
 *     status = db_stream_create_row_output(&s2, s1, &process_filtered_sensor_data, &row_count);
 * }
 * @endcode
 *  
 * The output node containing the applied filter is returned through reference to @a output.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_row_input, db_stream_create_row_output,<br>
 * db_stream_create_filter_uint64, db_stream_create_filter_sint32, db_stream_create_filter_uint32, db_stream_create_filter_float32, db_stream_create_filter_float64
 */
dbstatus_t db_stream_create_filter_sint64(db_stream_node_t * output, db_stream_node_t input, db_fieldno_t field, db_seek_t compare_op, int64_t value);

/**
 * @brief db_stream_create_filter_uint64 creates a filter on the input node.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Output node from filter
 * @param input
 *   Input node to supply row to filter on 
 * @param field
 *   Field of input row to apply filter to
 * @param compare_op
 *   Type of filter to apply 
 * @param value
 *   Value to use with filter
 * 
 * @par Description
 * @parblock
 * This function requires a stream @a graph to be initialized previously to this call.
 * Use this function to create an input node to a data stream, the created node will be
 * returned through a reference to @a output.
 * 
 * Use this function to create a filter on a streaming node. The @a compare_op will be applied on the @a input
 * node on the given @a field using the @a value.
 * 
 * The output node containing the applied filter is returned through reference to @a output. 
 * 
 * A stream with four fields is defined below: a sensor ID number, a sensor type attribute, a sample timestamp, and a sample value:
 * @code
 * // Define fields for each sensor data record
 * enum {
 *     kSensorId,
 *     kSensorType,
 *     kSampleTime,
 *     kSampleValue,
 * };
 * static const db_fielddef_t fields[] = {
 *     { kSensorId, "sensor_id", DB_COLTYPE_UINT64, 0, 0, DB_NOT_NULL, NULL, 0 },
 *     { kSensorType, "sensor_type", DB_COLTYPE_UINT64, 0, 0, DB_NOT_NULL, NULL, 0 },
 *     { kSampleTime, "sample_time", DB_COLTYPE_TIMESTAMP, 0, 0, DB_NOT_NULL, NULL, 0 },
 *     { kSampleValue, "sample_value", DB_COLTYPE_FLOAT64, 0, 0, DB_NOT_NULL, NULL, 0 },
 * };
 * 
 * static size_t nfields = DB_ARRAY_DIM(fields);
 * @endcode
 * 
 * The following example demonstrates how to set up the stream environment shown below:
 * @code
 * dbstatus_t status;
 * db_stream_graph_t graph;
 * static int32_t buffer[1024 * 1024];
 * db_stream_node_t s0, s1, s2;
 * int32_t row_count = 0;
 * 
 * status = db_stream_create_graph(&graph, buffer, sizeof buffer);
 * 
 * if (DB_SUCCESS(status)) {
 *     // Create input node
 *        status = db_stream_create_row_input(&s0, graph, fields, nfields, kSampleTime, -1);
 * }
 * 
 * if (DB_SUCCESS(status)) {
 *     // Create filter node with equality operation
 *     status = db_stream_create_filter_uint64(&s1, s0, kSensorId, DB_SEEK_EQUAL, 1);
 * }
 * 
 * if (DB_SUCCESS(status)) {
 *     // Create output node
 *     status = db_stream_create_row_output(&s2, s1, &process_filtered_sensor_data, &row_count);
 * }
 * 
 * @endcode
 * @endparblock
 *  
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_row_input, db_stream_create_row_output,<br>
 * db_stream_create_filter_sint64, db_stream_create_filter_sint32, db_stream_create_filter_uint32, db_stream_create_filter_float32, db_stream_create_filter_float64
 */
dbstatus_t db_stream_create_filter_uint64(db_stream_node_t * output, db_stream_node_t input, db_fieldno_t field, db_seek_t compare_op, uint64_t value);

/**
 * @brief db_stream_create_filter_float32 creates a filter on the input node.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Output node from filter
 * @param input
 *   Input node to supply row to filter on 
 * @param field
 *   Field of input row to apply filter to
 * @param compare_op
 *   Type of filter to apply 
 * @param value
 *   Value to use with filter
 *  
 * @par Description
 * @parblock
 * Use this function to create a filter on a streaming node for #float32_t data type. The @a compare_op will be applied on the @a input
 * node on the given @a field using the @a value.
 * @code
 * dbstatus_t status;
 * db_stream_graph_t graph;
 * static int32_t buffer[1024 * 1024];
 * db_stream_node_t s0, s1, s2;
 * int32_t row_count = 0;
 * 
 * status = db_stream_create_graph(&graph, buffer, sizeof buffer);
 * 
 * if (DB_SUCCESS(status)) {
 *     // Create input node
 *     status = db_stream_create_row_input(&s0, graph, fields, nfields, kSampleTime, -1);
 *     // Create filter node with equality operation
 *     status = db_stream_create_filter_float32(&s1, s0, kSensorId, DB_SEEK_EQUAL, 1);
 *     // Create output node
 *     status = db_stream_create_row_output(&s2, s1, &process_filtered_sensor_data, &row_count);
 * }
 * @endcode
 *  
 * The output node containing the applied filter is returned through reference to @a output.
 * @endparblock
 * 
 * @returns
 *  DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_row_input, db_stream_create_row_output,<br>
 * db_stream_create_filter_sint64, db_stream_create_filter_uint64, db_stream_create_filter_sint32, db_stream_create_filter_uint32, db_stream_create_filter_float64 
 */
dbstatus_t db_stream_create_filter_float32(db_stream_node_t * output, db_stream_node_t input, db_fieldno_t field, db_seek_t compare_op, float32_t value);

/**
 * @brief db_stream_create_filter_float64 creates a filter on the input node.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Output node from filter
 * @param input
 *   Input node to supply row to filter on 
 * @param field
 *   Field of input row to apply filter to
 * @param compare_op
 *   Type of filter to apply 
 * @param value
 *   Value to use with filter 
 * 
 * @par Description
 * @parblock
 * Use this function to create a filter on a streaming node for #float64_t data type. The @a compare_op will be applied on the @a input
 * node on the given @a field using the @a value.
 * @code
 * dbstatus_t status;
 * db_stream_graph_t graph;
 * static int32_t buffer[1024 * 1024];
 * db_stream_node_t s0, s1, s2;
 * int32_t row_count = 0;
 * 
 * status = db_stream_create_graph(&graph, buffer, sizeof buffer);
 * 
 * if (DB_SUCCESS(status)) {
 *     // Create input node
 *     status = db_stream_create_row_input(&s0, graph, fields, nfields, kSampleTime, -1);
 *     // Create filter node with equality operation
 *     status = db_stream_create_filter_float64(&s1, s0, kSensorId, DB_SEEK_EQUAL, 1);
 *     // Create output node
 *     status = db_stream_create_row_output(&s2, s1, &process_filtered_sensor_data, &row_count);
 * }
 * @endcode
 *  
 * The output node containing the applied filter is returned through reference to @a output.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_row_input, db_stream_create_row_output,<br>
 * db_stream_create_filter_sint64, db_stream_create_filter_uint64, db_stream_create_filter_sint32, db_stream_create_filter_uint32, db_stream_create_filter_float32 
 */
dbstatus_t db_stream_create_filter_float64(db_stream_node_t * output, db_stream_node_t input, db_fieldno_t field, db_seek_t compare_op, float64_t value);

/**
 * @brief db_stream_create_time_interval_window creates a range of timestamps called a window.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Output node with window applied
 * @param input
 *   Input node that supplies row to window 
 * @param window_size
 *   Size of the window, in microseconds (usec)
 * @param window_slide
 *   Window slide interval, in microseconds (usec)
 * @param buffer_row_count
 *   Size of the buffer to queue the window (number of entries)
 *
 * @par Description
 * @parblock
 * 
 * Use this function to create a time window on a streaming node. Input rows will be automatically removed from the output when they are older than @a window_size microseconds. 
 * If @a window_slide is less than or equal to zero, each input row will be immediately emitted as output. Otherwise, the operator will only emit output every @a window_slide microseconds.
 * The @a buffer_row_count specifies the maximum number of input rows to buffer per window.
 * 
 * The output node containing the applied filter is returned through reference to @a output.
 * The following example shows how to add a time window filter to a continuous query:
 * 
 * @code
 * // Handle to a previously assigned stream output node
 * db_stream_node_t s0;
 * 
 * // Add time interval window: most recent 10 milliseconds
 * int32_t interval_usec = 10 * 1000;
 * 
 * // Slide the time window forward once every 1 milliseconds
 * int32_t slide_usec = 1 * 1000;
 * 
 * // Size of window queue buffer
 * size_t buffer_row_count = 20;
 * 
 * dbstatus_t status;
 * db_stream_node_t s1;
 * status = db_stream_create_time_interval_window(&s1, s0, interval_usec, slide_usec, buffer_row_count);
 * 
 * int32_t row_count = 0;
 * 
 * if(DB_SUCCESS(status)){
 *     // process intermediate stream results
 *     db_stream_node_t s2;
 *     status = db_stream_create_row_output(&s2, s1, &process_filtered_sensor_data, &row_count);
 * }
 * 
 * if(DB_SUCCESS(status)){
 * 
 *     // ... process query results
 * }
 * @endcode
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_row_output
 */
dbstatus_t db_stream_create_time_interval_window(db_stream_node_t * output, db_stream_node_t input, db_interval_t window_size, db_interval_t window_slide, int32_t buffer_row_count);

/**
 * @brief db_stream_create_aggregates creates a list of aggregates to apply. 
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Output node with aggregate(s) applied
 * @param input
 *   Input node that supplies entries to the aggregates
 * @param field_list
 *   List of aggregate fields to output from this stream node. 
 * @param field_count
 *   Number of fields to aggregate.
 * 
 * @par Description
 * @parblock
 * To aggregate nodes, the callback function must be modified to handle a running total.
 * The following example shows how to modify the callback function to aggregate nodes:
 * This function creates a list of aggregates to be applied on the given @a field_list.
 * The output node containing the applied filter is returned through reference to @a output.
 * 
 * @code
 * // Handle to a previously assigned stream output node
 * db_stream_node_t s0;
 * 
 * // Define fields for the aggregate node
 * enum {
 *     kCount,
 *     kAverage
 * };
 * db_aggregate_field_t aggregate_list[] = {
 * { -1,           DB_AGGREGATE_COUNT,   NULL }, // kCount
 *  { kSampleValue, DB_AGGREGATE_AVERAGE, NULL } // kAverage
 * };
 * 
 * void process_aggregate(db_stream_node_t s, void * argument
 * {
 *     char tag = db_stream_get_tag(s);
 *     int32_t row_count = db_stream_get_sint32(s, kCount);
 *     db_float64_t average_value = db_stream_get_float64(s, kAverage);
 * }
 * dbstatus_t status;
 * db_stream_node_t s1;
 * 
 * status = db_stream_create_aggregates(&s1, s0, aggregate_list, DB_ARRAY_DIM(aggregate_list));
 * 
 * if(DB_SUCCESS(status)){
 *     // process intermediate stream query results
 *     db_stream_node_t s2;
 *     status = db_stream_create_row_output(&s2, s1, &process_aggregate, NULL);
 * }
 * 
 * if(DB_SUCCESS(status)){
 * 
 *     // ...process query results here
 * }
 * @endcode
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_time_interval_window
 */
dbstatus_t db_stream_create_aggregates(db_stream_node_t * output, db_stream_node_t input, const db_aggregate_field_t * field_list, size_t field_count);

/**
 * @brief db_stream_create_join_all_within joins a number of input nodes into one output.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Output node with join applied
 * @param input_list
 *   List of input nodes to join on
 * @param join_key_list
 *   List of keys to join inputs on
 * @param join_source_count
 *   Numer of inputs to join on
 * @param within_time_interval
 *   Window of time (in microseconds) to join on
 * @param buffer_row_count
 *   Size of the buffer to queue the join results (number of entries)
 * 
 * @par Description
 * @parblock
 * This function joins the row entries from the stream nodes defined in @a
 * input_list according the keys defined in the @a join_source_count and within
 * the time interval defined by @a within_time_interval.
 * 
 * The following example combines input from sensor 1 with input from sensor 2 that arrives during the same one second window:
 * @code
 * // Handle to a previously initialized stream graph
 * db_stream_graph_t graph;
 * 
 * enum {
 *     kPressureSensorId,
 *     kPressureSampleTime,
 *     kPressurePressureKPa,
 * };
 * // Define another stream to join with
 * static const db_fielddef_t pressure_fields[] = {
 *     { kPressureSensorId, "sensor_id", DB_COLTYPE_UINT64, 0, 0, DB_NOT_NULL, NULL, 0 },
 *     { kPressureSampleTime, "sample_time", DB_COLTYPE_TIMESTAMP, 0, 0, DB_NOT_NULL, NULL, 0 },
 *     { kPressurePressureKPa, "pressure_kPa", DB_COLTYPE_FLOAT64, 0, 0, DB_NOT_NULL, NULL, 0 },
 * };
 * static const size_t pressure_nfields  = DB_ARRAY_DIM(pressure_fields);
 * 
 * // Create input node with buffer for one row
 * db_stream_node_t s0;
 * db_stream_create_row_input(&s0, graph, pressure_fields, pressure_nfields, kPressureSampleTime, -1);
 * 
 * // Create another input node with buffer for one row
 * db_stream_node_t s1;
 * db_stream_create_row_input(&s1, graph, fields, nfields, kSampleTime, -1);
 * 
 * db_stream_node_t s2;
 * db_stream_node_t join_sources[2] = { s0, s1 };
 * db_fieldno_t join_keys[2] = { kPressureSensorId, kSensorId };
 * int32_t interval_usec = 1000 * 1000;
 * 
 * // Size of window queue buffer
 * size_t buffer_row_count = 20;
 * 
 * dbstatus_t status;
 * 
 * status = db_stream_create_join_all_within(&s2, join_sources, join_keys, DB_ARRAY_DIM(join_sources), interval_usec, buffer_row_count);
 * 
 * while (DB_SUCCESS(status)) {
 *     // ... process input into the stream
 * }
 * @endcode
 * 
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_aggregates,<br>
 * db_stream_create_row_input, db_stream_node_t
 */
dbstatus_t db_stream_create_join_all_within(db_stream_node_t * output, db_stream_node_t * input_list, db_fieldno_t * join_key_list, int32_t join_source_count, int64_t within_time_interval, int32_t buffer_row_count);


/// How to materialize streaming events
typedef enum {
    /// Refresh table from continuous query result: record insert, update,
    /// and delete events.
    DB_MATERIALIZE_REFRESH,
    /// Perform in-place updates to table without removal: record insert
    /// and update events, ignore delete events.
    DB_MATERIALIZE_IN_PLACE,
    /// Append full history of query results as table rows: append insert
    /// and update events, ignore delete events.
    DB_MATERIALIZE_APPEND,
    /// Output to time series columns: add row for insert events, put for
    /// update events, ignore delete events.
    DB_MATERIALIZE_TIME_SERIES,
} db_materialization_policy_t;

/// Configuration for #db_stream_create_table_output
typedef struct db_table_output_policy_t {
    /// How to materialize streaming events
    db_materialization_policy_t materialization_policy;

    /// Number of stream events to process per table insert or update.
    /// If the scale down factor is less than or equal to 1, all events are recorded to the table.
    /// Otherwise, events to skip are randomly selected.
    int32_t scale_down_factor;

    /// Maximum number of values to store in each time series column.
    /// This value is only used when policy is DB_MATERIALIZE_TIME_SERIES.
    int32_t time_series_size;
} db_table_output_policy_t;


/**
 * @brief db_stream_create_table_output creates a node to output stream events into a table.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Output node for table output
 * @param input
 *   Input node that supplies entries to the table
 * @param database
 *   Database connection
 * @param table_name
 *   Name of table to open or create
 * @param flags
 *   Must be #DB_CREATE_OR_OVERWRITE.
 *   Flags which specify whether the table should be created or must already exist.
 * @param policy
 *   Table output policy defines how stream events are mapped to table rows.
 * @param buffer_row_count
 *   Maximum number of rows to update or refresh.
 * 
 * @par Description
 * @parblock
 * This function can write the state of a stream processing node continuously to a database table .
 * The application must open a connection to a local existing database and have a #db_t connection handle will be needed to create a table output node. 
 * This connection should only be used in the thread that executes #db_stream_process.
 * @code
 * dbstatus_t status;
 * // Already opened database through db_open_iot_file_storage
 * db_t database;
 * db_stream_node_t input_node = ...;
 * 
 * db_table_output_policy_t policy;
 * memset(&policy, 0, sizeof policy);
 * 
 * policy.materialization_policy = DB_MATERIALIZE_REFRESH;
 * 
 * db_stream_node_t table_output_node;
 * status = db_stream_create_table_output(
 *     &table_output_node,
 *     input_node,
 *     database,
 *     "table1",
 *     DB_CREATE_OR_OVERWRITE,
 *     &policy,
 *     10);
 * 
 * while (DB_SUCCESS(status)) {
 *     // ... process input into the stream
 * }
 * @endcode
 * 
 * Note that each call to #db_stream_process that produces a change on the input node can result in an insert, update, or delete on the database table.
 * Before disconnecting the database connection, the application must first destroy any table output nodes using that connection. 
 * This is accomplished by freeing the stream graph using #db_stream_free_graph
 * @endparblock
 * 
 * Replace #DB_OPEN_OR_CREATE with one of:
 * - #DB_CREATE_OR_OVERWRITE to drop and overwrite an existing table.
 * - #DB_OPEN_EXISTING to open the table only if it already exists.
 * - #DB_CREATE_NEW to create a new table only if the table does not already exist.
 * 
 * @returns DB_NOERROR on success.
 * @returns negative error code on failure.
 * 
 * @see db_open_iot_file_storage, db_stream_process, DB_SUCCESS,<br>
 * DB_MATERIALIZE_REFRESH, DB_MATERIALIZE_IN_PLACE, DB_MATERIALIZE_APPEND, DB_MATERIALIZE_TIME_SERIES
 */
dbstatus_t db_stream_create_table_output(db_stream_node_t * output, db_stream_node_t input, db_t database, const char * table_name, uint32_t flags, db_table_output_policy_t * policy, int32_t buffer_row_count);


/**
 * @brief db_stream_create_index_output creates a node to output stream events into an index.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] output
 *   Output node for index output
 * @param input
 *   Input node that supplies entries to the index
 * @param database
 *   Database connection
 * @param index_id
 *   Identifier for an existing index in the database
 * @param field_list
 *   Schema of the index identified by @a index_id
 * @param field_count
 *   Number of fields in @a field_list
 * @param policy
 *   Table output policy defines how stream events are mapped to table rows.
 * 
 * @par Description
 * @parblock
 * This function can write the state of a stream processing node continuously to a database index.
 * The application must open a connection to a local existing database and have a #db_t connection handle will be needed to create a table output node. 
 * This connection should only be used in the thread that executes #db_stream_process.
 * 
 * Note that each call to #db_stream_process that produces a change on the input node can result in an insert, update, or delete on the database table.
 * Before disconnecting the database connection, the application must first destroy any table output nodes using that connection. 
 * This is accomplished by freeing the stream graph using #db_stream_free_graph
 * 
 * Only DB_MATERIALIZE_REFRESH and DB_MATERIALIZE_IN_PLACE policies can be used with index output.
 * @endparblock
 * 
 * @returns DB_NOERROR on success.
 * @returns DB_EFIELD when the fields of @a input are incompatible with @a field_list.
 * @returns negative error code on failure.
 * 
 * @see db_open_iot_file_storage, db_stream_process, DB_SUCCESS,<br>
 * DB_MATERIALIZE_REFRESH, DB_MATERIALIZE_IN_PLACE, DB_MATERIALIZE_APPEND, DB_MATERIALIZE_TIME_SERIES
 */
dbstatus_t db_stream_create_index_output(db_stream_node_t * output, db_stream_node_t input, db_t database, int index_id, const db_fielddef_t * field_list, const db_len_t field_count, const db_table_output_policy_t * policy);

/**
 * @brief Describe the fields of a stream node.
 * 
 * @param node
 *   An input or output node.
 * @param [out] field_list
 *   Destination array for .
 * @param max_field_count
 *   Number of elements in @a field_list array.
 * @return the total number of fields in the stream as a positive `dbstatus_t` value.
 * @return a negative `dbstatus_t` value on failure.
 */
dbstatus_t db_stream_describe_fields(db_stream_node_t node, db_fielddef_t * field_list, const db_len_t max_field_count);


/**
 * @brief db_stream_create_environment allocates a namespace of registered Data Streams
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] stream_env
 *   Destination for environment handle
 * 
 * @par Description
 * @parblock
 * The stream processing environment must be initialized before creating any continuous queries using #db_init_ex
 * A stream processing environment connects queries across multiple tasks. 
 * Before starting tasks, initialize a db_stream_environment_t variable in the global scope, or a scope shared with multiple tasks.
 * 
 * @code
 * db_stream_environment_t stream_env = NULL;
 * @endcode
 * 
 * The following statement must run to initialize the environment before starting either task in the following subsections:
 * @code
 * db_stream_create_environment(&stream_env);
 * @endcode
 * 
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_init_ex, db_stream_free_environment
 */
dbstatus_t db_stream_create_environment(db_stream_environment_t * stream_env);

/**
 * @brief db_stream_free_environment frees an environment previously created.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param stream_env
 *   Stream environment handle
 * 
 * @par Description
 * @parblock
 * Use this function to free the memory used by @a stream_env. All stream
 * graphs with output registered must be made free before the corresponding
 * environment(s).
 * 
 * A stream processing environment connects queries across multiple tasks.
 * @code
 * // stream_env was previously created
 * db_stream_environment_t stream_env;
 * @endcode
 * 
 * The following statement must run to free the stream processing environment environment
 * @code
 * db_stream_free_environment(stream_env);
 * @endcode
 * 
 * The environment handle must not be used after this function is called.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_environment,<br>
 * db_stream_free_graph
 */
void db_stream_free_environment(db_stream_environment_t stream_env);


/**
 * @brief db_stream_register_output registers stream output as a real-time view
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] node
 *   Destination node for further processing on the same graph
 * @param input
 *   Stream node to use as input to the real-time view
 * @param stream_env
 *   Stream environment handle in which to register the real-time view
 * @param stream_name
 *   Name of real-time view to register in the environment
 * 
 * @par Description
 * @parblock
 * This functions registers an output to create a view of the stream.
 * Since the aggregate node receives input from the 1-second window node, running averages are calculated over a 1-second window and output is refreshed once per second.
 * 
 * The below call register output to create the downsample real-time view for a running window query of aggregate operation:
 * 
 * @code
 * dbstatus_t Status;
 * //Given all the below initialized parameters:
 * db_stream_graph_t graph; * 
 * db_stream_node_t input_node;
 * db_stream_node_t window_node;
 * db_stream_node_t aggregate_node;
 * db_stream_node_t registration_node;
 * db_stream_node_t stream_output_node;
 * 
 * Status = db_stream_register_output(
 *     &registration_node,
 *     aggregate_node,
 *     stream_env,
 *     "downsample"
 *     );
 * @endcode
 * 
 * Refer to our tutorials on Streams to get full example code.
 * @endparblock
 *
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_create_row_output, db_stream_create_queued_input
 */
dbstatus_t db_stream_register_output(db_stream_node_t * node, db_stream_node_t input, db_stream_environment_t stream_env, const char * stream_name);

/** 
 * When reading input from a registered stream, process updates as inserts and
 * ignore deletes.
 * @see db_stream_create_queued_input
 */
#define DB_FOR_SYSTEM_TIME_ALL 1u

/**
 * @brief db_stream_create_queued_input creates an input queue from a named stream into a streaming graph.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param [out] node
 *   Destination for the created node
 * @param graph
 *   Graph in which to create the node and its dependents
 * @param stream_env
 *   Stream environment handle in which to find the real-time view
 * @param stream_name
 *   Name of real-time view to previously registered in the environment
 * @param flags
 *   Bitwise combination of:
 *   - #DB_FOR_SYSTEM_TIME_ALL: process updates as inserts and ignore deletes
 * @param queue_row_count
 *   Minimum number of rows for which to allocate a queue
 * 
 * @par Description
 * @parblock
 * 
 * An input queue is added to the @a graph and filled from @a stream_name in
 * the @a stream_env environment. The queue is read into @a node when
 * #db_stream_process_queued_input is called.
 * Assuming a data processing graph that has five nodes, an input node, two processing operators, and two output nodes:
 * 
 * @code
 * // Given all the below initialized parameters:
 * db_stream_graph_t graph;
 * db_stream_node_t input_node;
 * db_stream_environment_t stream_env;
 * do {
 *     Status = db_stream_create_queued_input(
 *         &input_node,            // [out] node
 *          graph,                  // graph 
 *          stream_env,             // stream_env 
 *         "sensors",              // stream_name
 *         DB_FOR_SYSTEM_TIME_ALL, // flags 
 *         1000);                  // queue_row_count
 *     // Repeat until "sensors" is registered by the ingestion task.
 * } while (Status == DB_ENOTABLE);
 * @endcode
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_process_queued_input
 */
dbstatus_t db_stream_create_queued_input(db_stream_node_t * node, db_stream_graph_t graph, db_stream_environment_t stream_env, const char * stream_name, uint32_t flags, size_t queue_row_count);

/**
 * @brief db_stream_process_queued_input processes all queued input on a given graph
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param graph
 *   Graph in which to create the node and its dependents
 * 
 * @par Description
 * @parblock
 * After the stream processing graph for the data processing task has been fully initialized, 
 * the following code will process all queries in the data processing graph .
 * 
 * @code
 * // Process queued input until `db_stream_finish_processing` is called
 * // in another task or by an interrupt.
 * Status = db_stream_process_queued_input(graph);
 * @endcode
 * 
 * #db_stream_process_queued_input is thread-safe, provided graph nodes are
 * only created before it is first called.
 * #db_stream_process_queued_input runs until all queued inputs have been processed and
 * #db_stream_finish_processing is called for the same @a graph.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * @returns
 *   DB_ESTATE if the graph contains no queues
 * 
 * @see db_stream_finish_processing, db_stream_create_queued_input
 */
dbstatus_t db_stream_process_queued_input(db_stream_graph_t graph);

/**
 * @brief Configuration options for execution of a stream SQL statement.
 */
typedef struct db_stream_sql_config_s {
    /**
     * @brief Reserve enough memory to queue this number of rows per input
     * stream listed in the FROM and JOIN clauses.
     */
    size_t input_queue_row_count;
} db_stream_sql_config_t;

/**
 * @brief db_stream_create_sql creates a continuous query node from an SQL
 * CSELECT statement.
 * @param [out] node
 *   Destination for the created node
 * @param graph
 *   Graph in which to create the node and its dependents
 * @param stream_env
 *   Stream environment handle in which to find named streams
 * @param statement
 *   The CSELECT statement text to execute.
 * @param config
 *   Optional configuration options.
 * 
 * @par Description
 * @parblock
 * An input queue is added to the @a graph for each stream named in the query.
 * The results of the SQL CSELECT query are output from @a node when
 * #db_stream_process_queued_input is called.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 */
dbstatus_t db_stream_create_sql(db_stream_node_t * node, db_stream_graph_t graph, db_stream_environment_t stream_env, const char * statement, const db_stream_sql_config_t * config);

/**
 * @brief db_stream_finish_processing stops processing once all queues are empty
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_stream.h>
 * @endcode
 * 
 * @param graph
 *   Graph in which to create the node and its dependents
 * 
 * @par Description
 * @parblock
 * All inputs queued before #db_stream_finish_processing is called are guaranteed
 * to be processed in the next or a concurrent call to #db_stream_process_queued_input.
 * 
 * @code
 * // Assuming a graph is created and streaming processing pipelines of nodes is defined
 * db_stream_graph_t graph;
 * db_stream_finish_processing(graph);
 * Status = db_stream_process_queued_input(graph);
 * @endcode
 * 
 * This function affects the next call to #db_stream_process_queued_input for
 * the same @a graph in any thread, and can be used to stop a concurrent
 * #db_stream_process_queued_input that is already running in another thread.
 * 
 * #db_stream_finish_processing is thread-safe.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 * 
 * @see db_stream_process_queued_input
 */
dbstatus_t db_stream_finish_processing(db_stream_graph_t graph);

/** @} */

C_HEADER_END

#endif // DB_STREAM_H
