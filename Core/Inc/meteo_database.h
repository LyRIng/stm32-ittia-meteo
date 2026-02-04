/**************************************************************************/
/*                                                                        */
/*      METEO Database Schema - Adapted for ITTIA DB                     */
/*      Based on meteo_schema.sql for ITTIA Analitica integration        */
/*                                                                        */
/**************************************************************************/

#ifndef METEO_DATABASE_H
#define METEO_DATABASE_H

#include <ittia/db/db_index_storage.h>
#include <ittia/db/db_stream.h>

C_HEADER_BEGIN

/** @brief Open or create a database that uses the meteo schema.
 * @param database_name A string used to identify the database.
 * @param config Database configuration options.
 * @return DB_NOERROR when the database is successfully opened.
 * @return A negative dbstatus_t error code when the database fails to open.
 */
dbstatus_t open_meteo_database(const char * database_name, const db_database_config_t * config);

/* Table meteo_readings4 ------------------------------- */
typedef struct meteo_readings_row_s {
    int32_t id;                          // Primary key
    db_timestamp_usec_t ts;              // Timestamp in microseconds
    double temperature;                   // Temperature (converted from ADC)
    double wind_speed;                    // Wind speed
    double wind_direction;                // Wind direction in degrees
} meteo_readings_row_t;

/* Database API functions */
static inline dbstatus_t put_meteo_readings(db_t database, const meteo_readings_row_t *meteo_array, size_t count);
static inline dbstatus_t insert_meteo_readings(db_t database, const meteo_readings_row_t *meteo);
static inline dbstatus_t update_meteo_readings(db_t database, const meteo_readings_row_t * updated_meteo_with_same_pk);
static inline dbstatus_t delete_meteo_readings(db_t database, const int32_t id);
static inline dbstatus_t scan_meteo_readings_by_PK(db_t database, meteo_readings_row_t *meteo_result, size_t max_result_count, size_t* result_count);
static inline dbstatus_t find_meteo_readings_by_PK(db_t database, const int32_t id, meteo_readings_row_t *meteo_result);
static inline void destroy_meteo_readings(db_t database, meteo_readings_row_t *meteo);
static inline dbstatus_t output_stream_to_meteo_readings_table(db_stream_node_t input_node, db_t database, const db_table_output_policy_t * policy);

C_HEADER_END

#endif // METEO_DATABASE_H

#include "meteo_database_impl.h"
