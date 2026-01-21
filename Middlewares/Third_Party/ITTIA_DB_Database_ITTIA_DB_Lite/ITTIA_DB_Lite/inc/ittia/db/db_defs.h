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

/** @file db_defs.h
 *
 * Constant defines.
 */

#ifndef DB_DEFS_H
#define DB_DEFS_H

/** Minimum page size.
 * @see StorageMode::page_size, db_file_storage_config_t::page_size */
#define DB_MIN_PAGE_SIZE 512
/** Maximum page size.
 * @see StorageMode::page_size, db_file_storage_config_t::page_size */
#define DB_MAX_PAGE_SIZE 32768
/** Default page size.
 * @see StorageMode::page_size, db_file_storage_config_t::page_size */
#define DB_DEF_PAGE_SIZE 4096

#ifndef DB_MAX_OBJECT_NAME
#   define DB_MAX_OBJECT_NAME   128
#endif
#define DB_MAX_SEQUENCE_NAME    DB_MAX_OBJECT_NAME
#define DB_MAX_TABLE_NAME       DB_MAX_OBJECT_NAME
#define DB_MAX_INDEX_NAME       DB_MAX_OBJECT_NAME
#define DB_MAX_FIELD_NAME       DB_MAX_OBJECT_NAME
#define DB_MAX_SAVEPOINT_NAME   DB_MAX_OBJECT_NAME
#define DB_MAX_FOREIGN_KEY_NAME DB_MAX_OBJECT_NAME

#define DB_SEQUENCE_NAME_SIZE       (DB_MAX_SEQUENCE_NAME + 1)
#define DB_TABLE_NAME_SIZE          (DB_MAX_TABLE_NAME + 1)
#define DB_INDEX_NAME_SIZE          (DB_MAX_INDEX_NAME + 1)
#define DB_FIELD_NAME_SIZE          (DB_MAX_FIELD_NAME + 1)
#define DB_SAVEPOINT_NAME_SIZE      (DB_MAX_SAVEPOINT_NAME + 1)
#define DB_FOREIGN_KEY_NAME_SIZE    (DB_MAX_FOREIGN_KEY_NAME + 1)

/** Minimum buffer count.
 * @see StorageMode::buffer_count, db_file_storage_config_t::buffer_count. */
#define DB_MIN_BUFFER_COUNT  8
/** Maximum buffer count.
 * @see StorageMode::buffer_count, db_file_storage_config_t::buffer_count. */
#define DB_MAX_BUFFER_COUNT  65536
#ifndef DB_COMPACT
#define DB_DEF_BUFFER_COUNT  1024
/** Default buffer count.
 * @see StorageMode::buffer_count, db_file_storage_config_t::buffer_count. */
#define DB_DEF_BUFFER_COUNT  1024
#else
/** Default buffer count.
 * @see StorageMode::buffer_count, db_file_storage_config_t::buffer_count. */
#define DB_DEF_BUFFER_COUNT  16
#endif
/** Default storage cache size, which does not override the buffer count.
 * @see db_file_storage_config_t::storage_cache_size. */
#define DB_DEF_STORAGE_CACHE_SIZE  0

/** Minimum number of buffers to flush at once.
 * @see StorageMode::flush_buffers, db_file_storage_config_t::flush_buffers. */
#define DB_MIN_FLUSH_BUFFERS 8
/** Default number of buffers to flush at once.
 * @see StorageMode::flush_buffers, db_file_storage_config_t::flush_buffers. */
#define DB_DEF_FLUSH_BUFFERS 32

#define DB_DEF_JOURNAL_TYPE DB_EXTERNAL_JOURNAL

/** Minimum checkpoint interval.
 * @see StorageMode::checkpoint_interval,
 *      db_file_storage_config_t::checkpoint_interval. */
#define DB_MIN_CHECKPOINT_INTERVAL 1
/** Maximum checkpoint interval.
 * @see StorageMode::checkpoint_interval,
 *      db_file_storage_config_t::checkpoint_interval. */
#define DB_MAX_CHECKPOINT_INTERVAL 1024*1024
/** Default checkpoint interval.
 * @see StorageMode::checkpoint_interval,
 *      db_file_storage_config_t::checkpoint_interval. */
#define DB_DEF_CHECKPOINT_INTERVAL 128

/** Minimum number of pages to preallocate for the journal.
 * @see StorageMode::preallocate, db_file_storage_config_t::preallocate. */
#define DB_MIN_PREALLOCATE_PAGES 1
/** Maximum number of pages to preallocate for the journal.
 * @see StorageMode::preallocate, db_file_storage_config_t::preallocate. */
#define DB_MAX_PREALLOCATE_PAGES 128
/** Default number of pages to preallocate for the journal.
 * @see StorageMode::preallocate, db_file_storage_config_t::preallocate. */
#define DB_DEF_PREALLOCATE_PAGES 32

#define DB_MIN_MEMORY_PAGE_SIZE 1024
#define DB_MAX_MEMORY_PAGE_SIZE 32768
#define DB_DEF_MEMORY_PAGE_SIZE 2048

#define DB_MIN_MEMORY_STORAGE_SIZE 0u
#define DB_MAX_MEMORY_STORAGE_SIZE 0x7FFFFFFFFFFFFFFFu
#define DB_DEF_MEMORY_STORAGE_SIZE 0u


#define DB_DEF_LIBCFG_CONCURRENCY DB_SINGLE_PROCESS
#define DB_DEF_LIBCFG_MEMMODE     DB_MEM_LARGE
#define DB_DEF_LIBCFG_NTX         20
#define DB_DEF_LMCFG_NOWNERS     0 /* use ntx estimaton */
#define DB_DEF_LMCFG_NOBJECTS    0 /* use dynamic memory */
#define DB_DEF_LMCFG_NLOCKS      0 /* use dynamic memory */

/* date/time limits */

#define DB_MIN_YEAR        (1)
#define DB_MAX_YEAR        (9999)
#define DB_TIME_SCALE      (1000000)

/** Minimum valid value for db_timestamp_usec_t, equal to timestamp '0001-01-01 00:00:00.000000'. */
#define DB_MIN_TIMESTAMP_USEC     (-62135596800000000)
/** Maximum valid value for db_timestamp_usec_t, equal to timestamp '9999-12-31 23:59:59.999999'. */
#define DB_MAX_TIMESTAMP_USEC     (253402300799999999)
#define DB_INVALID_TIMESTAMP_USEC INT64_MAX

#define DB_MAX_MEMSEGMENT_COUNT   8

#define DB_MAX_PEER_NAME    DB_MAX_OBJECT_NAME
#define DB_MAX_PEER_URI     DB_MAX_OBJECT_NAME

#define DB_MAX_FOREIGNKEY_FIELD_COUNT   16

#define DB_MAX_DTM_TXINFO       255
#define DB_DTM_TXINFO_SIZE      (DB_MAX_DTM_TXINFO + 1)
#define DB_MAX_PEER_COUNT       32

#define DB_XID_GTRID_MAX        64
#define DB_XID_BQUAL_MAX        64

#define DB_XID_DATA_MAX        (DB_XID_GTRID_MAX + DB_XID_BQUAL_MAX)

#define DB_CIPHER_DATA_MAX      65536
#define DB_USERNAME_MAX         32
#define DB_PASSWORD_MAX         32

#define DB_VAR_RID_MAX          255

#endif /* DB_DEFS_H */
