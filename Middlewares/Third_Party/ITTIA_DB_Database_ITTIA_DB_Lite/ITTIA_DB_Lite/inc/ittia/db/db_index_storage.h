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

/** @file db_index_storage.h
 *
 * Entry point for the ITTIA Index Storage C API. Part of the ITTIA DB Lite
 * Embedded C API.
 *
 * Note: Functions in this header file should not be called directly. Use only
 * with the ITTIA database API code generator.
 */

#ifndef DB_INDEX_STORAGE_H
#define DB_INDEX_STORAGE_H

#include <ittia/db/db_environment.h>
#include <ittia/db/db_iot_storage.h>
#include <ittia/db/db_coltype.h>

C_HEADER_BEGIN

/** Index handle. */
DB_DECLARE_HANDLE(db_index_t);

typedef struct db_index_buffer_s {
    uint8_t * row_data;
    size_t buffer_size;
    size_t data_size;
} db_index_buffer_t;

typedef uint16_t db_time_series_id_t;

/** @brief Open an index in an ITTIA DB database to read or write data.
  * @param [out] handle
  *   A handle used to access index data is assigned to @a handle.
  * @param db
  *   A local database connection.
  * @param index_id
  *   Unique identifier for the index.
  * @param [out] buffer
  *   Reserve a connection local scratch buffer until the index is closed, unless @a buffer is @c NULL.
  * @return dbstatus_t value DB_NOERROR when successful.
  */
dbstatus_t db_open_index(db_index_t * handle, db_t db, int index_id, db_index_buffer_t * buffer);

dbstatus_t db_close_index(db_index_t handle);

/**
 * @brief Insert or overwrite one or more index entries.
 *
 * @param handle
 *   An open index.
 * @param data
 *   Pointer to a data buffer containing an index entry.
 * @param key_size
 *   Number of key bytes in @a data.
 * @param value_size
 *   Number of value bytes in @a data immediately following the key.
 * @return dbstatus_t value DB_NOERROR when successful.
 */
dbstatus_t db_index_put(db_index_t handle, const void * data, size_t key_size, size_t value_size);

dbstatus_t db_index_remove(db_index_t handle, const void * data, size_t key_size, db_index_buffer_t * removed_value);

/**
 * @brief Find and get the first index entry that is greater or equal to the given key.
 *
 * @param handle
 *   An open index.
 * @param key
 *   Pointer to a key data buffer.
 * @param key_size
 *   Number of bytes in @a key.
 * @param key_fields
 *   Number of key fields to compare in a compound key.
 * @param [out] data
 *   A pointer to a buffer containing the data of the matching index entry is assigned to @a data.
 * @param [out] data_size
 *   The number of bytes in @a data is assigned to @a data_size.
 * @return dbstatus_t value DB_NOERROR when successful.
 * @return dbstatus_t value DB_ENOTFOUND when no matching index entry is found.
 *
 * The buffer pointed to by @a data is only valid until any other function is
 * called with the same @a handle argument.
 *
 * Call db_index_close to release @a data pointer when it is is no longer
 * needed.
 */
dbstatus_t db_index_get(db_index_t handle, const void * key, size_t key_size, size_t key_fields, const void ** data, size_t * data_size);

typedef struct db_index_range_s {
    const void * low_key;
    const void * high_key;
    size_t low_key_size;
    size_t high_key_size;
    size_t key_fields;
    uint32_t key_flags;
} db_index_range_t;

dbstatus_t db_index_get_range(
    db_index_t handle,
    const db_index_range_t * range,
    size_t max_row_count,
    db_index_buffer_t * buffer);

/**
 * @brief Reserve ID numbers for one or more time series.
 *
 * @param db
 *   A local database connection.
 * @param [out] id_array
 *   Destination array for the reserved ID values.
 * @param id_count
 *   Number of ID values to reserve.
 * @return dbstatus_t value DB_NOERROR when successful.
 * @return dbstatus_t value DB_EINVAL when any argument is invalid.
 */
dbstatus_t db_reserve_time_series_id(db_t db, db_time_series_id_t id_array[], size_t id_count);

/**
 * @brief Open a time series identified its ID number.
 * 
 * @param [out] series_handle
 *   Reference to the time series.
 * @param db_handle
 *   Handle to an ITTIA DB database connection.
 * @param id
 *   The number reserved to identify the time series.
 * @param type
 *   Type of data to store in the time series.
 * @return dbstatus_t value DB_NOERROR when successful.
 * @return dbstatus_t value DB_EINVAL when any argument is invalid.
 */
dbstatus_t db_open_time_series_by_id(db_time_series_t * series_handle, db_t db_handle, db_time_series_id_t id, db_coltype_t type);

/**
 * @brief Release reserved time series ID numbers.
 * 
 * @param db
 *   A local database connection.
 * @param id_array
 *   Destination array for the reserved ID values.
 * @param id_count
 *   Number of ID values to release.
 * @return dbstatus_t value DB_NOERROR when successful.
 * @return dbstatus_t value DB_EINVAL when any argument is invalid.
 */
dbstatus_t db_unreserve_time_series_id(db_t db, const db_time_series_id_t id_array[], size_t id_count);

// Return values for db_index_compare_keys_t
#define BTREE_KEY_LT 0
#define BTREE_KEY_SUFFIX_LT 1
#define BTREE_KEY_EQ 2
#define BTREE_KEY_SUFFIX_GT 3
#define BTREE_KEY_GT 4
#define BTREE_KEY_COMPARE_NONE 5

// Key comparison callback function type
typedef dbstatus_t (*db_index_compare_keys_t)(const void * key1, const void * key2, size_t key_field_count, uint32_t flags);

typedef const struct db_media_driver_s * db_media_driver_t;

/**
 * @brief @c db_database_config_t defines database configuration parameters to
 * open or create an ITTIA DB database on a storage media.
 * @ingroup CAPIDatabase
 */
typedef struct db_database_config_t {
    /** @brief The storage media driver on which to create or open a storage. */
    db_media_driver_t driver;
    /** @brief An optional pointer to memory used by the driver. */
    void * driver_info;
    /** @brief An unsigned integer of flags that control how the storage is
     * created, including:
     * - @c DB_OPEN_EXISTING
     * - @c DB_OPEN_OR_CREATE
     * - @c DB_CREATE_OR_OVERWRITE
     * - @c DB_CREATE_NEW
     */
    uint32_t flags;
    /** @brief A signed integer that specifies the maximum size of each database
     * page. */
    int32_t page_size;
    /** @brief A pointer to the base address of the database page cache memory
     * segment. */
    void * storage_cache_segment;
    /** @brief A integer that specifies the size of the database page cache
     * memory segment, in bytes. */
    size_t storage_cache_size;
    /** @brief (optional) A pointer to a #db_auth_info_t structure that specifies
     * the authorization credentials for a password-protected or encrypted
     * database, or <tt>NULL</tt>. */
    db_auth_info_t * auth_info;
} db_database_config_t;

/**
 * @brief Open or create an ITTIA DB database.
 * @ingroup CAPIDatabase
 *
 * @param storage_name
 *   A string containing a unique name for the storage.
 * @param config
 *   Database storage configuration parameters.
 * @param index_compare_func_array
 *   An array of key comparison functions with @a index_count elements.
 * @param index_count
 *   The number of indexes to initialize.
 * @return DB_NOERROR if the database is successfully opened.
 * @return A dbstatus_t error code if the database fails to open.
 */
dbstatus_t db_open_index_storage(
    const char * storage_name,
    const db_database_config_t * config,
    const db_index_compare_keys_t * index_compare_func_array,
    size_t index_count);


#define DB_STATISTICS_QUERY     0u   /**< Flag to only query statistics. */
#define DB_STATISTICS_RESET     1u   /**< Flag to reset metrics after statistics are read. */

/**
 * @brief Statistics for an index storage.
 */
typedef struct db_index_storage_statistics_s {
    /** Elapsed time since reset in microseconds. */
    uint64_t elapsed_time;

    /** Total number of mtable entry pages. (does not reset) */
    size_t total_mtable_pages;
    /** Total number of data pages. (does not reset) */
    size_t total_data_pages;

    /** Maximum number of cached mtable entries, always less than or equal to @c total_data_pages. (does not reset) */
    size_t cached_mtable_entries_capacity;
    /** Maximum number of cached data pages, always less than or equal to @c total_data_pages. (does not reset) */
    size_t cached_data_pages_capacity;
    /** Number of cached mtable entries, always less than or equal to @c cached_mtable_entries_capacity. (does not reset) */
    size_t cached_mtable_entries;
    /** Number of cached data pages, always less than or equal to @c cached_data_pages_capacity. (does not reset) */
    size_t cached_data_pages;
    /** Number of data pages with unsaved changes, always less than or equal to @c cached_data_pages. (does not reset) */
    size_t dirty_data_pages;

    /** Number of page cache hits since reset. */
    size_t page_cache_hits;
    /** Number of page cache misses since reset. */
    size_t page_cache_misses;
    /** Number of page cache evictions since reset. */
    size_t page_cache_evictions;
    /** Number of mtable entry cache hits since reset. */
    size_t entry_cache_hits;
    /** Number of mtable entry cache misses since reset. */
    size_t entry_cache_misses;
    
    /* Media Disk I/O Metrics */
    /** Number of media read operations since reset. */
    size_t media_read_operations;
    /** Number of media write operations since reset. */
    size_t media_write_operations;
    /** Total bytes read from media since reset. */
    size_t media_bytes_read;
    /** Total bytes written to media since reset. */
    size_t media_bytes_written;
    /** Maximum read latency since reset in microseconds. */
    uint64_t media_max_read_latency;
    /** Maximum write latency since reset in microseconds. */
    uint64_t media_max_write_latency;
    /** Total time spent reading since reset in microseconds. */
    uint64_t media_total_read_time;
    /** Total time spent writing since reset in microseconds. */
    uint64_t media_total_write_time;
    
    /* Memory Footprint Metrics */
    /** Page cache memory usage in bytes. */
    size_t memory_page_cache;
    /** Mtable entries cache memory usage in bytes. */
    size_t memory_index_cache;
    /** Media buffer memory usage in bytes. */
    size_t memory_media_buffers;
    /** Media metadata memory usage in bytes. */
    size_t memory_media_metadata;
    /** Total storage RAM usage in bytes. */
    size_t memory_storage_ram;
    /** Stream Processing RAM usage in bytes. */
    size_t memory_processing_ram;
    /** Communication RAM usage in bytes. */
    size_t memory_network_ram;
    /** Total ITTIA DB Lite RAM usage in bytes. */
    size_t memory_total_ram;
    
    /* Table Performance Metrics */
    /** Number of query operations since reset. */
    size_t table_query_operations;
    /** Number of ingestion operations since reset. */
    size_t table_ingestion_operations;
    /** Total bytes processed in queries since reset. */
    size_t table_query_bytes_processed;
    /** Total bytes processed in ingestion since reset. */
    size_t table_ingestion_bytes_processed;
    /** Total time spent on queries in nanoseconds. */
    uint64_t table_query_total_time;
    /** Total time spent on ingestion in nanoseconds. */
    uint64_t table_ingestion_total_time;
    /** Current query latency in microseconds. */
    uint64_t table_query_latency;
    /** Current ingestion latency in microseconds. */
    uint64_t table_ingestion_latency;

} db_index_storage_statistics_t;

/**
 * @brief Capture statistics for index storage
 * 
 * @param db
 *   A local database connection.
 * @param flags 
 *   A bitwise combination of @c DB_STATISTICS_QUERY and/or @c DB_STATISTICS_RESET.
 * @param [out] stats 
 *   Destination for statistics (optional).
 *
 * @return dbstatus_t value DB_NOERROR when successful.
 */
dbstatus_t db_index_storage_statistics(db_t db, uint32_t flags, db_index_storage_statistics_t * stats);


dbstatus_t db_compare_values(uint8_t type,
              const void * a_ptr, db_len_t a_size, int a_prec,
              const void * b_ptr, db_len_t b_size, int b_prec,
              bool ignorecase);

dbstatus_t db_encode_datetime(uint8_t type, void * data, db_len_t data_size, const db_date_t * date, const db_time_t* time, const uint32_t microsec);

dbstatus_t db_decode_datetime(uint8_t type, const void * data, db_len_t data_size, db_date_t * date, db_time_t* time, uint32_t * microsec);


#ifdef __cplusplus
#define DB_RESULT_CONST(value) reinterpret_cast<db_result_t>(value)
#else
#define DB_RESULT_CONST(value) (db_result_t)(value)
#endif

/** API call result. @see DB_OK, DB_FAIL, Errors */
typedef struct rc_s *         db_result_t;

/** A result of success. No error code was set. @see Errors */
#define DB_OK                 DB_RESULT_CONST(1)

/** A result of failure for functions that return #db_result_t. An error code was set.
 * @see get_db_error, Errors. */
#define DB_FAIL               DB_RESULT_CONST(0)

/** @name Transaction Commit Mode
 *  @{ */
/** @ingroup CAPITransaction */
/** Default isolation mode. */
#define DB_DEFAULT_ISOLATION     0x00000000u
/** Read committed isolation. */
#define DB_READ_COMMITTED        0x00000010u
/** Repeatable read isolation. */
#define DB_REPEATABLE_READ       0x00000020u
/** Serializable isolation. */
#define DB_SERIALIZABLE          0x00000030u
/** Bitwise mask for isolation level flags. */
#define DB_ISOLATION_MASK        0x000000F0u
/** @} */

/** @name Transaction Lock Mode
 *  @{ */
/** @ingroup CAPITransaction */
/** Default lock mode. */
#define DB_LOCK_DEFAULT          0x00000000u
/** Obtain shared locks when possible. */
#define DB_LOCK_SHARED           0x00000001u
/** Obtain exclusive locks only. */
#define DB_LOCK_EXCLUSIVE        0x00000002u
/** Bitwise mask for lock type flags. */
#define DB_LOCK_MODE_MASK        0x00000003u
/** @} */

/** @name Transaction Type
 *  @{ */
/** @ingroup CAPITransaction */
/** Start a read-only tx. */
#define DB_TX_READ_ONLY          0x00000004u
/** Start a read/write tx. */
#define DB_TX_READ_WRITE         0x0000000Cu
/** Bitwise mask for transaction mode flags. */
#define DB_TX_MODE_MASK          0x0000000Cu
/** Default transaction flags. */
#define DB_TX_DEFAULT            DB_READ_ONLY_TX
/** @} */

/**
 * @brief Begin a transaction on the given database connection.
 * 
 * @param hdb
 *   A local database connection without an active transaction.
 * @param flags
 *   The transaction mode, either @c DB_TX_READ_ONLY or @c DB_TX_READ_WRITE.
 * @return DB_OK if the transaction starts successfully.
 * @return DB_FAIL if a transaction is already active.
 */
db_result_t db_begin_tx(db_t hdb, db_flags_t flags);

/**
 * @brief Commit the active transaction on the given database connection.
 * 
 * @param hdb
 *   A local database connection with an active transaction.
 * @param flags
 *   Transaction completion flags.
 * @return DB_OK if the transaction is committed successfully.
 * @return DB_FAIL if no transaction is active or the transaction cannot be committed.
 */
db_result_t db_commit_tx(db_t hdb, db_flags_t flags);

/**
 * @brief Abort the active transaction on the given database connection.
 * 
 * @param hdb
 *   A local database connection with an active transaction.
 * @param flags
 *   Transaction completion flags.
 * @return DB_OK if the transaction is aborted successfully, DB_FAIL otherwise.
 * 
 */
db_result_t db_abort_tx(db_t hdb, db_flags_t flags);


/** Return value when @c db_begin_transaction starts a transaction. */
#define DB_BEGIN_TRANSACTION_STARTED 1

/**
 * @brief Begin a database transaction.
 * @ingroup CAPITransaction
 * 
 * @param hdb
 *   A local database connection with or without an active transaction.
 * @param flags 
 *   The transaction mode, either @c DB_TX_READ_ONLY or @c DB_TX_READ_WRITE.
 * @return DB_BEGIN_TRANSACTION_STARTED if a transaction is successfully started.
 * @return DB_NOERROR if a compatible transaction is already active.
 * @return DB_ETXSTATE if a read-only transaction is active and @a flags is DB_TX_READ_WRITE.
 * @return DB_EINVAL if the @a hdb or @a flags are invalid.
 */
dbstatus_t db_begin_transaction(db_t hdb, db_flags_t flags);

/**
 * @brief Commit or rollback to complete the current transaction.
 * @ingroup CAPITransaction
 * 
 * @param hdb
 *   A local database connection with an active transaction.
 * @param status
 *   The transaction is committed only when @a status is a dbstatus_t code that
 *   indicates success.
 * @param flags
 *   Transaction completion flags.
 * @return DB_NOERROR if the transaction is successfully committed.
 * @return DB_ENOTX if no transaction is currently active.
 * @return dbstatus_t value equal to @a status if @a status indicates failure.
 */
dbstatus_t db_complete_transaction(db_t hdb, dbstatus_t status, db_flags_t flags);


C_HEADER_END

#endif // DB_INDEX_STORAGE_H
