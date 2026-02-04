/**************************************************************************/
/*                                                                        */
/*      METEO Database Implementation Header                              */
/*      Inline database API functions                                     */
/*                                                                        */
/**************************************************************************/

#ifndef METEO_DATABASE_IMPL_H
#define METEO_DATABASE_IMPL_H

#include <ittia/db/db_index_storage.h>

/* Include this file only from meteo_database.h */

/* Table: meteo_readings4 ----------------------------------------- */

static inline dbstatus_t put_meteo_readings(db_t database, const meteo_readings_row_t *meteo_array, size_t count)
{
    /* Batch insert - not implemented in this basic version */
    return DB_ENOTIMPL;
}

/**
 * @brief Insert a METEO reading (stream-based, no cursor needed)
 * 2/2/26 Addition by C
 * In ITTIA DB Lite stream architecture, data insertion happens
 * through the stream pipeline, not via table cursors.
 */
static inline dbstatus_t insert_meteo_readings(
    db_t database,
    const meteo_readings_row_t* row)
{
    // Stream-based insertion handled by ProcessMeteoFrameToStream()
    // This function exists for API compatibility but is not used
    return DB_NOERROR;
}

static inline dbstatus_t update_meteo_readings(db_t database, const meteo_readings_row_t * updated_meteo_with_same_pk)
{
    /* Update - not implemented in this basic version */
    return DB_ENOTIMPL;
}

static inline dbstatus_t delete_meteo_readings(db_t database, const int32_t id)
{
    /* Delete - not implemented in this basic version */
    return DB_ENOTIMPL;
}

static inline dbstatus_t scan_meteo_readings_by_PK(db_t database, meteo_readings_row_t *meteo_result, size_t max_result_count, size_t* result_count)
{
    /* Scan - not implemented in this basic version */
    return DB_ENOTIMPL;
}

static inline dbstatus_t find_meteo_readings_by_PK(db_t database, const int32_t id, meteo_readings_row_t *meteo_result)
{
    /* Find - not implemented in this basic version */
    return DB_ENOTIMPL;
}

static inline void destroy_meteo_readings(db_t database, meteo_readings_row_t *meteo)
{
    /* No dynamic memory to free in basic struct */
}

// New 1.2.26
static inline dbstatus_t output_stream_to_meteo_readings_table(db_stream_node_t input_node, db_t database, const db_table_output_policy_t * policy)
{
    /* Stream to table output */
    db_stream_node_t output_node;
    dbstatus_t rc = db_stream_create_table_output(
        &output_node,           // output parameter (result)
        input_node,             // input node
        database,               // database handle
        "meteo_readings4",      // table name
        0,                      // flags (0 = default)
        (db_table_output_policy_t*)policy,  // policy (cast away const)
        1                       // buffer_row_count (1 = minimal buffering)
    );
    return rc;
}

#endif // METEO_DATABASE_IMPL_H
