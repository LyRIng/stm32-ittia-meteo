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

#ifndef DB_DEBUG_H
#define DB_DEBUG_H

/* Part of testing framework, these values can be used to set various testing
 * parameters in debug mode, and are intended for internal use only.
 */

#include "ittia/os/os_debug.h"

#ifdef DB_DEBUG

#include "ittia/os/os_atomic.h"

/** @name file storage break events.
 *
 * all the sizes and counters are in pages.
 */

/**@{*/

/** prohibit any file operation with page number greater of equal to
 * (emulate low disk space condition)
 */
extern os_atomic_t db_max_file_size;

/**
 * smulate @c db_break_on_file_read operation as failed with DB_EIO
 */
extern os_atomic_t db_break_on_file_read;

/**
 * smulate @c db_break_on_file_write operation as failed with DB_EIO
 */
extern os_atomic_t db_break_on_file_write;

/** Number of file read operations carried so far.
 * This is very difficult to calculate amount of read/writes,
 * thus we provide this coutnters as emperical values.
 */
extern os_atomic_t db_file_read_count;

/** Number of file write operations carried so far.
 * This is very difficult to calculate amount of read/writes,
 * thus we provide this coutnters as emperical values.
 */
extern os_atomic_t db_file_write_count;

/**@}*/

/**
 * @name Nested atomic operations are very critical to storage
 * coherency so these variables are to estimate and simulate breaks
 * in terms of nested operations
 */

/**@{*/

/** break when @c db_break_on_atomic_begin atomic operation is
 * about to start.
 */
extern os_atomic_t db_break_on_atomic_begin;

/** break when @c db_break_on_atomic_begin atomic operation is
 * about to commit.
 */
extern os_atomic_t db_break_on_atomic_end;

/** number of atomic operations started so far.
 *
 * at anytime in user mode followinf condition should be true:
 * db_atomic_begin_count = db_atomic_end_count
 *                       + db_atomic_abort_count;
 */
extern os_atomic_t db_atomic_begin_count;

/** number of atomic operations committed so far. */
extern os_atomic_t db_atomic_end_count;

/** number of atomic operations aborted so far. */
extern os_atomic_t db_atomic_abort_count;

/**@}*/

C_HEADER_BEGIN

long _db_set_alloc_break(long n);
long _db_get_cur_alloc(void);
long _db_get_break_alloc(void);
int  _db_enable_alloc_break(int state, long);
int  _db_set_alloc_break_state( int flag );
long _db_get_alloc_fails(void);
int _db_backtrace(void ** buffer, int size);
int _db_inspect_frame(int, void ** guard_frame, int guard_size);
int _db_enable_inspect_frame(int);

#define _DB_DEBUG_INT32_OPTION              0x10000u
#define _DB_DEBUG_OPTION(tag, type)			(type | tag)
#define DB_DEBUG_CHECK_INDEX_ON_COMMIT      _DB_DEBUG_OPTION(1,_DB_DEBUG_INT32_OPTION)
#define DB_DEBUG_CHECK_INDEX_ON_CLOSE       _DB_DEBUG_OPTION(2,_DB_DEBUG_INT32_OPTION)
DBDLL_API int _db_set_debug_option(uint32_t, void * optval, int optlen);

C_HEADER_END

#endif

#ifdef DB_ENABLE_FAILURE
C_HEADER_BEGIN

DBDLL_API void _db_api_fs_enable(int service, bool use_counter);
DBDLL_API void _db_api_fs_disable();

C_HEADER_END
#endif

#ifdef DB_ENABLE_TIME_STATISTICS
C_HEADER_BEGIN

typedef struct {
    /** Units per second for all other members. */
    uint64_t frequency;

    /** Time spent in file read operations. */
    uint64_t file_read;
    /** Time spent in file write operations. */
    uint64_t file_write;
} db_time_statistics_t;

DBDLL_API void _db_time_statistics(db_time_statistics_t * stat, int flags);

C_HEADER_END
#endif

#endif /* DB_DEBUG_H */
