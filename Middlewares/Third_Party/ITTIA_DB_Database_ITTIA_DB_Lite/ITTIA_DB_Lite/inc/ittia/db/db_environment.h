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

/** @file db_environment.h
 *
 * Common environment declarations for ITTIA DB.
 */

#ifndef ITTIA_DB_ENVIRONMENT_H
#define ITTIA_DB_ENVIRONMENT_H

#include <ittia/os/os_config.h>
#include <ittia/os/os_byte_order.h>
#include <ittia/db/db_defs.h>
#include <ittia/db/db_error.h>

C_HEADER_BEGIN

/* supported API versions */
#define DB_API_113 0x113u

#ifndef DB_API_VER
/** Current API version number. @see db_init_ex. */
#define DB_API_VER DB_API_113
#endif

#if (DB_API_VER < DB_API_113) || (DB_API_VER > DB_API_113)
#error "Unsupported API version"
#endif

/** Declare a handle type. */
#define DB_DECLARE_HANDLE(name) typedef struct name##_s * name

/** Database connection handle. @see CAPIDatabase @ingroup CAPIDatabase */
DB_DECLARE_HANDLE(db_t);

/** Flag bits data type. */
typedef int32_t db_flags_t;

/** API version type. @see db_init_ex */
typedef uint16_t       db_api_t;

/** Get the dimension of a static array. The dimension is computed at
 * compile-time, so this value can be used as a constant. */
#define DB_ARRAY_DIM(x) (sizeof(x) / sizeof(x[0]))

/**
 * ITTIA DB library configuration.
 * @ingroup CAPIConfiguration
 *
 * @see db_config_init, db_config_destroy, db_set_config, db_get_config
 */
typedef struct db_config_t
{
#if DB_API_VER >= DB_API_100
    int concurrency;

    /** Memory optimization strategy */
    int memory_mode;

    /** Approximate number of concurrent transactions expected */
    int transactions;
#endif
} db_config_t;

/** Lock manager configuration for local lock manager.
 * @ingroup CAPIConfiguration
 *
 * @see db_init_ex
 */
typedef struct lm_local_config_t
{

#if DB_API_VER >= DB_API_100
    /** @name Lock manager configuration
     *  @{ */
    int nlocks;   /**< Maximum number of locks to use at once. */
    int nowners;  /**< Maximum number of lock owners (connections). */
    int nobjects; /**< Maximum database objects to be locked at once. */
    /** @} */
#endif

} lm_local_config_t;

#ifndef _DB_OMIT_LM_MODE_NAMES
#define LM_NONE                  0
#define LM_LOCAL                 1
#endif

/** Lock manager configuration.
 * @ingroup CAPIConfiguration
 *
 * @see db_init_ex
 */
typedef struct lm_config_t
{

#if DB_API_VER >= DB_API_100

    int lm_mode;
    union {
        /*! Local lock manger. Use with <code>lm_mode = _LM_LOCAL</code>. */
        lm_local_config_t local;
    } u;

#endif

} lm_config_t;


/* initialize memory component */

/** Memory segment for general allocation.
 * @ingroup CAPIEnvironment
 */
typedef struct db_memory_segment_s
{
    /**
     * A preallocated buffer of the given size. Use NULL to obtain a buffer
     * from the system allocator.
     */
    void * ptr;

    /** Size of the memory segment, in bytes. */
    size_t size;
} db_memory_segment_t;

/**
 * Configuration for built-in memory allocator. Settings and resources defined
 * here are shared between all database connections.
 * @ingroup CAPIEnvironment
 */
typedef struct db_memory_config_s
{
    size_t      block_size; /**< Minimum unit of allocation. */
    /**
     * Memory allocation flags. Any combination of:
     * - #DB_MEMFLAGS_TRACE
     */
    db_flags_t  mem_flags;

    /**
     * Enable memory allocation statistics. This incurs additional overhead and
     * should only be used during testing.
     * @ingroup CAPIEnvironment
     *
     * @see db_mem_statistics, db_done_ex.
     */
#define     DB_MEMFLAGS_TRACE           1

    /**
     * Number of memory segments for the built-in memory allocator. If a
     * single contiguous segment of memory of the desired size is not
     * available, up to eight segments can be defined. Set to zero to use the
     * system memory allocator.
     */
    size_t      num_segs;
    /** List of memory segment definitions. */
    db_memory_segment_t mem_seg[DB_MAX_MEMSEGMENT_COUNT];
} db_memory_config_t;

/** Extended library initialization parameters.
 * @ingroup CAPIEnvironment
 */
typedef struct db_init_s
{
    const db_memory_config_t * mem_config;     /**< Memory allocator configuration. */
    const db_config_t * db_config;             /**< Database library configuration. */
    const lm_local_config_t * lm_local_config; /**< Local lock manager configuration. */
    const lm_config_t * lm_config;             /**< General lock manager configuration. Takes precedent over lm_local_config if supplied. */
} db_init_t;

/** Measured statistics item.
 * @ingroup CAPIEnvironment
 */
typedef struct db_statitem_s
{
    db_counter_t cur_value; /**< Current value. */
    db_counter_t max_value; /**< Maximum value recorded since last reset. */
} db_statitem_t;

/**
 * Count ITTIA DB C API handles opened by the application to measure resource
 * usage and prevent leaks. If an application closes handles correctly,
 * db.cur_value, cursor.cur_value, and seq.cur_value should be zero when all
 * storages are closed.
 * @ingroup CAPIEnvironment
 *
 * @see db_api_statistics, db_done_ex
 */
typedef struct db_api_statistics_s
{
    /** True when API statistics are available. Some builds may not collect
     * these statistics. */
    int have_statistics;

    db_statitem_t db;       /**< Number of database handles open. @see db_t */
    db_statitem_t row;      /**< Number of rows open. @see db_row_t */
    db_statitem_t cursor;   /**< Number of cursors open. @see db_cursor_t */
    db_statitem_t seq;      /**< Number of sequences open. @see db_sequence_t */
    db_statitem_t seqdef;   /**< Number of sequence definitions open. @see db_seqdef_t */
    db_statitem_t tabledef; /**< Number of table definitions open. @see db_tabledef_t */
    db_statitem_t indexdef; /**< Number of index definitions open. @see db_indexdef_t */
    db_statitem_t oid;      /**< Number of object IDs open. @see db_oid_t */

} db_api_statistics_t;

/**
 * Lock manager statistics.
 * @ingroup CAPIEnvironment
 * 
 * @see db_lm_statistics, db_done_ex
 */
typedef struct db_lm_statistics_s
{
    /** True when lock manager statistics are available. Some builds may not
     * collect these statistics. */
    int have_statistics;

    db_statitem_t nlocks;   /**< Number of locks in use. */
    db_statitem_t nowners;  /**< Number of connections with open locks. */
    db_statitem_t nobjects; /**< Number of locked database objects. */
} db_lm_statistics_t;

/**
 * Statistics for memory allocation with the built-in allocator. Only memory
 * allocated internally by ITTIA DB is tracked, but the amount can vary with
 * application behavior.
 * @ingroup CAPIEnvironment
 *
 * @see db_mem_statistics, db_done_ex, db_init_ex
 */
typedef struct db_mem_statistics_s
{
    /**
     * True when memory allocation statistics are available. These statistics
     * are only available when the built-in allocator has been enabled, the
     * #DB_MEMFLAGS_TRACE flag is set, and the QA library is linked.
     *
     * @see db_init_ex */
    int have_statistics;

    db_statitem_t nreq; /**< Number of outstanding allocations. */
    struct
    {
        /** Total heap memory consumed, including overhead, in bytes. */
        db_statitem_t sys;

        /** Total allocated space, excluding overhead, in bytes. */
        db_statitem_t user;

        /** Maximum individual allocation request, in bytes, since reset. */
        size_t max_alloc;
    } actual_bytes;

    struct
    {
        /* Upper bound for the estimated number of blocks required for current
         * allocations. This value is computed as though each request uses at
         * least one full block, even though the actual number of blocks used
         * may be less in practice. */
        db_statitem_t heap;

        /** Maximum number of blocks required for an individual allocation
         * since reset. */
        size_t max_alloc;
    } estimate_blocks;

    size_t num_segs;    /**< Number of segments used for at least one allocation. */
    size_t page_size;   /**< Block size (minimum unit of allocation). */
    size_t heap_size;   /**< Total memory configured for the allocator. */
} db_mem_statistics_t;


/** Extended library decommissioning parameters.
 * @ingroup CAPIEnvironment
 */
typedef struct db_done_s
{
    struct db_mem_statistics_s * mem_statistics; /**< Final memory allocation statistics, if enabled. */
    struct db_api_statistics_s * api_statistics; /**< Final API statistics. */
    struct db_lm_statistics_s  * lm_statistics;  /**< Final lock-manager statistics. */
} db_done_t;

/** @addtogroup CAPIEnvironment
 *  @{ */

DBDLL_API int                db_init_ex(db_api_t, const db_init_t *);
DBDLL_API int                db_done_ex(db_done_t *);

/** @} */

/** @name Trace Facilities
 *  @{ */
/** @ingroup CAPIDatabase */
/* Note: Trace facilities are also listed in oslib/db_dump.h */
#define DB_TRACE_USERMSG    0x00000001u  /**< User message debug facility */
#define DB_TRACE_BUFMGR     0x00000002u  /**< Buffer manager debug facility */
#define DB_TRACE_LOCKMGR    0x00000004u  /**< Lock manager debug facility */
#define DB_TRACE_LOGMGR     0x00000008u  /**< Log manager debug facility */
#define DB_TRACE_SPACEMGR   0x00000010u  /**< Space manager debug facility */
#define DB_TRACE_TXMGR      0x00000020u  /**< Transaction manager debug facility */
#define DB_TRACE_CATALOG    0x00000040u  /**< Catalog debug facility */
#define DB_TRACE_OSLIB      0x00000080u  /**< OS layer debug facility */
#define DB_TRACE_CAPI       0x00000100u  /**< C API debug facility */
#define DB_TRACE_DUMP       0x00000200u  /**< General message debug facility */
#define DB_TRACE_MEMTABLE   0x00000400u  /**< Memory storage component */
#define DB_TRACE_REP        0x00000800u  /**< Replication component */
#define DB_TRACE_BTREE      0x00001000u  /**< Btree component */
#define DB_TRACE_HEAP       0x00002000u  /**< Memory allocations */
#define DB_TRACE_DISKTABLE  0x00004000u  /**< FileStorage component */
#define DB_TRACE_SQL        0x00008000u  /**< SQL component */
#define DB_TRACE_IPC        0x00010000u  /**< IPC client/server component */
#define DB_TRACE_JOURNAL    0x00020000u  /**< Journal rotation and archiving */
#define DB_TRACE_BTREE_TRAV 0x00040000u  /**< Btree component traversal */
#define DB_TRACE_STREAM     0x00080000u  /**< Stream processing */
#define DB_TRACE_MAX        0x70000000u  /**< max possible trace facility */
#define DB_TRACE_ALL        0x7FFFFFFFu  /**< all facilities included */
/** @} */

/** @addtogroup CAPIConfiguration
 *  @{ */

DBDLL_API void db_set_trace(const char * trace_file, uint32_t enable_mask, uint32_t disable_mask);

/** @} */


/** @cond INTERNAL */
#define DB_CIPHER_AES_         0x01000000u
#define DB_CIPHER_HARDWARE_AES_ 0xf1000000u
#define DB_CIPHER_CTR_         0x00010000u
#define DB_CIPHER_TYPE_MASK_   0xff000000u
#define DB_CIPHER_MODE_MASK_   0x00ff0000u
#define DB_CIPHER_PARAM_MASK_  0x0000ffffu
/** @endcond */

#if DB_API_VER >= DB_API_10C && !defined(_DB_OMIT_CIPHER_NAMES)
/** @name Encryption Cipher
 *  @{ */
/** @ingroup CAPIDatabase */
/** No cipher for encryption and decryption. */
#define DB_CIPHER_NONE         0
/** User-defined cipher. @see db_crypt_t */
#define DB_CIPHER_USER         0xff000000u
/** AES-128 in CTR mode. */
#define DB_CIPHER_AES128_CTR   (DB_CIPHER_AES_ | DB_CIPHER_CTR_ | 128u)
/** AES-256 in CTR mode. */
#define DB_CIPHER_AES256_CTR   (DB_CIPHER_AES_ | DB_CIPHER_CTR_ | 256u)
/** AES with hardware-specific configuration and key management in CTR mode. */
#define DB_CIPHER_HARDWARE_AES_CTR (DB_CIPHER_HARDWARE_AES_ | DB_CIPHER_CTR_)
/** @} */
#endif

/**
 * Authorization credentials for a password-protected or encrypted database.
 * @ingroup CAPIDatabase
 */
typedef struct db_auth_info_s
{
    uint32_t        cipher_type;
    const void *    cipher_data;
    uint32_t        cipher_data_size;

    const char * user_name;
    const char * password;
} db_auth_info_t;


/** @name File/Table Open Flags
 *  @{ */
/** @ingroup CAPIDatabase */
/// Open the database object, which must already exist.
#define DB_OPEN_EXISTING       0x000000u
/// Open the database object, creating it if necessary.
#define DB_OPEN_OR_CREATE      0x100000u
/// Create an empty database object, replacing an existing database if necessary.
#define DB_CREATE_OR_OVERWRITE 0x200000u
/// Create an empty database object, which must not already exist.
#define DB_CREATE_NEW          0x300000u

/// Bitmask for open/create flags.
#define DB_OPEN_CREATE_MASK    0x300000u
/** @} */

/** @name File Storage Flags
 *  @{ */
/** @ingroup CAPIDatabase */
/** Open database for reading only. */
#define DB_READ_ONLY            0x0001u
/** Disable logging. */
#define DB_NOLOGGING            0x0002u
/** Verify checksums when data is read from the database file. */
#define DB_VERIFY_CHECKSUMS     0x0100u
/** Do not immediately flush full data pages. */
#define DB_DEFERRED_PAGE_FLUSH  0x0400u
/** Optimize time series range queries, but with higher memory and storage consumption. */
#define DB_TIME_SERIES_FAST_RANGE 0x0800u
/** @} */

/** @name Flush Mode
 *  @{ */
/** @ingroup CAPITransaction */
#ifndef _DB_OMIT_FLUSH_NAMES
/** Flush only the journal. */
#define DB_FLUSH_JOURNAL         0x00000000u
/** Flush both the journal and storage. */
#define DB_FLUSH_STORAGE         0x10000000u
/** Bitwise mask for transaction flush mode flags. */
#define DB_FLUSH_MASK            0x10000000u
#endif
/** @} */

/** @name Transaction Completion Modes
 *  @{ */
/** @ingroup CAPITransaction */
#ifndef _DB_OMIT_COMPLETION_NAMES
/** Default completion mode. */
#define DB_DEFAULT_COMPLETION    0x00000000u
/** Lazy completion. */
#define DB_LAZY_COMPLETION       0x01000000u
/** Immediate completion. */
#define DB_FORCED_COMPLETION     0x02000000u
/** Group completion. */
#define DB_GROUP_COMPLETION      0x03000000u
/** Bitwise mask for transaction completion mode flags. */
#define DB_COMPLETION_MASK       0x07000000u
#endif
/** @} */


#ifndef _DB_OMIT_SEEK_NAMES
/** Seek flags.
 * @ingroup CAPITableCursor
 * @see db_seek, db_qseek.
 */
typedef enum
{
    /** Less than. */
    DB_SEEK_LESS             = 0,
    /** Less than or equal to. */
    DB_SEEK_LESS_OR_EQUAL    = 1,
    /** First exact match. */
    DB_SEEK_FIRST_EQUAL      = 2,
    /** Last exact match. */
    DB_SEEK_LAST_EQUAL       = 3,
    /** Greater than. */
    DB_SEEK_GREATER          = 4,
    /** Greater than or equal to. */
    DB_SEEK_GREATER_OR_EQUAL = 5,

    /** Equivalent to DB_SEEK_FIRST_EQUAL. */
    DB_SEEK_EQUAL            = DB_SEEK_FIRST_EQUAL
} db_seek_t;
#endif

C_HEADER_END

#endif /* ITTIA_DB_ENVIRONMENT_H */
