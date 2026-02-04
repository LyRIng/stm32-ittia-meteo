/**************************************************************************/
/*                                                                        */
/*      METEO Streams Implementation                                      */
/*                                                                        */
/**************************************************************************/

#include "meteo_streams.h"

#include <stdio.h>
#include <stdlib.h>

/* ITTIA DB stream processing */
#include <ittia/db/db_stream.h>
/* Error names and descriptions */
#include "dbs_error_info.h"

int init_meteo_readings_stream(
    db_stream_environment_t stream_env,
    db_stream_graph_t graph,
    db_stream_node_t * input_node,
    db_stream_node_t * output_node)
{
    dbstatus_t status;

    /* Create an input node for the `meteo_readings4` stream */
    static const db_fielddef_t fields[] = {
        { kMeteoReadingsId,          "id",             DB_COLTYPE_SINT32,    0, 0, DB_NOT_NULL, NULL, 0 },
        { kMeteoReadingsTs,          "ts",             DB_COLTYPE_TIMESTAMP, 0, 0, DB_NOT_NULL, NULL, 0 },
        { kMeteoReadingsTemperature, "temperature",    DB_COLTYPE_FLOAT64,   0, 0, DB_NOT_NULL, NULL, 0 },
        { kMeteoReadingsWindSpeed,   "wind_speed",     DB_COLTYPE_FLOAT64,   0, 0, DB_NOT_NULL, NULL, 0 },
        { kMeteoReadingsWindDirection,"wind_direction",DB_COLTYPE_FLOAT64,   0, 0, DB_NOT_NULL, NULL, 0 },
    };
    
    /* Create row input with primary key on 'id' field */
    static const db_fieldno_t key_field_array[] = { kMeteoReadingsId };
    status = db_stream_create_row_input_compound_key(
        input_node, 
        graph, 
        fields, 
        DB_ARRAY_DIM(fields), 
        kMeteoReadingsTs,  // Timestamp field for ordering
        key_field_array, 
        DB_ARRAY_DIM(key_field_array));
    /*----------------------------------------------------------------*/

    if (DB_SUCCESS(status)) {
        /* Register output to create a real-time view */
        status = db_stream_register_output(output_node, *input_node, stream_env, "meteo_readings4");
    }
    /*----------------------------------------------------------------*/

    if (DB_FAILED(status)) {
        fprintf(stderr,
            "Cannot create `meteo_readings4` real-time stream: %s\n",
            dbs_get_error_info(status).description);
        return EXIT_FAILURE;
    }

    printf("Created `meteo_readings4` real-time stream\n");
    return EXIT_SUCCESS;
}

dbstatus_t put_meteo_readings_stream(db_stream_node_t node, const meteo_readings_row_t * row)
{
    db_stream_set_sint32(node, kMeteoReadingsId, row->id);
    db_stream_set_timestamp_usec(node, kMeteoReadingsTs, row->ts);
    db_stream_set_float64(node, kMeteoReadingsTemperature, row->temperature);
    db_stream_set_float64(node, kMeteoReadingsWindSpeed, row->wind_speed);
    db_stream_set_float64(node, kMeteoReadingsWindDirection, row->wind_direction);

    return db_stream_process(node);
}

void get_meteo_readings_stream(db_stream_node_t node, meteo_readings_row_t * row)
{
    row->id            = db_stream_get_sint32(node, kMeteoReadingsId);
    row->ts            = db_stream_get_timestamp_usec(node, kMeteoReadingsTs);
    row->temperature   = db_stream_get_float64(node, kMeteoReadingsTemperature);
    row->wind_speed    = db_stream_get_float64(node, kMeteoReadingsWindSpeed);
    row->wind_direction = db_stream_get_float64(node, kMeteoReadingsWindDirection);
}
