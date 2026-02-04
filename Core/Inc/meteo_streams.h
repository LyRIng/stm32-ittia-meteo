/**************************************************************************/
/*                                                                        */
/*      METEO Streams - Real-time stream processing for meteorological   */
/*      data using ITTIA DB streaming API                                 */
/*                                                                        */
/**************************************************************************/

#ifndef METEO_STREAMS_H
#define METEO_STREAMS_H

/* ITTIA DB stream processing */
#include <ittia/db/db_stream.h>

#include "meteo_database.h"


/** @brief Fields of the `meteo_readings4` real-time stream
 *
 * @code{.sql}
 * CREATE STREAM meteo_readings4 (
 *     id                   INTEGER PRIMARY KEY,
 *     ts                   TIMESTAMP NOT NULL,
 *     temperature          DOUBLE PRECISION NOT NULL,
 *     wind_speed           DOUBLE PRECISION NOT NULL,
 *     wind_direction       DOUBLE PRECISION NOT NULL
 * );
 * @endcode
 */
enum {
    kMeteoReadingsId,           // int32
    kMeteoReadingsTs,           // timestamp
    kMeteoReadingsTemperature,  // float64 (double)
    kMeteoReadingsWindSpeed,    // float64 (double)
    kMeteoReadingsWindDirection // float64 (double)
};

/**
 * @brief Initialize the meteo_readings4 stream
 * @param stream_env Stream environment
 * @param graph Stream graph
 * @param input_node Output parameter for input node
 * @param output_node Output parameter for output node
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int init_meteo_readings_stream(
    db_stream_environment_t stream_env,
    db_stream_graph_t graph,
    db_stream_node_t * input_node,
    db_stream_node_t * output_node);

/**
 * @brief Put a meteo reading into the stream
 * @param node Stream node
 * @param row Meteo reading data
 * @return DB_NOERROR on success
 */
dbstatus_t put_meteo_readings_stream(db_stream_node_t node, const meteo_readings_row_t * row);

/**
 * @brief Get a meteo reading from the stream
 * @param node Stream node
 * @param row Output parameter for meteo reading data
 */
void get_meteo_readings_stream(db_stream_node_t node, meteo_readings_row_t * row);

#endif // METEO_STREAMS_H
