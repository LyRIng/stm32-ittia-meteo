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
 * Entry point for the ITTIA IoT Storage C API. Part of the ITTIA DB Lite
 * Embedded C API.
 */

#ifndef DB_IOT_STORAGE_H
#define DB_IOT_STORAGE_H

#include <ittia/db/db_environment.h>
#include <ittia/db/db_coltype.h>

C_HEADER_BEGIN

/** Time series handle. @see CAPITimeSeries @ingroup CAPITimeSeries */
DB_DECLARE_HANDLE(db_time_series_t);
/** Time series query handle. @see CAPITimeSeries @ingroup CAPITimeSeries */
DB_DECLARE_HANDLE(db_time_series_query_t);

/**
 * @brief db_open_iot_storage opens or creates a database storage.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param file_name
 *   A string containing the name of file or storage media to open.
 * @param alias
 *   (optional) A string containing the alias name for connection and closed storage, or <tt>NULL</tt>.
 * @param flags
 *   Unsigned long integer that specifies the behavior when database does or
 *   does not already exist, and other flags.
 * @param page_size
 *   A signed integer that specifies the size of each database page.
 * @param storage_cache_segment
 *   A pointer to the base address of the database page cache memory segment.
 * @param storage_cache_size
 *   A integer that specifies the size of the database page cache memory segment, in bytes.
 * @param auth_info
 *   (optional) A pointer to a #db_auth_info_t structure that specifies the authorization credentials for a password-protected or encrypted database, or <tt>NULL</tt>.
 * 
 * @par Description
 * @parblock
 * The #db_open_iot_file_storage opens a database file. A large amount of memory is allocated for each open storage. 
 * The size of this allocation is controlled by the storage cache size argument. The memory is deallocated only when the storage is closed.
 * To open a database file named example.ittiadb, creating the file only if it does not already exist:
 * 
 * @code
 * dbstatus_t status;
 * status = db_open_iot_file_storage(
 *   "example.ittiadb",    // File name
 *   NULL,                 // Alias
 *   DB_OPEN_OR_CREATE,    // Flags
 *   DB_DEF_PAGE_SIZE,     // Page size
 *   NULL,                 // Storage cache memory segment
 *   128 * 1024);          // Storage cache size
 * @endcode
 * 
 * Replace #DB_OPEN_OR_CREATE with one of:
 * - #DB_CREATE_OR_OVERWRITE to overwrite an existing file, even if it is not a database file.
 * - #DB_OPEN_EXISTING to open the database only if it already exists.
 * - #DB_CREATE_NEW to create a new database only if the file does not already exist.
 * 
 * Other flags can be added to @a flags with the bitwise OR operator:
 * - #DB_READ_ONLY to disable API functions that modify the database contents.
 * - #DB_NOLOGGING to disable database recovery.
 * - #DB_VERIFY_CHECKSUMS to enable rigorous validation of page checksums.
 * - #DB_DEFERRED_PAGE_FLUSH to buffer time series data in memory until explicitly flushed with #db_flush_file.
 * - #DB_TIME_SERIES_FAST_RANGE to optimize time series range queries, such as
 *   #db_query_time_series_range_float32, at the cost of higher memory and storage
 *   consumption. #DB_TIME_SERIES_FAST_RANGE has no effect if the database file
 *   already exists.
 * 
 * The application can avoid this large memory allocation by supplying an aligned memory segment for the storage to use. 
 * A global or static array is appropriate for this purpose, since the memory must remain available until the storage is closed. 
 * Do not use an array on the C stack for the storage cache. 
 * For example, to supply a 128 KiB memory segment:
 * 
 * @code
 * static uint32_t page_cache[128 * 1024 / sizeof(uint32_t)];
 * dbstatus_t status;
 * status = db_open_iot_file_storage(
 *  "example.ittiadb",    // File name
 *  NULL,                 // Alias
 *  DB_OPEN_OR_CREATE,    // Flags
 *  DB_DEF_PAGE_SIZE,     // Page size
 *  page_cache,           // Page cache memory segment
 *  sizeof page_cache);   // Segment size
 * @endcode
 * 
 * @endparblock
 * 
 * @returns On Success #db_open_iot_file_storage returns the DB_NOERROR code.
 * @returns On failure, #db_open_iot_file_storage does nothing and returns one of the following error codes:
 * - #DB_EEXIST: File already exists
 * - #DB_ENOENT: File doesn’t exist
 * - #DB_EACCESS: File cannot be opened due to invalid access mode or insufficient file system privileges
 * - #DB_ESTORAGE: File exists, but is not recognized as a valid database storage
 * 
 * @see db_close_storage, <br>
 * DB_CREATE_OR_OVERWRITE, DB_OPEN_EXISTING, DB_CREATE_NEW
 */
dbstatus_t db_open_iot_file_storage(const char * file_name, const char * alias, uint32_t flags, int32_t page_size, void * storage_cache_segment, size_t storage_cache_size, db_auth_info_t * auth_info);

/** @addtogroup CAPIDatabase
 *  @{ */

/**
 * @brief db_close_storage closes all connections to a database storage and
 * releases all connection handles.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param name
 *  An optional string of the name or alias of the storage, or <tt>NULL</tt>.
 * 
 * @par Description
 * @parblock
 * The #db_close_storage closes a database file. A large amount of memory is allocated for each open storage. 
 * The size of this allocation is controlled by the storage cache size argument. The memory is deallocated only when the storage is closed.
 * To close a database file named example.ittiadb that was successfully opened or created by #db_open_iot_file_storage:
 * 
 * * @code
 * static uint32_t page_cache[128 * 1024 / sizeof(uint32_t)];
 * dbstatus_t status;
 * status = db_open_iot_file_storage(
 *  "example.ittiadb",    // File name
 *  NULL,                 // Alias
 *  DB_OPEN_OR_CREATE,    // Flags
 *  DB_DEF_PAGE_SIZE,     // Page size
 *  page_cache,           // Page cache memory segment
 *  sizeof page_cache);   // Segment size
 * 
 * if (DB_SUCCESS(status)) {
 *   // ...
 *  // Any thread may connect to the storage until it is closed
 *  // ...
 *  status = db_close_storage("example.ittiadb");
 * }
 * @endcode
 * 
 * When name is NULL, all databases instances currently opened are closed.
 * For example:
 * 
 * @code
 * dbstatus_t status;
 * status = db_close_storage(NULL);
 * @endcode
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 *
 * @see db_open_iot_file_storage
 */
dbstatus_t db_close_storage(const char * name);

/**
 * @brief db_connect opens a connection to an open database storage from a single thread.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param [out] handle
 *   Destination for database connection handle
 * @param name
 *   Name or alias of storage
 * @param user_name
 *   (optional) name of an authorized user
 * @param password
 *   (optional) password token
 * @param context
 *   (optional) User connection context
 * 
 * @par Description
 * @parblock
 * Each thread must connect to a database storage to access its contents through an opaque handle.
 * The handle assigned by db_connect must not be shared with other threads. 
 * If the database was opened with an alias, @a name must be an alias.
 * To connect to an open database named example.ittiadb:
 * 
 * @code
 * dbstatus_t status;
 * db_t db;
 * status = db_connect(
 * &db,                // Handle
 * "example.ittiadb",  // Name
 * NULL,               // User name
 * NULL,               // Password
 * NULL);              // Context pointer
 * @endcode
 * A database storage may require a user name and/or password to successfully connect. Otherwise, those arguments are optional.
 * @endparblock
 * When the handle is no longer needed, and after closing any other associated handles, call db_disconnect to release connection resources.
 * 
 * @returns On Success #db_connect returns the #DB_NOERROR code.
 * @returns On failure, #db_connect does nothing and returns one of the following error codes:
 * - #DB_ENOTFOUND: Storage name not found, needs to be opened with #db_open_iot_file_storage first.
 * - #DB_EPASSWORD: Access password is not valid or access is denied
 *
 * @see db_disconnect, <br>
 * db_open_iot_file_storage, Errors
 */
dbstatus_t db_connect(db_t * handle, const char * name, const char * user_name, const char * password, void * context);

/**
 * @brief db_get_connection_context returns the context pointer associated with
 * a database connection.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Handle to an ITTIA DB database connection
 * 
 * @par Description
 * @parblock
 * Assuming that you have successfully created a database using #db_open_iot_file_storage and accessed using #db_connect through a #db_t
 * @code
 * dbstatus_t status;
 * db_t db;
 * status = db_connect(
 * &db,                // Handle
 *  "example.ittiadb",  // Name
 *  NULL,               // User name
 *  NULL,               // Password
 *  "connection 1");    // Context pointer
 * if (DB_SUCCESS(status)) {
 * // ...
 * // Work with database contents through the `db` handle
 * // ...
 * }
 * @endcode
 * You can get the connection context pointer from the connection handle:
 * @code
 * void * context_pointer = db_get_connection_context(db);
 * // Prints "Connection name: connection 1"
 * printf("Connection name: %s\n", (const char *)context_pointer);
 * @endcode
 * @endparblock
 * 
 * @returns
 *   Context pointer that was originally supplied to db_connect
 * 
 * @see db_open_iot_file_storage, db_connect <br>
 */
void * db_get_connection_context(db_t handle);

/**
 * @brief db_disconnect closes a database connection.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Handle to an ITTIA DB Lite database connection
 * 
 * @par Description
 * @parblock
 * When the handle is no longer needed, and after closing any other associated handles, call #db_disconnect to release connection resources. 
 * The connection handle is an opaque pointer type that may be copied safely.
 * 
 * @code
 * dbstatus_t status;
 * db_t db;
 * status = db_connect(
 * &db,                // Handle
 *  "example.ittiadb",  // Name
 *  NULL,               // User name
 *  NULL,               // Password
 *  NULL);              // Context pointer
 * if (DB_SUCCESS(status)) {
 * // ...
 * // Work with database contents through the `db` handle
 * // ...
 *  status = db_disconnect(db);
 * }
 * @endcode
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 *
 * @see db_connect <br>
 */
dbstatus_t db_disconnect(db_t handle);

/**
 * @brief db_flush_file writes cached database changes to disk
 *  
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Handle to an ITTIA DB database connection
 * @param flags
 *   Flush mode and completion mode
 * 
 * @par Description
 * @parblock
 * After a timestamp/value pair is put, it is immediately available to queries from all database connections. 
 * However, to improve performance, it may remain buffered in memory until more data has accumulated.
 * To explicitly flush buffered data from memory to the database file, call db_flush_file from any connection:
 *
 * The default flags are sufficient to persist data. Example:
 * @code
 * // A handle previously assigned by db_connect
 * db_t db;
 * dbstatus_t flush_result = db_flush_file(db, DB_FLUSH_JOURNAL);
 * @endcode
 * Choose one flush mode flag:
 * - #DB_FLUSH_JOURNAL (default)
 * - #DB_FLUSH_STORAGE
 *
 * Choose one completion mode flag:
 * - #DB_GROUP_COMPLETION (default)
 * - #DB_FORCED_COMPLETION
 * @endparblock
 * 
 * @returns #DB_NOERROR on success, negative number on failure.
 * Example of a failure code:
 * - #DB_ENAME: Invalid name
 *
 * @see #db_connect, <br>
 * db_open_iot_file_storage, Errors
 */
dbstatus_t db_flush_file(db_t handle, db_flags_t flags);
/** @} */

/** @addtogroup CAPITimeSeries
 *  @{ */ 
/**
 * @brief db_set_max_timestamps defines the maximum number of timestamps to store 
 *     per time series column per table row.
 *  
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Handle to an ITTIA DB database connection
 * @param max_count
 *   Defines the the maximum number of timestamps to store.
 * @param pages_per_remove
 *   Number of pages to remove each ingestion
 * 
 * * @par Description
 * @parblock
 * Before ingesting data into a time series column, an upper limit can be
 * imposed on the number of timestamps at which a value can be stored.
 * Here is an example:
 *
 * @code
 * dbstatus_t status;
 * // Previously connected to a DB handle successfully
 * db_t db;
 * int32_t max_count = 2000; 
 * int32_t pages_per_remove = 10000;
 * status = db_set_max_timestamps(db, max_count, pages_per_remove);
 * if (DB_SUCCESS(status)) {
 *  // ...
 *  // New max count applied successfully to the `db` handle
 *  // ..
 *     status = db_disconnect(db);
 * }
 * @endcode
 * @endparblock
 * 
 * @returns DB_NOERROR on success
 * @returns a negative number on failure
 * 
 * @see db_set_time_series_data_retention <br>
 */
dbstatus_t db_set_max_timestamps(db_t handle, int32_t max_count, int32_t pages_per_remove);
/** @} */

/** @addtogroup CAPIDatabase
 *  @{ */
/**
 * @brief The db_file_info_t struct describes information about a database
 * file storage.
 */
typedef struct db_file_info_s {
    int64_t data_file_bytes;  ///< Size of the the data file(s), in bytes
    int64_t log_file_bytes;   ///< Size of the current log file, in bytes
    int64_t used_data_bytes;  ///< Number of used bytes in the data file(s)
    int64_t time_series_data_bytes;  ///< Size of time series data, excluding overhead, in bytes
} db_file_info_t;

/**
 * @brief db_file_info obtains information about a database file storage.
 *  
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Handle to an ITTIA DB database connection
 * @param [out] info
 *   Destination for file information
 * 
 * @par Description
 * @parblock
 * To determine the amount of storage currently consumed, use #db_file_info:
 * 
 * @code
 * int64_t data_file_bytes;  ///< Size of the the data file(s), in bytes
 * int64_t log_file_bytes;   ///< Size of the current log file, in bytes
 * int64_t used_data_bytes;  ///< Number of used bytes in the data file(s)
 * db_file_info_t info;
 * if (DB_SUCCESS(db_file_info(db, &info))) {
 *     printf("Using %lld of %lld data bytes\n",
 *            info.used_data_bytes,
 *            info.data_file_bytes);
 * 
 *     printf("Log file is %lld bytes\n",
 *            info.log_file_bytes);
 * }
 * @endcode
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR on success, negative number on failure
 */
dbstatus_t db_file_info(db_t handle, db_file_info_t * info);
/** @} */

/** @addtogroup CAPITimeSeries
 *  @{ */ 
/**
 * @brief Set data retention policy for time series data.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Handle to an ITTIA DB database connection
 * @param retain_data_bytes
 *   Number of bytes of time series data to retain
 * @param max_data_bytes
 *   Maximum number of bytes to allocate for time series data
 * 
 * @par Description
 * @parblock
 * After creating and/or ingesting data into a time series. You may want to set a retention policy and put a cap on the size of timeseries.
 * Here is an example:
 *
 * @code
 * dbstatus_t status;
 * // Previously connected to a DB handle successfully
 * db_t db;
 * int64_t retain_data_bytes = 20000; 
 * int64_t max_data_bytes = 100000;
 * status = db_set_time_series_data_retention(db, retain_data_bytes, max_data_bytes);
 * if (DB_SUCCESS(status)) {
 *  // ...
 *  // New Retention policy applied successfully to the `db` handle
 *  // ..
 *     status = db_disconnect(db);
 * }
 * @endcode
 *
 * Wrap around performance is influenced by the difference between
 * @a max_data_bytes and @a retain_data_bytes. @a max_data_bytes must be at
 * least one database page larger than @a retain_data_bytes.
 * @endparblock
 * 
 * @return
 *   DB_NOERROR on success, negative number on failure
 *
 * @see db_connect,  <br>
 * db_open_iot_file_storage, Errors
 */
dbstatus_t db_set_time_series_data_retention(db_t handle, int64_t retain_data_bytes, int64_t max_data_bytes);

/**
 * @brief Convert timestamp data structure to microseconds timestamp
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param [out] dest
 *   Timestamp in microseconds since epoch
 * @param src
 *   Timestamp as a data structure
 * 
 * @par Description
 * @parblock
 * A timestamp is represented by the number of microseconds elapsed since the UNIX epoch, 1970-01-01 00:00:00 UTC, in the 64-bit signed integer #type db_timestamp_usec_t. 
 * To convert a timestamp from individual date and time components a microsecond timestamp, use the #db_timestamp_to_usec function.
 * To store a 32-bit floating point value at any timestamp, call #db_time_series_put_float32. 
 * For best performance, values should be ingested in increasing timestamp order.
 * If a value already exists in the series at the given timestamp, it is replaced.
 * 
 * @code
 * // A handle previously assigned by calling db_connect
 * db_t db;
 * dbstatus_t status;
 * db_time_series_t series;
 * // Previously opened timeseries
 * status = db_open_time_series(&series, db, "temperature", DB_COLTYPE_FLOAT32);
 * if (DB_SUCCESS(status)) {
 * static const db_timestamp_t kStartTime = { { { 2020, 12, 1 }, { 0, 0, 0 } }, 0 };
 * static const db_float32_t value = 123.4;
 * 
 * db_timestamp_usec_t sample_time_usec;
 * db_timestamp_to_usec(&sample_time_usec, kStartTime);
 * 
 * // Put 123.4 into "temperature" at "2020-12-01 00:00:00"
 *     status = db_time_series_put_float32(series, sample_time_usec, value);
 *     if (DB_FAILED(status)) {
 *         fprintf(stderr, "Put timestamp/value failed: %d\n", status);
 *     }
 * }
 * 
 * (void)db_close_time_series(series);
 * @endcode
 * @endparblock
 * 
 * An invalid @a src timestamp is converted to DB_INVALID_TIMESTAMP_USEC.
 * @see db_time_series_put_float32 <br>
 */
void db_timestamp_to_usec(db_timestamp_usec_t * dest, const db_timestamp_t * src);

/**
 * @brief Convert microseconds timestamp to timestamp data structure.
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param [out] dest
 *   Timestamp as a data structure
 * @param src
 *   Timestamp in microseconds since epoch
 * 
 * @par Description
 * @parblock
 * A timestamp is represented by the number of microseconds elapsed since the UNIX epoch, 1970-01-01 00:00:00 UTC, in the 64-bit signed integer #type db_timestamp_usec_t. 
 * To convert a timestamp from individual date and time components a microsecond timestamp, use the #db_timestamp_to_usec function.
 * To store a 32-bit floating point value at any timestamp, call #db_time_series_put_float32. 
 * For best performance, values should be ingested in increasing timestamp order.
 * If a value already exists in the series at the given timestamp, it is replaced.
 * 
 * @code
 * // A handle previously assigned by calling db_connect
 * static const db_timestamp_t kStartTime = { { { 2020, 12, 1 }, { 0, 0, 0 } }, 0 };
 * static const db_float32_t value = 123.4;
 * 
 * db_timestamp_usec_t sample_time_usec;
 * db_timestamp_to_usec(&sample_time_usec, kStartTime);
 * db_timestamp_from_usec(&kStartTime, sample_time_usec);
 * @endcode
 * An invalid @a src timestamp is converted to 0000-00-00 00:00:00.
 * @endparblock
 * 
 * @see db_timestamp_to_usec <br>
 * 
 */
void db_timestamp_from_usec(db_timestamp_t * dest, db_timestamp_usec_t src);
/** @} */

/** @addtogroup CAPITimeSeries
 *  @{ */ 
/**
 * @brief Open a named series of timestamp/value pairs
 *  
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param [out] series_handle
 *   Reference to the time series
 * @param db_handle
 *   Handle to an ITTIA DB database connection
 * @param name
 *   Name of the time series
 * @param type
 *   Type of data to store in the time series
 * 
 * @par Description
 * @parblock
 * The time series is uniquely identified in the database by its name. A new
 * time series is added to the database if one does not already exist with the
 * given name.
 *
 * After the time series is opened successfully, a handle is allocated and
 * assigned as output. Pass this handle to other functions that need to operate
 * on the time series. When access to the time series is no longer needed, call
 * #db_close_time_series to release the handle.
 * 
 * @code
 * // A handle previously assigned by calling db_connect
 * db_t db;
 * 
 * dbstatus_t status;
 * db_time_series_t series;
 * status = db_open_time_series(&series, db, "temperature", DB_COLTYPE_FLOAT32);
 * @endcode
 * Before ingesting timestamped values, the time series must first be opened by name with #db_open_time_series to obtain a series handle. 
 * This handle must not be shared between threads.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR when successful, negative number on failure
 *
 * @see db_close_time_series, <br>
 * db_connect, Errors
 */
dbstatus_t db_open_time_series(db_time_series_t * series_handle, db_t db_handle, const db_ansi_t * name, db_coltype_t type);

/**
 * @brief Invalidate a time series handle and release associated resources
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Reference to the time series
 * 
 * @par Description
 * @parblock
 *
 * After the time series is opened successfully, a handle is allocated and
 * assigned as output. Pass this handle to other functions that need to operate
 * on the time series. When access to the time series is no longer needed, call
 * #db_close_time_series to release the handle.
 * 
 * @code
 * // A handle previously assigned by calling db_connect
 * db_t db;
 * 
 * dbstatus_t status;
 * db_time_series_t series;
 * // Previously opened timeseries
 * status = db_open_time_series(&series, db, "temperature", DB_COLTYPE_FLOAT32);
 * if (DB_SUCCESS(status)) {
 *  //Apply or ingest some data into series
 * }
 * 
 * (void)db_close_time_series(series);
 * @endcode
 * 
 * A limited number of time series can be open at once. A time series handle
 * must be closed with when it is no longer needed.
 *
 * The handle is invalidated and must not be used after calling this function.
 * @endparblock
 * 
 * @returns
 *   DB_NOERROR when successful, negative number on failure
 *
 * @see db_open_time_series <br>
 */
dbstatus_t db_close_time_series(db_time_series_t handle);

/**
 * @brief Put a timestamp/value pair into a time series
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Reference to the time series
 * @param usec
 *   Microseconds timestamp
 * @param value
 *   Value data
 * 
 * @par Description
 * @parblock
 * After the time series is opened successfully, a handle is allocated and
 * assigned as output. Pass this handle to other functions that need to operate
 * on the time series. 
 * To store a 32-bit floating point value at any timestamp, call #db_time_series_put_float32. 
 * For best performance, values should be ingested in increasing timestamp order.
 * If a value already exists in the series at the given timestamp, it is replaced.
 * A timestamp is represented by the number of microseconds elapsed since the UNIX epoch, 1970-01-01 00:00:00 UTC, in the 64-bit signed integer #type db_timestamp_usec_t. 
 * To convert a timestamp from individual date and time components a microsecond timestamp, use the #db_timestamp_to_usec function.
 * 
 * @code
 * // A handle previously assigned by calling db_connect
 * db_t db;
 * dbstatus_t status;
 * db_time_series_t series;
 * // Previously opened timeseries
 * status = db_open_time_series(&series, db, "temperature", DB_COLTYPE_FLOAT32);
 * if (DB_SUCCESS(status)) {
 * static const db_timestamp_t kStartTime = { { { 2020, 12, 1 }, { 0, 0, 0 } }, 0 };
 * static const db_float32_t value = 123.4;
 * 
 * db_timestamp_usec_t sample_time_usec;
 * db_timestamp_to_usec(&sample_time_usec, kStartTime);
 * 
 * // Put 123.4 into "temperature" at "2020-12-01 00:00:00"
 *     status = db_time_series_put_float32(series, sample_time_usec, value);
 *     if (DB_FAILED(status)) {
 *         fprintf(stderr, "Put timestamp/value failed: %d\n", status);
 *     }
 * }
 * 
 * (void)db_close_time_series(series);
 * @endcode
 * 
 * An application can call db_time_series_put_* any number of times before closing the time series.
 * After a timestamp/value pair is put, it is immediately available to queries from all database connections. 
 * However, to improve performance, it may remain buffered in memory until more data has accumulated.
 * @endparblock
 * 
 * @returns DB_NOERROR when successful, negative number on failure.
 * @returns DB_EDATA when @a usec is an invalid timestamp.
 *
 * @see db_open_time_series,<br>
 * db_close_time_series, db_timestamp_to_usec, Errors
 */
dbstatus_t db_time_series_put_float32(db_time_series_t handle, db_timestamp_usec_t usec, float32_t value);

/**
 * @brief Read the greatest timestamp and its value from a time series
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Reference to the time series
 * @param [out] timestamp_dest
 *   Destination for microseconds timestamp
 * @param [out] value_dest
 *   Destination for value data
 * 
 * @par Description
 * @parblock
 * After the time series is opened successfully, a handle is allocated and
 * assigned as output. Pass this handle to other functions that need to operate
 * on the time series. 
 * To read a 32-bit floating point value at any timestamp, call #db_time_series_get_float32. 
 * 
 * * @code
 * // A handle previously assigned by calling db_connect
 * db_t db;
 * dbstatus_t status;
 * db_time_series_t series;
 * // Previously opened timeseries
 * status = db_open_time_series(&series, db, "temperature", DB_COLTYPE_FLOAT32);
 * if (DB_SUCCESS(status)) {
 * static const db_timestamp_t kStartTime = { { { 2020, 12, 1 }, { 0, 0, 0 } }, 0 };
 * static const db_float32_t value;
 * 
 * db_timestamp_usec_t sample_time_usec;
 * db_timestamp_to_usec(&sample_time_usec, kStartTime);
 * 
 * // Read value from "temperature" at "2020-12-01 00:00:00"
 *     status = db_time_series_get_float32(series, &sample_time_usec, &value);
 *     if (DB_FAILED(status)) {
 *         fprintf(stderr, "Get timestamp/value failed: %d\n", status);
 *     }
 * }
 * 
 * (void)db_close_time_series(series);
 * @endcode
 * @endparblock
 * 
 * @returns DB_NOERROR when successful, negative number on failure.
 * @returns DB_ENOTFOUND when the time series is empty.
 *
 * @see db_time_series_put_float32,<br>
 * db_open_time_series, db_close_time_series, db_timestamp_to_usec, Errors
 */
dbstatus_t db_time_series_get_float32(db_time_series_t handle, db_timestamp_usec_t * timestamp_dest, float32_t * value_dest);

/**
 * @brief Get the number of unique timestamp/value pairs in a time series
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Reference to the time series
 * @param [out] cardinality_dest
 *   Destination for the number of unique timestamp/value pairs in the series
 * 
 * @par Description
 * @parblock
 * After the time series is opened successfully, a handle is allocated and
 * assigned as output. Pass this handle to other functions that need to operate
 * on the time series. 
 * To get the current cardinality of the series, call #db_time_series_get_cardinality. 
 * 
 * @code
 * // A handle previously assigned by calling db_connect
 * db_t db;
 * dbstatus_t status;
 * db_time_series_t series;
 * // Previously opened timeseries
 * status = db_open_time_series(&series, db, "temperature", DB_COLTYPE_FLOAT32);
 * if (DB_SUCCESS(status)) {
 * static const int64_t * cardinality_dest;
 * status = db_time_series_get_cardinality(series, &cardinality_dest);
 *     if (DB_FAILED(status)) {
 *         fprintf(stderr, "Get Cardinality failed: %d\n", status);
 *     }
 * }
 * 
 * (void)db_close_time_series(series);
 * @endcode
 * 
 * @endparblock
 * 
 * @returns DB_NOERROR when successful, negative number on failure.
 * @returns DB_ENOTFOUND when the time series is empty.
 * 
 * @see db_open_time_series, <br>
 * db_close_time_series, db_timestamp_to_usec, Errors
 */
dbstatus_t db_time_series_get_cardinality(db_time_series_t handle, int64_t * cardinality_dest);

/**
 * @brief Remove all the records that have a timestamp that's strictly older than the specified timestamp from the database using its handle
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Handle to an ITTIA DB database connection
 * @param usec
 *   Lowest timestamp to keep
 * 
 * @par Description
 * @parblock
 * After the time series is opened successfully, a handle is allocated and
 * assigned as output. Pass this handle to other functions that need to operate
 * on the time series. 
 * To remove old values from a series the series, call #db_time_series_remove_before: 
 * 
 * @code
 * // A handle previously assigned by calling db_connect
 * db_t db;
 * dbstatus_t status;
 * db_time_series_t series;
 * // Previously opened timeseries
 * status = db_open_time_series(&series, db, "temperature", DB_COLTYPE_FLOAT32);
 * if (DB_SUCCESS(status)) {
 * * static const db_timestamp_t kStartTime = { { { 2020, 12, 1 }, { 0, 0, 0 } }, 0 };
 * static const db_float32_t value;
 * 
 * db_timestamp_usec_t sample_time_usec;
 * db_timestamp_to_usec(&sample_time_usec, kStartTime);
 * 
 * // Remove values from "temperature" series before "2020-12-01 00:00:00"
 * status = db_time_series_remove_before(series, &sample_time_usec);
 *     if (DB_FAILED(status)) {
 *         fprintf(stderr, "Remove before failed: %d\n", status);
 *     }
 * }
 * 
 * (void)db_close_time_series(series);
 * @endcode
 * Remove all timestamp/value pairs strictly older than @a usec. Any values stored at precisely
 * @a usec are not removed.
 * @endparblock
 * 
 * @returns
 *   Number of distinct timestamps removed when successful, negative number on failure
 *
 * @see db_open_time_series, <br>
 * db_close_time_series, db_timestamp_to_usec, Errors
 */
dbstatus_t db_time_series_remove_before(db_t handle, db_timestamp_usec_t usec);

/** @name Time Series Query Direction
 *  @{ */
/** Sort time series query results oldest first. */
#define DB_QUERY_OLDEST_TO_NEWEST   0x00000000u
/** Sort time series query results newest first. */
#define DB_QUERY_NEWEST_TO_OLDEST   0x00000001u
/** @} */

/**
 * @brief Query a time range in a single time series
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle 
 *   Reference to a prepared time series query
 * @param begin_time
 *   Start timestamp, inclusive
 * @param end_time
 *   End timestamp, exclusive
 * @param [out] timestamp_dest
 *   Destination for timestamp data
 * @param [out] value_dest
 *   Destination for value data
 * @param max_read
 *   Maximum number of timestamp/value pairs to read at once
 * @param [out] next_begin_time
 *   Start timestamp for next batch, if successful
 * 
 * @par Description
 * @parblock
 * An application can query a time series to access the timestamp/value pairs stored within any time period. 
 * A time period is defined by an start timestamp, which is included in the query results, and an end timestamp that is excluded from the results.
 * The db_query_time_series_range_float32 function reads a series of timestamp/value pairs into corresponding arrays supplied by the application. 
 * Both arrays must have the same size, with at least one element each.
 * The following example reads all timestamp/value pairs, 100 at a time, stored in the temperature time series on 2020-12-01 from minute one to minute two after midnight.
 * 
 * @code
 * // Query values on 2020-12-01 from 00:01:00 to 00:02:00
 * static const db_timestamp_t kStartTime = { { { 2020, 12, 1 }, { 0, 1, 0 } }, 0 };
 * static const db_timestamp_t kStopTime = { { { 2020, 12, 1 }, { 0, 2, 0 } }, 0 };
 * dbstatus_t status;
 * db_time_series_t series;
 * status = db_open_time_series(&series, db, "temperature", DB_COLTYPE_FLOAT32);
 * if (DB_SUCCESS(status)) {
 *     dbstatus_t value_count;
 *     db_timestamp_usec_t begin_time;
 *     db_timestamp_usec_t end_time;
 *     db_timestamp_to_usec(&begin_time, kStartTime);
 *     db_timestamp_to_usec(&end_time, kStopTime);
 * 
 *     do {
 *         db_timestamp_usec_t timestamp_array[kMaxReadCount];
 *         db_float32_t value_array[kMaxReadCount];
 * 
 *         // Read timestamp/value pairs between begin_time and end_time
 *         db_timestamp_usec_t next_begin_time;
 *         value_count = db_query_time_series_range_float32(
 *             series,
 *             begin_time,      // start timestamp, inclusive
 *             end_time,        // end timestamp, exclusive
 *             timestamp_array, // [out] destination for timestamp data
 *             value_array,     // [out] destination for value data
 *             kMaxReadCount,   // maximum number of timestamp/value pairs to read
 *             &next_begin_time // [out] start timestamp for next batch, if successful
 *             );
 * 
 *         // Print timestamp/value pairs
 *         for (int i = 0; i < value_count; ++i) {
 *             // see function definition below
 *             print_timestamp_value_pair(
 *                 "temperature",
 *                 timestamp_array[i],
 *                 value_array[i]);
 *         }
 * 
 *         // Iterate to read next batch of timestamp/value pairs
 *         begin_time = next_begin_time;
 *     } while (value_count == kMaxReadCount);
 * 
 *     (void)db_close_time_series(series);
 * }
 * @endcode
 * 
 *  Reads up to @a max_read timestamp/value pairs from the series into the
 * arrays @a timestamp_dest and @a value_dest. Timestamps and values are sorted
 * in ascending order by time.
 *
 * If successful, the number of values read is returned and @a next_begin_time
 * is assigned a value that is greater than any returned timestamp, but less
 * than or equal to @a end_time. In the next call to this function, replace
 * the @a begin_time argument with *next_begin_time from the previous call to
 * and repeat until a number less than @a max_read is returned. This will read
 * all timestamp/value pairs in the original time range.
 *
 * Pairs are read greater or equal to the @a begin_time and strictly less than
 * the @a end_time. If no timestamp/value pairs exist in that interval, 0 is
 * returned.
 * @endparblock
 * 
 * @returns
 *   number of timestamp/value pairs read when successful, negative number on
 *   failure
 *
 * @see db_open_time_series, <br>
 * db_close_time_series, db_timestamp_to_usec, Errors
 */
dbstatus_t db_query_time_series_range_float32(db_time_series_t handle, db_timestamp_usec_t begin_time, db_timestamp_usec_t end_time, db_timestamp_usec_t timestamp_dest[], float32_t value_dest[], db_len_t max_read, db_timestamp_usec_t * next_begin_time);

/**
 * @brief Query a time range in any number of time series
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param [out] handle
 *   Reference to query state
 * @param db
 *   Handle to an ITTIA DB database connection
 * @param [in,out] series_name_list
 *   (optional) Array of series names to include in query result
 * @param series_count
 *   Number of names in @a series_name_list
 * @param begin_time
 *   (optional) Start timestamp, inclusive
 * @param end_time
 *   (optional) End timestamp, exclusive
 * @param flags
 *   Query flags
 * 
 * @par Description
 * @parblock
 * An application can query many or all time series at once, retrieving all values stored within any time period.
 * The following example reads all timestamp/value pairs, up to 100 at a time, stored in any time series on 2020-12-01 from minute one to minute two after midnight.
 * 
 * @code
 *  // Query values on 2020-12-01 from 00:01:00 to 00:02:00
 *  static const db_timestamp_t kStartTime = { { { 2020, 12, 1 }, { 0, 1, 0 } }, 0 };
 * static const db_timestamp_t kStopTime = { { { 2020, 12, 1 }, { 0, 2, 0 } }, 0 };
 * 
 * db_timestamp_usec_t begin_time;
 * db_timestamp_usec_t end_time;
 * db_timestamp_to_usec(&begin_time, kStartTime);
 * db_timestamp_to_usec(&end_time, kStopTime);
 * // Query the set of time series with values stored in a given range
 * db_time_series_query_t series_set;
 * dbstatus_t time_series_count = db_prepare_time_series_query(
 *     &series_set,       // [out] query handle
 *     db,                // database handle
 *     NULL,              // series name list
 *     0,                 // series count
 *     &begin_time,       // start timestamp, inclusive
 *     &end_time,         // end timestamp, exclusive
 *     DB_QUERY_OLDEST_TO_NEWEST
 *     );
 * 
 * dbstatus_t value_count;
 * do {
 *     db_timestamp_usec_t sample_time;
 *     db_fieldno_t field_array[kMaxReadCount];
 *     db_float32_t value_array[kMaxReadCount];
 * 
 *     // Read from the set, one timestamp at a time
 *     value_count = db_fetch_next_timestamp_float32(
 *         series_set,     // query handle
 *         &sample_time,   // [out] next sample time
 *         field_array,    // [out] destination for list of time series fields
 *         value_array,    // [out] destination for list of time series values
 *         kMaxReadCount
 *         );
 * 
 *     // Print each value found at the timestamp
 *     for (int i = 0; i < value_count; ++i) {
 *         print_timestamp_value_pair(
 *             db_get_time_series_name(series_set, field_array[i]),
 *             sample_time,
 *             value_array[i]);
 *     }
 * 
 * } while (value_count > 0);
 * 
 * if (DB_FAILED(value_count)) {
 *     fprintf(stderr, "Query failed: %d\n", status);
 * }
 * 
 * (void)db_close_time_series_query(series_set);
 * @endcode
 * 
 * If @a series_name_list is NULL, the query result will include all time
 * series with values in the given time range.
 *
 * If @a begin_time or @a end_time is NULL, the query result will not have a
 * lower or upper bound, respectively.
 *
 * Query flags accepted:
 *
 * - #DB_QUERY_OLDEST_TO_NEWEST: sort results from oldest to newest timestamp
 * - #DB_QUERY_NEWEST_TO_OLDEST: sort results from newest to oldest timestamp
 * @endparblock
 * 
 * @returns
 *   The actual number of time series in the query result, or a negative number
 *   on failure
 *
 * @see db_query_time_series_range_float32 <br>
 */
dbstatus_t db_prepare_time_series_query(db_time_series_query_t * handle, db_t db, const char* series_name_list[], db_len_t series_count, db_timestamp_usec_t * begin_time, db_timestamp_usec_t * end_time, db_flags_t flags);

/**
 * @brief Count distinct timestamps in a time series query
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Reference to a prepared time series query
 * @param [out] read_timestamp_count
 *   Number of distinct timestamps read from the query so far.
 * @param [out] total_timestamp_estimate
 *   Estimate of the total number of distinct timestamps in the query
 * 
 * @par Description
 * @parblock
 * Immediately after preparing a query, the @a read_timestamp_count starts at
 * zero, then increments each time #db_fetch_next_timestamp_float32 returns with
 * a different timestamp. If @a read_timestamp_count is NULL, the count is not
 * assigned.
 *
 * The @a total_timestamp_estimate is assigned the approximate number of
 * distinct timestamp values in the time range specified when the query
 * was prepared. The accuracy of the estimate will generally improve after
 * part of the query has been read, and will be equal to @a read_timestamp_count
 * after the entire query result has been read.
 *
 * If @a total_timestamp_estimate is <tt> NULL</tt>, the estimate is not calculated
 * or assigned. Calculating the estimate is comparable in cost to
 * db_fetch_next_timestamp_float32.
 * 
 * @endparblock
 * @returns
 *   Negative number on failure
 *
 * @see db_prepare_time_series_query <br>
 */
dbstatus_t db_count_timestamps(db_time_series_query_t handle, int32_t * read_timestamp_count, int32_t * total_timestamp_estimate);

/**
 * @brief Invalidate a time series query handle and release associated resources
 *  
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Reference to a prepared time series query
 * 
 * @par Description
 * @parblock
 * After using #db_prepare_time_series_query to prepare a query set using #db_fetch_next_timestamp_float32 to read the query answer,
 * the query must be closed by #db_close_time_series_query to release resources. Example:
 * 
 * @code
 * // Prepared Query set using db_prepare_time_series_query
 * //series_set
 * dbstatus_t value_count;
 * do {
 *     db_timestamp_usec_t sample_time;
 *     db_fieldno_t field_array[kMaxReadCount];
 *     db_float32_t value_array[kMaxReadCount];
 * 
 *     // Read from the set, one timestamp at a time
 *     value_count = db_fetch_next_timestamp_float32(
 *         series_set,     // query handle
 *         &sample_time,   // [out] next sample time
 *         field_array,    // [out] destination for list of time series fields
 *         value_array,    // [out] destination for list of time series values
 *         kMaxReadCount
 *         );
 * 
 *     // Print each value found at the timestamp
 *     for (int i = 0; i < value_count; ++i) {
 *         print_timestamp_value_pair(
 *             db_get_time_series_name(series_set, field_array[i]),
 *             sample_time,
 *             value_array[i]);
 *     }
 * 
 * } while (value_count > 0);
 * (void)db_close_time_series_query(series_set);
 * @endcode
 * @endparblock
 * 
 * @returns
 *   Negative number on failure
 *
 *  @see db_prepare_time_series_query
 */
dbstatus_t db_close_time_series_query(db_time_series_query_t handle);

/**
 * @brief Get the name of a time series in a query results
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Reference to a prepared time series query
 * @param fieldno
 *   Query column number
 * 
 * @par Description
 * @parblock
 * After using #db_prepare_time_series_query to prepare a query set using #db_fetch_next_timestamp_float32 to read the query answer,
 * the query must be closed by #db_close_time_series_query to release resources. Example:
 * 
 * @code
 * // Prepared Query set using db_prepare_time_series_query
 * //series_set
 * dbstatus_t value_count;
 * do {
 *     db_timestamp_usec_t sample_time;
 *     db_fieldno_t field_array[kMaxReadCount];
 *     db_float32_t value_array[kMaxReadCount];
 * 
 *     // Read from the set, one timestamp at a time
 *     value_count = db_fetch_next_timestamp_float32(
 *         series_set,     // query handle
 *         &sample_time,   // [out] next sample time
 *         field_array,    // [out] destination for list of time series fields
 *         value_array,    // [out] destination for list of time series values
 *         kMaxReadCount
 *         );
 * 
 *     // Print each value found at the timestamp
 *     for (int i = 0; i < value_count; ++i) {
 *         print_timestamp_value_pair(
 *             db_get_time_series_name(series_set, field_array[i]),
 *             sample_time,
 *             value_array[i]);
 *     }
 * 
 * } while (value_count > 0);
 * (void)db_close_time_series_query(series_set);
 * @endcode
 * 
 * The @a fieldno must be strictly less than the number of series in the query,
 * returned by #db_prepare_time_series_query.
 *
 * The pointer returned by `db_get_time_series_name` is only valid until the
 * next use of the same database connection. Either call this function each
 * time the name of a time series is needed, or copy the string data before
 * calling this function again.
 * @endparblock
 * 
 * @returns
 *   Name of the time series in the specified column
 *
 * @see db_prepare_time_series_query <br>
 */
const char * db_get_time_series_name(db_time_series_query_t handle, db_fieldno_t fieldno);

/**
 * @brief Fetch values for the next timestamp in a query results
 * 
 * @par Headers
 * @code {.c}
 * #include <ittia/db/db_iot_storage.h>
 * @endcode
 *
 * @param handle
 *   Reference to a prepared time series query
 * @param [out] timestamp
 *   Destination for timestamp
 * @param [out] fieldno_list
 *   Array of field numbers
 * @param [out] value_list
 *   Array of values
 * @param list_size
 *   Size of @a field_list and @a value_list
 * 
 * @par Description
 * @parblock
 * After using #db_prepare_time_series_query to prepare a query set. Call this function
 * repeatedly to read all query results. Example:
 * 
 * @code
 * // Prepared Query set using db_prepare_time_series_query
 * //series_set
 * dbstatus_t value_count;
 * do {
 *     db_timestamp_usec_t sample_time;
 *     db_fieldno_t field_array[kMaxReadCount];
 *     db_float32_t value_array[kMaxReadCount];
 * 
 *     // Read from the set, one timestamp at a time
 *     value_count = db_fetch_next_timestamp_float32(
 *         series_set,     // query handle
 *         &sample_time,   // [out] next sample time
 *         field_array,    // [out] destination for list of time series fields
 *         value_array,    // [out] destination for list of time series values
 *         kMaxReadCount
 *         );
 * 
 *     // Print each value found at the timestamp
 *     for (int i = 0; i < value_count; ++i) {
 *         print_timestamp_value_pair(
 *             db_get_time_series_name(series_set, field_array[i]),
 *             sample_time,
 *             value_array[i]);
 *     }
 * 
 * } while (value_count > 0);
 * (void)db_close_time_series_query(series_set);
 * @endcode
 * 
 * The next timestamp is assigned to @a timestamp. An array of fieldno and
 * value pairs are assigned to @a fieldno_list and @a value_list. If a time
 * series does not have a value at the timestamp, it is not included in these
 * lists. The lists are sorted in fieldno order.
 *
 * This function reads up to @a list_size values for the next timestamp. If
 * more values are available, the next call to this function will continue to
 * read from the same timestamp.
 * @endparblock
 * 
 * @returns
 *   Number of fieldno/value pairs read, or a negative number on failure
 *
 *@see db_prepare_time_series_query <br>
 */
dbstatus_t db_fetch_next_timestamp_float32(db_time_series_query_t handle, db_timestamp_usec_t * timestamp, db_fieldno_t fieldno_list[], float32_t value_list[], db_len_t list_size);
/** @} */

C_HEADER_END

#endif // DB_IOT_STORAGE_H
