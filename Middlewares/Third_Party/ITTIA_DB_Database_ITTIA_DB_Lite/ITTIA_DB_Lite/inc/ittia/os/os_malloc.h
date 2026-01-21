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

/* @file
 *
 * Memory allocation functions
 */

#ifndef OS_MALLOC_H
#define OS_MALLOC_H

#include "ittia/os/os_config.h"
#include "ittia/os/os_debug.h"
#include "ittia/os/os_atomic.h"

/* === C stuff === */
C_HEADER_BEGIN

#ifdef DB_DEBUG
#   define DB_MEMTRACE
#endif

#define DB_UNKNOWN_MEMBLOCK     0
#define DB_FREE_MEMBLOCK        1
#define DB_DELAYFREE_MEMBLOCK   2
#define DB_SYSTEM_MEMBLOCK      3
#define DB_MALLOC_MEMBLOCK      4
#define DB_NEW_MEMBLOCK         5
#define DB_NEWARR_MEMBLOCK      6

/* relatively small memory chunk allocation */
DBDLL_API void * _db_malloc(db_size_t, const char * file, int line, int block_kind);
DBDLL_API void * _db_realloc(void *, db_size_t, const char * file, int line, int block_kind);
DBDLL_API void * _db_calloc(db_size_t, db_size_t, const char * file, int line, int block_kind);
DBDLL_API void   _db_free(void *, const char * file, int line, int block_kind);
DBDLL_API void * _db_memdup(void *, db_size_t, const char * file, int line, int block_kind);

#ifdef DB_DEBUG
#   define db_malloc(size)          _db_malloc(size, __FILE__, __LINE__, DB_MALLOC_MEMBLOCK )
#   define db_realloc(ptr, size)    _db_realloc(ptr, size, __FILE__, __LINE__, DB_MALLOC_MEMBLOCK)
#   define db_calloc(n, size)       _db_calloc(n, size, __FILE__, __LINE__, DB_MALLOC_MEMBLOCK )
#   define db_free(ptr)             _db_free(ptr, __FILE__, __LINE__, DB_MALLOC_MEMBLOCK)
#   define db_memdup(ptr, size)     _db_memdup( ptr, size, __FILE__, __LINE__, DB_MALLOC_MEMBLOCK)
#else
#   define db_malloc(size)          _db_malloc(size, NULL, 0, DB_UNKNOWN_MEMBLOCK )
#   define db_realloc(ptr, size)    _db_realloc(ptr, size, NULL, 0, DB_UNKNOWN_MEMBLOCK)
#   define db_calloc(n, size)       _db_calloc(n, size, NULL, 0, DB_MALLOC_MEMBLOCK )
#   define db_free(ptr)             _db_free(ptr, NULL, 0, DB_UNKNOWN_MEMBLOCK)
#   define db_memdup(ptr, size)     _db_memdup( ptr, size, NULL, 0, DB_UNKNOWN_MEMBLOCK)
#endif

/* large memory allocation */

DBDLL_API void * db_hmalloc(db_size_t);
DBDLL_API dbstatus_t db_hfree(void *, db_size_t);

/* internal memory allocator */

typedef struct _db_UblkStat
{
    size_t cur_heap;
    size_t max_heap;

    size_t max_alloc;
} _db_UblkStat;

typedef struct
{
    double nfail;       /* number of failures due to absence of memory */
    double nmalloc;     /* number of successfull mallocs */
    double nrealloc;    /* number of successfull reallocs */
    double nfree;       /* number of successfull frees */

    size_t nreq_cur;    /* number of current allocations */
    size_t nreq_max;    /* peak of current allocations */
    double nreq_all;    /* number of allocations served */

    size_t sys_init;    /* initial space occupied by system data */
    size_t sys_cur;     /* currently used heap space */
    size_t sys_max;     /* maximum used heap space */
    double sys_all;     /* */
    
    size_t user_cur;    /* currently allocated space */
    size_t user_max;    /* maximum ever allocated space */
    double user_all;    /* */

    /* memory consumption */
    _db_UblkStat ublk;

    size_t max_alloc;   /* maximal individual allocation request */
    size_t num_segs;
    size_t page_size;
    size_t heap_size;
/*    size_t free_pages; */

} _db_MemStatistics;

typedef struct
{
	void * ptr;     /* could be NULL. the segment would be allocated from OS */
    size_t blocks;  /* number of blocks in this segment */
} _db_MemSegment;

#define _DB_MEMSEGMENT_MAX      8

/* db_set_memory flags */
#define _DB_MEM_TRACE           1

/* attach memory */
DBDLL_API int _db_set_memory( size_t block_size, const _db_MemSegment *, size_t num_segs, unsigned flags );
DBDLL_API int _db_get_mem_statistics(_db_MemStatistics *, int reset );
DBDLL_API void _db_mem_dump(void);
int _db_get_final_mem_statistics(_db_MemStatistics * );

#ifdef DB_MEMTRACE

typedef struct db_mem_trace_t {
    os_atomic_t total_mem;    /**< Current total allocations. */
    os_atomic_t peak_mem;     /**< Peak memory usage. */
    os_atomic_t halloc_mem;   /**< Current allocations made with db_halloc(). */
    os_atomic_t malloc_count; /**< Number of calls to db_malloc(). */
    os_atomic_t free_count;   /**< Number of calls to db_free(). */
} db_mem_trace_t;

db_mem_trace_t db_get_mem_trace(void);
void db_reset_mem_trace(void);
long os_memory_used(void);
long os_hmemory_used(void);

#endif /* DB_MEMTRACE */

C_HEADER_END


#ifdef DB_DIRECT_MALLOC

/* Calling malloc directly can aid in debugging on some platforms. */
#   include "ittia/os/std/malloc.h"
#   define DB_MALLOC(size)       malloc(size)
#   define DB_REALLOC(p, size)   realloc(p, size)
#   define DB_FREE(p)            free(p)
#   define DB_CALLOC(n, size)    calloc(n, size)

#else /* DB_DIRECT_MALLOC */

#   define DB_MALLOC(size)       db_malloc(size)
#   define DB_REALLOC(p, size)   db_realloc(p, size)
#   define DB_FREE(p)            db_free(p)
#   define DB_CALLOC(n, size)    db_calloc(n, size)

#endif /* DB_DIRECT_MALLOC */



/* === C++ stuff === */


/* === deprecated stuff === */

#if 0 /* deprecated */

/* checked alloc */
#if defined(DB_DEBUG) && defined(OS_MSC) && defined(OS_WINNT)

#   include <stdlib.h>
#   include <crtdbg.h>

C_LINKAGE DBDLL_API void * db_validate_pointer(void *, int null_ok);
C_LINKAGE DBDLL_API void db_check_mem(void);

#ifdef DB_MEMDEBUG
#   define DB_VALIDATE_PTR(p) db_validate_pointer(p, null_ok)
#else
#   define DB_VALIDATE_PTR(p) (p)
#endif /* DB_MEMDEBUG */

/* NOTE: DB_DELETE() and DB_FREE() may be prefixed with :: to ensure global scope. */

#  define THIS_FILE             __FILE__
#ifdef __cplusplus
#  define DB_NEW(x)             DB_VALIDATE_PTR(new (_NORMAL_BLOCK, THIS_FILE, __LINE__) x)
#  define DB_NEW_ARRAY(x, size) DB_VALIDATE_PTR(new (_NORMAL_BLOCK, THIS_FILE, __LINE__) x [size])
#  define DB_DELETE(x)          delete DB_VALIDATE_PTR(x)
#  define DB_DELETE_ARRAY(x)    delete [] DB_VALIDATE_PTR(x)
#endif

#  define DB_MALLOC(size)       DB_VALIDATE_PTR(_malloc_dbg(size, _NORMAL_BLOCK, THIS_FILE, __LINE__))
#  define DB_REALLOC(p, size)   DB_VALIDATE_PTR(_realloc_dbg(DB_VALIDATE_PTR(p), size, _NORMAL_BLOCK, THIS_FILE, __LINE__))
#  define DB_FREE(p)            _free_dbg(DB_VALIDATE_PTR(p), _NORMAL_BLOCK)

#else /* defined(DB_DEBUG) && defined(OS_MSC) && defined(OS_WINNT) */

#ifdef __cplusplus
#  define DB_NEW(x)             new x
#  define DB_NEW_ARRAY(x, size) new x [size]
#  define DB_DELETE(x)          delete x
#  define DB_DELETE_ARRAY(x)    delete [] x
#endif /* __cplusplus */

#endif /* defined(DB_DEBUG) && defined(OS_MSC) && defined(OS_WINNT) */

#endif /* 0 */


#endif /* OS_MALLOC_H */
