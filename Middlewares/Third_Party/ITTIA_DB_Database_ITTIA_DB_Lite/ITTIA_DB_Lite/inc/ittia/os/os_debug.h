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
 * Debugging features. Functions and macros defined in this file are only
 * active when DB_DEBUG is defined. DB_DEBUG should only be defined when
 * linking against the debug version of the ITTIA DB library.
 */

#ifndef OS_DEBUG_H
#define OS_DEBUG_H

#include "ittia/os/os_config.h"
#include "ittia/os/os_error.h"
#include "ittia/os/os_dump.h"

/** @returns true only if p implies q */
#define IMPLY(p,q) (!(p) || (q))

/** @returns true only if p implies q and q implies p*/
#define EQUI(p,q) (IMPLY(p,q) && IMPLY(q,p))

/** Mark a function parameter as unused to prevent compiler warnings. */
#define DB_UNUSED(x)  ((void)&(x))

#ifdef DB_DEBUG

#ifndef CLANG_ANALYZER_NORETURN
#ifndef __has_feature
#define __has_feature(x) 0
#endif
#if __has_feature(attribute_analyzer_noreturn)
#define CLANG_ANALYZER_NORETURN __attribute__((analyzer_noreturn))
#else
#define CLANG_ANALYZER_NORETURN
#endif
#endif

C_LINKAGE DBDLL_API void os_dbg_break(const char * file, int line, const char *, ...) CLANG_ANALYZER_NORETURN;

C_LINKAGE typedef void (DB_CALLBACK * os_dbg_break_callback_t)(void * object, const char * file, int line, const char * fmt, va_list va);
C_LINKAGE DBDLL_API void os_dbg_break_set_callback(os_dbg_break_callback_t callback, void * object);

/* debug version */
#define DB_REQUIRE(p,e)    do { if (!(p)) { DB_TRACE_OUTPUT(( __FILE__, __LINE__, -1, "require failed: %s", #p); os_dbg_break(__FILE__, __LINE__, "validate failed\n\t%s\n", #p)); return e; } } while (false)
#ifndef OS_VXWORKS_55
#   define DB_ASSERT(p)       do { if (!(p)) { DB_TRACE_OUTPUT(( __FILE__, __LINE__, -1, "assertion failed: %s", #p); os_dbg_break(__FILE__, __LINE__, "assertion failed\n\t%s\n", #p)); } } while (false)
#else
#   include <stdio.h>
#   include <stdlib.h>
#   define DB_ASSERT(p)       ((p) ? (void)0 : printf(#p "\n"))
#endif


#define DB_VERIFY(x)       DB_ASSERT(x)
#define DB_IN_DEBUG(x)     x
#define DB_VERIFY_RETURN(x, y) ((x) = (y))

#define DB_DBG_BREAK(reason) os_dbg_break(__FILE__, __LINE__, reason)

#ifdef __cplusplus

#   define DB_ASSERT_MSG(p, msg)     do { int __p = int(p); if (!__p) { dbdump(-1) << "assertion failed: " << #p << ": " << msg << '\n'; os_dbg_break(__FILE__, __LINE__, "assertion failed:\n%s", #p); }} while (false)
#   define DB_REQUIRE_MSG(p, e, msg) do { int __p = int(p); if (!__p) { dbdump(-1) << "require failed: "   << #p << ": " << msg << '\n'; os_dbg_break(__FILE__, __LINE__, "requirement failed:\n%s", #p); return e; }} while (false)
#endif

struct db_ResCounters
{
	long num_files;	/* number of files opened */
	long num_sockets;
	long num_mutexes;
	long num_sems;
	long num_rwlocks;
	long num_condvars;
    long num_barriers;
    long num_fastlocks;
	long num_memory;
    long num_hmemory;
};

C_LINKAGE void _db_get_rescounters(struct db_ResCounters *);
C_LINKAGE int _db_compare_rescounters(const struct db_ResCounters *, const struct db_ResCounters *);
C_LINKAGE int _db_check_resleaks(const struct db_ResCounters *);
C_LINKAGE int _db_inspect_frame(int, void ** guard_frame, int guard_size);
C_LINKAGE int _db_backtrace(void ** buffer, int size);

#else /* !DB_DEBUG */

/* release version */

#define DB_REQUIRE(p,e)  do { if (!(p)) return e; } while (false)
#define DB_ASSERT(p)     ((void)0)
#define DB_VERIFY(x)     (void)(x)
#define DB_IN_DEBUG(x)   ((void)0)
#define DB_VERIFY_RETURN(x, y) ((void)y)

#define DB_DBG_BREAK(reason) ((void)0)

#ifdef __cplusplus
#   define DB_ASSERT_MSG(p, msg)     DB_ASSERT( p )
#   define DB_REQUIRE_MSG(p, e, msg) DB_REQUIRE(p, e)
#endif

#endif /* !DB_DEBUG */

#define DB_VALIDATE(p,e) do { if (!(p)) return e; } while (false)

#ifdef __cplusplus

//template <class A> inline void db_same_type(A,A) {}
//#define DB_SAME_TYPE(A,B) db_same_type((A*)0, (B*)0)

#endif /* __cplusplus */

/* portability with existing code */

#define THROW_ERROR(x) do { int __rc__; if (DB_FAILED(__rc__ = (int)(x))) return __rc__; } while (false)
#define THROW_ERROR_TO(x, lab) do { int __rc__ = (x); if (DB_FAILED(__rc__)) goto lab; } while (false)

#if defined(HAVE_THREADS) && defined(DB_DEBUG)
DBDLL_API void os_debug_yield(void);
DBDLL_API void os_debug_ryield(void);
#else
#   define os_debug_yield() (void)0
#   define os_debug_ryield() (void)0
#endif

#endif /* OS_DEBUG_H */
