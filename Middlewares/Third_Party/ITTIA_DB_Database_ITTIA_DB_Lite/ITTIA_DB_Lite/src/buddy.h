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

#ifndef BUDDY_H
#define BUDDY_H

#define DB_NEED_BUILTIN_ALLOCATOR

#include "ittia/os/os_config.h"
#include "ittia/os/os_malloc.h"

C_HEADER_BEGIN

#define BUDDY_SEGMENTS_MAX      _DB_MEMSEGMENT_MAX

typedef struct Buddy Buddy;

#if 0
typedef struct
{
    double nfail;       /* number of failures due to absence of memory */
    double nmalloc;     /* number of successfull mallocs */
    double nrealloc;    /* number of successfull reallocs */
    double nfree;       /* number of successfull frees */

    size_t nreq_cur;
    size_t nreq_max;
    double nreq_all;
    
    size_t sys_cur;
    size_t sys_max;
    double sys_all;
    
    size_t user_cur;
    size_t user_max;
    double user_all;

    size_t num_segs;
    size_t page_size;
    size_t heap_size;
//    size_t free_pages;

} BuddyStats;

typedef struct
{
	void * ptr;
    size_t blocks;
} BuddySegment;

#else

typedef _db_MemSegment BuddySegment;
typedef _db_MemStatistics BuddyStats;

#endif

#define DBG_CHECK_FREE          1
#define DBG_VALIDATE_ALWAYS     2
#define DBG_VALIDATE_LEAVE      4
#define DBG_DUMP_LEAKS          8

Buddy * buddy_init(Buddy *, size_t block_size, const BuddySegment *, size_t nsegments, unsigned flags);
void buddy_destroy( Buddy * );
void buddy_dump_leaks( Buddy *);

unsigned buddy_set_debug_flags(Buddy *, unsigned);

void * _buddy_malloc(Buddy *, size_t size, size_t * real_size, const char * file, int lineno, int block_kind, unsigned long opno);
void * _buddy_realloc(Buddy *, void *, size_t size, size_t * real_size, const char * file, int lineno, int block_kind, unsigned long opno);
void _buddy_free(Buddy *, void * , const char * file, int lineno, int block_kind, unsigned long opno);

#define buddy_malloc(heap, size, real_size, block_kind, opno) _buddy_malloc(heap, size, real_size, __FILE__, __LINE__, block_kind, opno)
#define buddy_realloc(heap, ptr, size, real_size, block_kind, opno) _buddy_realloc(heap, ptr, size, real_size, __FILE__, __LINE__, block_kind, opno)
#define buddy_free(heap, ptr, block_kind, opno) _buddy_free(heap, ptr, __FILE__, __LINE__, block_kind, opno)

int buddy_get_stats(Buddy *, BuddyStats*, int reset );
void _buddy_check_heap(Buddy *);

/* void _buddy_check_pointer(Buddy *); */

#ifdef NDEBUG
#   define buddy_check_heap(heap)  ((void)0)
#else
#   define buddy_check_heap(heap)  _buddy_check_heap(heap)
#endif

/* propose the optimal buffer adjustment.
 * Returns count if items such as: result >= min_count && result <= max_count
 * result is as close to max_count as possible.
 * the count is optimal in terms of space wastage
 * if you allocate this numberof items, the space waste would be 
 * minimal within given boundaries.
 *
 * Interpretation of arguments is as follows:
 *
 * Item * buffer = NULL, cur_item;
 * size_t count = 0;
 * size_t used = 0;
 *
 * while( get_next( &cur_item ) ) {
 *     if (used == count) {
 *         size_t req = buddy_adjust( heap, sizeof(Item), used + 1, used * 2);
 *         buffer = buddy_realloc( heap, buffer, sizeof(Item) * req, NULL );
 *         count = req;
 *     }
 *     buffer[used++] = cur_item;
 * }
 *
 * in the example above buffer grows almost exponentially, but respects
 * memory boundaries.c
 */

size_t buddy_adjust(Buddy *, size_t item_size, size_t min_count, size_t max_count);

C_HEADER_END

#endif /* BUDDY_H */
