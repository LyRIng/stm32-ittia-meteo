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

#define DB_BUILTIN_ALLOC
/* #define DB_MEMDEBUG */
/* #define DB_MEMTRACE */

#include "ittia/os/os_env.h"

#include "os/os_lib.h"
#include "ittia/os/os_malloc.h"
#include "ittia/os/std/malloc.h"
#include "ittia/os/std/memory.h"
#include "ittia/os/os_error.h"
#include "ittia/os/os_debug.h"
#include "ittia/os/os_atomic.h"
#ifdef OS_WIN32
#include "os/win32/win32.h"
#endif

#include "os/os_mockup.h"

#ifdef DB_ENABLE_FAILURE
#include "ittia/os/os_fail.h"
#else
#define FS_THROW_RES(service, result)
#endif

#if defined(DB_MEMDEBUG) && defined(DB_POINTER_ALIGNMENT)
#   undef DB_MEMDEBUG
#endif

#ifdef DB_MEMTRACE
static db_mem_trace_t mem_trace = { 0, 0, 0, 0 };
static os_atomic_t cur_memory_count = 0;
static os_atomic_t cur_hmemory_count = 0;
#define os_memory_account( inc, dec )   os_atomic_add( &cur_memory_count, (long)(inc) - (long)(dec))
#define os_hmemory_account( inc, dec )  os_atomic_add( &cur_hmemory_count, (long)(inc) - (long)(dec))
#endif

#ifdef DB_DEBUG
#define FAIL_AT 0

static int enable_alloc_break = 0;
os_atomic_t fail_alloc_at = FAIL_AT;
os_atomic_t cur_alloc = 0;
os_atomic_t break_alloc = 0;
os_atomic_t num_alloc_fails = 0;
static os_atomic_t break_at = 0;
static os_atomic_t cur_req = 0;

long _db_get_cur_alloc(void)
{
    return cur_alloc;
}

long _db_get_break_alloc(void)
{
    return break_alloc;
}

long _db_set_alloc_break(long n)
{
    long cur = fail_alloc_at;

    fail_alloc_at = n;
    cur_alloc = 0;
    break_alloc = 0;
    num_alloc_fails = 0;

    return cur;
}

long _db_get_alloc_fails()
{
#if 0
    /* atomic exchange would be sufficient if it was implemented */
    return os_atomic_exch( &num_alloc_fails, 0 );
#else
    os_atomic_t n;
    do {
        n = os_atomic_fetch( &num_alloc_fails );
    } while( !os_atomic_cas( &num_alloc_fails, n, 0) );
    return n;
#endif
}

int
_db_enable_alloc_break(int new_state, long new_fail_at)
{
    int cur_state = enable_alloc_break;

    if (new_state >= 0) {
        if (new_fail_at == 0) {
            /* check if frame inspecting could be used? */
        }

        if (new_fail_at >= 0) {
            _db_set_alloc_break( new_fail_at );
        }

        enable_alloc_break = new_state;
    }

    return cur_state;
}

int _db_set_alloc_break_state( int flag )
{
	int cur = enable_alloc_break;
	enable_alloc_break = flag != 0;
	return cur;
}

int check_alloc_fail()
{
    os_atomic_t n;
    if (enable_alloc_break) {
        /* always count allocations for debug purporses */
        n = os_atomic_inc(&cur_alloc);

        /* check if counting break is enabled */
        if (fail_alloc_at > 0) {

            /* let put a break point a step before the failure */
            if (n + 1 == fail_alloc_at) {
                n = n;
            }

            /* break when required */
            else if (n >= fail_alloc_at) {
                goto simulate;
            }
        }
        
        /* use inspect break */
        else if (_db_inspect_frame(0, NULL, 0)) {
            goto simulate;
        }

    }

    return 0;

simulate:
    set_db_error( DB_EDBGFAILURE );
    os_atomic_inc( &num_alloc_fails );
    os_atomic_cas( &break_alloc, 0, n );
    return 1;
}

#endif

static int allocate = 0; /* non-zero when allocation is enabled.
                          * Not thread safe due to the circular dependences
                          */

#ifdef DB_BUILTIN_ALLOC
static _db_MemStatistics * final_mem_statistics = NULL;
#endif

#ifdef DB_MEMDEBUG
/* simple memory debugging with guards. just to start with */

#include "ittia/os/os_atomic.h"
#include "ittia/os/os_debug.h"

#define INIT_BEGIN  0xdeaddead
#define INIT_END    0xbeafbeaf
#define OVERHEAD    (sizeof(size_t)*4)
#define OFFSET      (sizeof(size_t)*2)

#   define uptr2rptr(ptr)             (void*)((char*)(ptr) - OFFSET)
#   define rptr2uptr(ptr)             (void*)((char*)(ptr) + OFFSET)
#   define get_rptr_size(rptr)        (*(size_t*)(rptr))


static
void
check_mem(void * ptr)
{
    size_t * begin = (size_t *)ptr;
    size_t * end;
    size_t size;
    
    size = *begin;
    end = (size_t*)((char*)begin + size + OVERHEAD);

    DB_ASSERT( end >= (size_t*)((uint8_t*)begin + OVERHEAD));
    DB_ASSERT( end[-1] == size );
    
    DB_ASSERT( begin[1] == INIT_BEGIN );
    DB_ASSERT( end[-2] == INIT_END );
}

static
void
init_mem(void * ptr, size_t size)
{
    size_t * begin = (size_t*)ptr;
    size_t * end   = (size_t*)((char*)ptr + size + OVERHEAD);

    begin[0] = size;
    begin[1] = INIT_BEGIN;
    end[-2] = INIT_END;
    end[-1] = size;

    check_mem( ptr );
}

static
void
free_mem(void * ptr)
{
    size_t size;

    check_mem(ptr);

    size = get_rptr_size(ptr);

    memset(ptr, 0xce, size + OVERHEAD);
}

#elif defined(DB_MEMTRACE)

#   define OVERHEAD sizeof(double)  /* just something restrictive */
#   define uptr2rptr(ptr)             (void*)((char*)(ptr) - OVERHEAD)
#   define rptr2uptr(ptr)             (void*)((char*)(ptr) + OVERHEAD)
#   define set_rptr_size(rptr, size) (*(size_t*)(rptr) = (size))
#   define get_rptr_size(rptr)       (*(size_t*)(rptr))

#else
#   define OVERHEAD 0

#   define uptr2rptr(ptr)             (ptr)
#   define rptr2uptr(ptr)             (ptr)

#endif

#if defined (_MSC_VER) && defined(_DEBUG)
#   include <crtdbg.h>
#   define std_malloc(size, file, line)         _malloc_dbg(size, _NORMAL_BLOCK, file, line)
#   define std_realloc(ptr, size, file, line)   _realloc_dbg(ptr, size, _NORMAL_BLOCK, file, line)
#   define std_free(ptr, file, line)            _free_dbg(ptr, _NORMAL_BLOCK)
#elif defined(OS_MAPUSOFT) && defined(OS_VXWORKS)
#   define std_malloc(size, file, line)         malloc(size)
#   define std_realloc(ptr, size, file, line)   mapusoft_realloc(ptr, size)
#   define std_free(ptr, file, line)            free(ptr)
#else
#   if defined(OS_MAPUSOFT)
#       undef malloc
#       undef realloc
#       undef free
#   endif
#   define std_malloc(size, file, line)         malloc(size)
#   define std_realloc(ptr, size, file, line)   realloc(ptr, size)
#   define std_free(ptr, file, line)            free(ptr)
#endif

#ifdef DB_BUILTIN_ALLOC

#   include "buddy.h"

static Buddy * heap = NULL;

#   define real_malloc(size, file, line, kind, opno)        \
        (heap ? _buddy_malloc( heap, (size), NULL, file, line, kind, opno) \
                : std_malloc( size, file, line ))

#   define real_realloc(ptr, size, file, line, kind, opno)  \
        (heap ? _buddy_realloc( heap, (ptr), (size), NULL, file, line, kind, opno) \
                : std_realloc( (ptr), (size), file, line ))

#   define real_free(ptr, file, line, kind, opno)                  \
        (heap ? _buddy_free( heap, (ptr), file, line, kind, opno ) \
                : std_free( ptr, file, line ))

#else

#   define real_malloc(size, file, line, kind, opno)        \
        malloc( size )
#   define real_realloc(ptr, size, file, line, kind, opno)  \
        realloc( ptr, size )
#   define real_free(ptr, file, line, kind)                 \
        free( ptr )

#endif

int
_db_set_memory( size_t block_size, const _db_MemSegment * segs, size_t num_segs, unsigned flags )
{
#ifdef DB_BUILTIN_ALLOC
    if (heap != NULL || allocate) {
        /* already initialized */
        return DB_ESTATE;
    }

    allocate = 1;

    if (segs != NULL && num_segs > 0) {
        heap = buddy_init( NULL, block_size, segs, num_segs, flags );

        if (heap == NULL)
            return DB_EINVAL;
    }

    return DB_NOERROR;
#else
    DB_IN_DEBUG(os_trace_output(__FILE__, __LINE__, DB_TRACE_OSLIB, "not implemented in this build: built-in memory allocator"));
    return DB_ENOTIMPL;
#endif
}

int
os_mem_init(void)
{
    allocate = 1;
    return DB_NOERROR;
}

int
os_mem_done(void)
{
    allocate = 0;

    /* destroy the buddy heap */
#ifdef DB_BUILTIN_ALLOC
    if (heap != NULL) {
        if (final_mem_statistics) {
            buddy_get_stats( heap, final_mem_statistics, 0);
            final_mem_statistics = NULL;
        }
        buddy_destroy(heap);
        heap = NULL;
    }
#endif
    return DB_NOERROR;
}

int _db_get_mem_statistics(_db_MemStatistics * mem_stats, int reset)
{
#ifdef DB_BUILTIN_ALLOC
    if (heap == NULL)
        return DB_ESTATE;

    return buddy_get_stats( heap, mem_stats, reset );
#else
    DB_IN_DEBUG(os_trace_output(__FILE__, __LINE__, DB_TRACE_OSLIB, "not implemented in this build: built-in memory allocator"));
    return DB_ENOTIMPL;
#endif
}

int _db_get_final_mem_statistics(_db_MemStatistics * mem_stats)
{
#if defined(DB_MEMTRACE)
#ifdef DB_BUILTIN_ALLOC
    if (heap == NULL)
#endif
    {
        db_mem_trace_t mem_trace;
        mem_trace = db_get_mem_trace();

        final_mem_statistics = mem_stats;
        memset(mem_stats, 0, sizeof *mem_stats);
        mem_stats->user_cur = mem_trace.total_mem;
        mem_stats->user_max = mem_trace.peak_mem;
        mem_stats->nmalloc = mem_trace.malloc_count;
        mem_stats->nfree = mem_trace.free_count;
        mem_stats->nreq_cur = mem_trace.malloc_count - mem_trace.free_count;
        return 1;
    }
#endif

#ifdef DB_BUILTIN_ALLOC
    if (heap == NULL)
        return DB_ESTATE;

    final_mem_statistics = mem_stats;
    return buddy_get_stats( heap, mem_stats, 0 );
#else
    DB_IN_DEBUG(os_trace_output(__FILE__, __LINE__, DB_TRACE_OSLIB, "not implemented in this build: built-in memory allocator"));
    return DB_ENOTIMPL;
#endif
}

void _db_mem_dump()
{
    if (heap)
        buddy_dump_leaks( heap );
}

#ifdef DB_DEBUG
static C_INLINE_SPECIFIER unsigned long
check_request()
{
    os_atomic_t n = os_atomic_inc(&cur_req);
    if (n == break_at && break_at != 0)
    {
        os_dbg_break(__FILE__, __LINE__, "Memory request {%d}break\n", n);
        break_at = 0;
    }

    return n;
}
#else
#define check_request() (0)
#endif

#if defined(OS_MAPUSOFT) && defined(OS_VXWORKS)
void * mapusoft_realloc(VOID* ptr, UNSIGNED nBytes)
{
	INT_OS_DYNAMIC_HDR * header;
	VOID* resptr;

	if (ptr == NULL)
		return malloc(nBytes);

	header = (UNSIGNED)ptr - INT_OS_MEM_OVERHEAD;

	if (nBytes == 0) {
		free(ptr);
		return NULL;
	}

	if (nBytes <= header->size)
		return ptr;

	/* Grow and copy buffer. */
	resptr = malloc(nBytes);
	if (resptr == NULL)
		return NULL;

	memcpy(resptr, ptr, header->size);
	free(ptr);

	return resptr;
}
#endif

void * _db_malloc(db_size_t size, const char * file, int line, int block_kind)
{
    uint8_t * rptr;

    unsigned long opno = check_request();

    if (!allocate) {
        set_db_error( DB_ESTATE );
        return NULL;
    }

#ifdef DB_DEBUG
    if (check_alloc_fail())
        return NULL;
#endif

    FS_THROW_RES( FS_MALLOC, (void*)NULL );

    rptr = (uint8_t*)real_malloc(size + OVERHEAD, file, line, block_kind, opno);

    if (rptr == NULL) {
		set_db_error(DB_ENOMEM);
        return NULL;
    }
    /* check alignment */
    DB_ASSERT( (uintptr_t) rptr % sizeof(db_AlignType) == 0 );

#ifdef DB_MEMTRACE
    {
        os_atomic_t total_mem = os_atomic_add(&mem_trace.total_mem, size);
        /* note: peak_mem is not calculated atomically because slight inaccuracy is permissible. */
        if (total_mem > os_atomic_fetch(&mem_trace.peak_mem))
            os_atomic_store(&mem_trace.peak_mem, total_mem);
        os_atomic_inc(&mem_trace.malloc_count);
    }
#endif

#ifdef DB_MEMDEBUG
    init_mem(rptr, size);
#elif defined(DB_MEMTRACE)
    set_rptr_size( rptr, size );

    os_memory_account(size, 0);
#endif

    DB_ASSERT( (uintptr_t) rptr2uptr( rptr ) % sizeof(db_AlignType) == 0 );

    DB_TRACE_OUTPUT(( file, line, DB_TRACE_HEAP, "%lu: malloc(%d): %p", opno, size, rptr ));

    return rptr2uptr( rptr );
}

void * _db_realloc(void * orig_ptr, db_size_t size, const char * file, int line, int block_kind)
{
    void * rptr = orig_ptr ? uptr2rptr( orig_ptr ) : NULL;
    void * resptr;

#ifdef DB_MEMTRACE
    size_t block_size = (rptr) ? get_rptr_size( rptr ) : 0;
#endif

    unsigned long opno = check_request();

    if (!allocate) {
        set_db_error( DB_ESTATE );
        return NULL;
    }

#ifdef DB_DEBUG
    if (check_alloc_fail())
        return NULL;
#endif

    FS_THROW_RES( FS_MALLOC, (void*)NULL );

#ifdef DB_MEMDEBUG
    if ( rptr )
        check_mem( rptr );

    if (size == 0 && orig_ptr != NULL)
        free_mem(rptr);
#endif

    resptr = real_realloc( rptr, size + OVERHEAD, file, line, block_kind, opno );

    if (resptr == NULL && size > 0) {
		set_db_error(DB_ENOMEM);
        return NULL;
    }

#ifdef DB_MEMTRACE
    {
        os_atomic_t total_mem = os_atomic_add(&mem_trace.total_mem, size);
        /* note: peak_mem is not calculated atomically because slight inaccuracy is permissible. */
        if(total_mem > os_atomic_fetch(&mem_trace.peak_mem))
            os_atomic_store(&mem_trace.peak_mem, total_mem);
    }

    os_atomic_add(&mem_trace.total_mem, -(os_atomic_t)block_size);
    if (size == 0 && orig_ptr != NULL)
        os_atomic_inc(&mem_trace.free_count);
    else if (orig_ptr == NULL && size > 0)
        os_atomic_inc(&mem_trace.malloc_count);
#endif

    if (resptr == NULL)
        return NULL;

    /* check alignment */
    DB_ASSERT( (uintptr_t) resptr % sizeof(db_AlignType) == 0 );

#ifdef DB_MEMDEBUG
    init_mem(resptr, size);
#elif defined(DB_MEMTRACE)
    set_rptr_size( resptr, size );
    os_memory_account(size, block_size);
#endif

    DB_ASSERT( (uintptr_t) rptr2uptr( resptr ) % sizeof(db_AlignType) == 0 );

    DB_TRACE_OUTPUT(( file, line, DB_TRACE_HEAP, "%lu: remalloc(%p, %d): %p", opno, rptr, size, resptr ));

    return rptr2uptr(resptr);
}

void * _db_calloc( db_size_t n, db_size_t size, const char * file, int line, int block_kind)
{
    const db_size_t actual_size = n * size;
    if (n != 0 && actual_size / n != size) {
        // Memory allocation fails if multiplication resulted in an integer overflow.
		set_db_error(DB_ENOMEM);
        return NULL;
    }

    void * ptr = _db_malloc(actual_size, file, line, block_kind);
    return NULL == ptr ? NULL : memset(ptr, 0, actual_size);
}


void _db_free(void * ptr, const char * file, int line, int block_kind)
{
    void * rptr;

    unsigned long opno = check_request();

    if (ptr == NULL || !allocate)
        return;

    rptr = uptr2rptr(ptr);
    
    DB_TRACE_OUTPUT(( file, line, DB_TRACE_HEAP, "%lu: free(%p)", opno, rptr ));

#ifdef DB_MEMTRACE
    os_atomic_add(&mem_trace.total_mem, -(os_atomic_t)get_rptr_size( rptr ));
    os_atomic_inc(&mem_trace.free_count);

    os_memory_account(0, get_rptr_size( rptr ));
#endif

#ifdef DB_MEMDEBUG
    free_mem(rptr);
#endif

    real_free( rptr, file, line, block_kind, opno );
}

#ifdef DB_MEMTRACE

db_mem_trace_t db_get_mem_trace(void)
{
    db_mem_trace_t result;

    /* note: since these are not a single atomic operation, results may be slightly inconsistent. */
    result.total_mem = os_atomic_fetch(&mem_trace.total_mem);
    result.peak_mem = os_atomic_fetch(&mem_trace.peak_mem);
    result.halloc_mem = os_atomic_fetch(&mem_trace.halloc_mem);
    result.malloc_count = os_atomic_fetch(&mem_trace.malloc_count);
    result.free_count = os_atomic_fetch(&mem_trace.free_count);

    return result;
}

void db_reset_mem_trace(void)
{
    os_atomic_store(&mem_trace.total_mem, 0);
    os_atomic_store(&mem_trace.peak_mem, 0);
    os_atomic_store(&mem_trace.halloc_mem, 0);
    os_atomic_store(&mem_trace.malloc_count, 0);
    os_atomic_store(&mem_trace.free_count, 0);
}

long os_memory_used(void)
{
    return os_atomic_fetch( &cur_memory_count );
}

long os_hmemory_used(void)
{
    return os_atomic_fetch( &cur_hmemory_count );
}

#endif /* DB_MEMTRACE */

void * _db_memdup(void * p, db_size_t size, const char * file, int line, int block_kind)
{
    void * ptr;

    unsigned long opno = check_request();

    if (p == NULL)
        return NULL;

    if ((ptr = real_malloc(size, file, line, block_kind, opno)) == NULL)
        return NULL;

    memcpy(ptr, p, size);
    return ptr;
}
   
#ifdef OS_WIN32
/* huge memory allocation */
#include "ittia/os/std/windows.h"

void * db_hmalloc(db_size_t size)
{
    LPVOID ptr;

    ptr = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
    if (ptr == NULL)
        return os_win32_error(), NULL;

#ifdef DB_MEMTRACE
    os_atomic_add(&mem_trace.halloc_mem, size);
    {
        os_atomic_t total_mem = os_atomic_add(&mem_trace.total_mem, size);
        /* note: peak_mem is not calculated atomically because slight inaccuracy is permissible. */
        if(total_mem > os_atomic_fetch(&mem_trace.peak_mem))
            os_atomic_store(&mem_trace.peak_mem, total_mem);
    }

    os_hmemory_account(size, 0);
#endif

    return ptr;
}

dbstatus_t db_hfree(void * ptr, db_size_t size)
{
    DB_UNUSED(size);

/*    if (!VirtualFree(ptr, size, MEM_DECOMMIT))
        return os_win32_error();
*/
    if (!VirtualFree(ptr, 0, MEM_RELEASE))
        return os_win32_error();

#ifdef DB_MEMTRACE
    if (ptr) {
        os_atomic_add(&mem_trace.halloc_mem, -(os_atomic_t)size);
        os_atomic_add(&mem_trace.total_mem, -(os_atomic_t)size);
        os_hmemory_account(0, size);
    }
#endif

    return DB_NOERROR;
}

#else
/** @todo implement hmalloc for non-windows platforms */

#ifdef HMALLOC_SEGMENT_SIZE
// Simple huge memory allocator with almost zero overhead, suitable for
// applications that never free huge allocations or always free all huge
// allocations at once.
static int32_t hmalloc_count = 0;
#ifndef HMALLOC_SEGMENT_BASE
#define HMALLOC_SEGMENT_BASE &hmalloc_segment
static uint32_t hmalloc_segment[HMALLOC_SEGMENT_SIZE / sizeof(uint32_t)];
#endif
static char * hmalloc_top = (char*)(HMALLOC_SEGMENT_BASE);
#endif

void * db_hmalloc(db_size_t size)
{
#ifdef HMALLOC_SEGMENT_SIZE
    void * p;

    if (hmalloc_top + size > (char*)(HMALLOC_SEGMENT_BASE) + HMALLOC_SEGMENT_SIZE) {
        DB_SET_ERROR( DB_ENOMEM );
        return NULL;
    }

    // Allocate from the top of the segment.
    p = hmalloc_top;
    hmalloc_top += size;
    ++hmalloc_count;
#else
    void * p = malloc(size);
    if (p == NULL) {
        set_db_error(DB_ENOMEM);
        return NULL;
    }
#endif

#ifdef DB_MEMTRACE
    if (p != NULL) {
        os_atomic_add(&mem_trace.halloc_mem, size);
        os_atomic_t total_mem = os_atomic_add(&mem_trace.total_mem, size);
        /* note: peak_mem is not calculated atomically because slight inaccuracy is permissible. */
        if(total_mem > os_atomic_fetch(&mem_trace.peak_mem))
            os_atomic_store(&mem_trace.peak_mem, total_mem);

        os_hmemory_account(size, 0);
    }
#endif

    return p;
}

int db_hfree(void * ptr, db_size_t size)
{
    if (ptr != NULL) {
#ifdef HMALLOC_SEGMENT_BASE
        --hmalloc_count;
        if (hmalloc_count == 0) {
            // Reclaim memory only when the entire segment is free.
            hmalloc_top = (char*)(HMALLOC_SEGMENT_BASE);
        }
#else
        free(ptr);
#endif

#ifdef DB_MEMTRACE
        os_atomic_add(&mem_trace.halloc_mem, -(os_atomic_t)size);
        os_atomic_add(&mem_trace.total_mem, -(os_atomic_t)size);
        os_hmemory_account(0, size);
#endif
    }

    return DB_NOERROR;
}

#endif

#if defined(DB_DEBUG) && defined(OS_MSC)
#ifndef OS_WINCE
#   include <crtdbg.h>
#endif
void * db_validate_pointer(void * p, int null_ok)
{
#ifndef OS_WINCE
    if (p == NULL)
        DB_ASSERT( null_ok );
    else
        DB_ASSERT( _CrtIsValidHeapPointer(p) );
#endif
    return p;
}

#endif

#if defined(DB_DEBUG) && defined(OS_MSC)
void db_check_mem(void)
{
#ifndef OS_WINCE
    DB_ASSERT( _CrtCheckMemory() );
#endif
}

#endif
