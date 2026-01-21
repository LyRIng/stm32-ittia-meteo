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
 * Dump stream for debugging output. When #DB_DEBUG is defined, this file
 * provides control over debugging output.
 *
 * @see DB_ERROR, DB_OUTPUT, DB_NEWLINE
 */

#ifndef OS_DUMP_H
#define OS_DUMP_H

#include "ittia/os/os_config.h"

#include <stdarg.h>

#ifdef DB_DEBUG

/** @name Debug Facilities
 * @{ */

/* Note: These should have match with ittia/db/db.h */
#define DB_TRACE_USERMSG    0x00000001u  /**< User message debug facility. unused */
#define DB_TRACE_BUFMGR     0x00000002u  /**< Buffer manager debug facility */
#define DB_TRACE_LOCKMGR    0x00000004u  /**< Lock manager debug facility */
#define DB_TRACE_LOGMGR     0x00000008u  /**< Log manager debug facility */
#define DB_TRACE_SPACEMGR   0x00000010u  /**< Space manager debug facility */
#define DB_TRACE_TXMGR      0x00000020u  /**< Transaction manager debug facility */
#define DB_TRACE_CATALOG    0x00000040u  /**< Catalog debug facility */
#define DB_TRACE_OSLIB      0x00000080u  /**< OS layer debug facility. unused */
#define DB_TRACE_CAPI       0x00000100u  /**< C API debug facility */
#define DB_TRACE_DUMP       0x00000200u  /**< General message debug facility. unused */
#define DB_TRACE_MEMTABLE   0x00000400u  /**< MemStorage component */
#define DB_TRACE_REP        0x00000800u  /**< Replication component */
#define DB_TRACE_BTREE      0x00001000u  /**< Btree component */
#define DB_TRACE_HEAP       0x00002000u  /**< memory allocations */
#define DB_TRACE_DISKTABLE  0x00004000u  /**< FileStorage component */
#define DB_TRACE_SQL        0x00008000u  /**< SQL component */
#define DB_TRACE_IPC        0x00010000u  /**< IPC client/server component */
#define DB_TRACE_JOURNAL    0x00020000u  /**< Journal rotation and archiving */
#define DB_TRACE_BTREE_TRAV 0x00040000u  /**< Btree component traversal */
#define DB_TRACE_STREAM     0x00080000u  /**< Stream processing */
#define DB_TRACE_LSS        0x00100000u  /**< Log structured storage */
#define DB_TRACE_MAX        0x70000000u  /**< max possible trace facility */
#define DB_TRACE_ALL        0x7FFFFFFFu  /**< all facilities included */

/** @} */

C_HEADER_BEGIN

/** @name Debug Output Configuration
 * @{ */

/** Output debug info to the given file. @note #DB_DEBUG must be defined. */
DBDLL_API int  os_debug_file(const char *);

/** Turn debugging facility on or off. @note #DB_DEBUG must be defined. */
DBDLL_API uint32_t os_debug_set(uint32_t enable, uint32_t disable);

/** Check if @c facility is being traced. @note #DB_DEBUG must be defined. */
DBDLL_API int os_debug_traced(uint32_t facility);

/** @} */

/** Output debug message. @note #DB_DEBUG must be defined. */
DBDLL_API void os_trace_output(const char * file, int line, int32_t facility, const char * msg, ... );
/** Output debug message. @note #DB_DEBUG must be defined. */
DBDLL_API void os_trace_outputv(const char * file, int line, int32_t facility, const char * msg, va_list va );

C_HEADER_END

#ifdef __cplusplus

/*
  #include <wchar.h>
*/

const char newline = '\n';
const char tab     = '\t';

enum DumpStyle
{
    DUMP_HEX, DUMP_ASCII, DUMP_HEXA
};

/** Debugging output dump stream. @note DB_DEBUG must be defined. */
class DBDLL_API DumpStream
{
public:
    int visible;

    DumpStream();
    DumpStream(const DumpStream&);
    ~DumpStream();

    DumpStream& operator=(const DumpStream&);

    DumpStream& output(char);

    DumpStream& output(const char * s);
    DumpStream& output(const signed char * s);
    DumpStream& output(const unsigned char * s);

    DumpStream& output(const void * s, int width, int base, int fill = ' ');

    DumpStream& output(const wchar_t *);

    DumpStream& output(signed char, int width = 0, int base = 0, int fill = ' ');
    DumpStream& output(unsigned char, int width = 0, int base = 0, int fill = ' ');

    DumpStream& output(signed short, int width = 0, int base = 10, int fill = ' ');
    DumpStream& output(unsigned short, int width = 0, int base = 10, int fill = ' ');
    DumpStream& output(signed int, int width = 0, int base = 10, int fill = ' ');
    DumpStream& output(unsigned int, int width = 0, int base = 10, int fill = ' ');
    DumpStream& output(signed long, int width = 0, int base = 10, int fill = ' ');
    DumpStream& output(unsigned long, int width = 0, int base = 10, int fill = ' ');
#ifdef HAVE_LONGLONG
    DumpStream& output(longlong_t, int width = 0, int base = 10, int fill = ' ');
    DumpStream& output(ulonglong_t, int width = 0, int base = 10, int fill = ' ');
#endif

    DumpStream& output(float);
    DumpStream& output(double);
    DumpStream& output(bool);

    DumpStream& output(const void *);

    DumpStream& dump(const void *, int, DumpStyle);

    DumpStream& ansistr(const char *, int len);
    DumpStream& utf8str(const uint8_t *, int len);
    DumpStream& utf16str(const uint16_t *, int len);
    DumpStream& utf32str(const uint32_t *, int len);

    void flush();

    DumpStream& nc() const { return *const_cast<DumpStream*>(this); }
};

inline DumpStream& operator << (DumpStream& os, const char * value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, const signed char * value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, const unsigned char * value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, char value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, signed char value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, unsigned char value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, signed short value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, unsigned short value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, signed int value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, unsigned int value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, signed long value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, unsigned long value) { return os.output(value); }
#ifdef HAVE_LONGLONG
inline DumpStream& operator << (DumpStream& os, longlong_t value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, ulonglong_t value) { return os.output(value); }
#endif
inline DumpStream& operator << (DumpStream& os, float value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, double value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, bool value) { return os.output(value); }
inline DumpStream& operator << (DumpStream& os, const void * value) { return os.output(value); }

/** @cond ignore */

/* Output hex code. */
template <class T>
struct _int
{
    T value;
    int width;
    int base;
    int fill;

    _int(T _value, int _width, int _base, int _fill)
        : value(_value), width(_width), base(_base), fill(_fill) {}
};

/* strings */
struct _ansistr
{
    const char * ptr;
    int size;

    _ansistr(const char * _ptr, int _size) : ptr(_ptr), size(_size) {}
};

struct _utf8str
{
    const uint8_t * ptr;
    int size;

    _utf8str(const uint8_t * _ptr, int _size) : ptr(_ptr), size(_size) {}
};

struct _utf16str
{
    const uint16_t * ptr;
    int size;

    _utf16str(const uint16_t * _ptr, int _size) : ptr(_ptr), size(_size) {}
};

struct _utf32str
{
    const uint32_t * ptr;
    int size;

    _utf32str(const uint32_t * _ptr, int _size) : ptr(_ptr), size(_size) {}
};

struct _dump
{
    const void * ptr;
    int size;
    DumpStyle style;

    _dump(const void * _ptr, int _size, DumpStyle _style) : ptr(_ptr), size(_size), style(_style) {}
};

template <class T>
inline DumpStream& operator<<(DumpStream& os, const _int<T>& w) { return os.output(w.value, w.width, w.base, w.fill); }

inline _int<unsigned char> hexw(signed char value, int width = sizeof(signed char) * 2)     { return _int<unsigned char>(value, width, 16, '0'); }
inline _int<unsigned char> hexw(unsigned char value, int width = sizeof(unsigned char) * 2) { return _int<unsigned char>(value, width, 16, '0'); }

inline _int<unsigned short> hexw(signed short value, int width = sizeof(signed short) * 2)     { return _int<unsigned short>(value, width, 16, '0'); }
inline _int<unsigned short> hexw(unsigned short value, int width = sizeof(unsigned short) * 2) { return _int<unsigned short>(value, width, 16, '0'); }

inline _int<unsigned int> hexw(signed int value, int width = sizeof(signed int) * 2)     { return _int<unsigned int>(value, width, 16, '0'); }
inline _int<unsigned int> hexw(unsigned int value, int width = sizeof(unsigned int) * 2) { return _int<unsigned int>(value, width, 16, '0'); }

inline _int<unsigned long> hexw(signed long value, int width = sizeof(signed long) * 2)     { return _int<unsigned long>(value, width, 16, '0'); }
inline _int<unsigned long> hexw(unsigned long value, int width = sizeof(unsigned long) * 2) { return _int<unsigned long>(value, width, 16, '0'); }

#ifdef HAVE_LONGLONG
inline _int<ulonglong_t> hexw(longlong_t value, int width = sizeof(longlong_t) * 2)   { return _int<ulonglong_t>(value, width, 16, '0'); }
inline _int<ulonglong_t> hexw(ulonglong_t value, int width = sizeof(ulonglong_t) * 2) { return _int<ulonglong_t>(value, width, 16, '0'); }
#endif

inline _int<const void *> hexw(const void * value, int width = sizeof(const void *) * 2) { return _int<const void *>(value, width, 16, '0'); }

inline _int<signed char> intw(signed char value, int width = 0, int base = 10, int fill = ' ') { return _int<signed char>(value, width, base, fill); }
inline _int<unsigned char> intw(unsigned char value, int width = 0, int base = 10, int fill = ' ') { return _int<unsigned char>(value, width, base, fill); }

inline _int<signed short> intw(signed short value, int width = 0, int base = 10, int fill = ' ') { return _int<signed short>(value, width, base, fill); }
inline _int<unsigned short> intw(unsigned short value, int width = 0, int base = 10, int fill = ' ') { return _int<unsigned short>(value, width, base, fill); }

inline _int<signed int> intw(signed int value, int width = 0, int base = 10, int fill = ' ') { return _int<signed int>(value, width, base, fill); }
inline _int<unsigned int> intw(unsigned int value, int width = 0, int base = 10, int fill = ' ') { return _int<unsigned int>(value, width, base, fill); }

inline _int<signed long> intw(signed long value, int width = 0, int base = 10, int fill = ' ') { return _int<signed long>(value, width, base, fill); }
inline _int<unsigned long> intw(unsigned long value, int width = 0, int base = 10, int fill = ' ') { return _int<unsigned long>(value, width, base, fill); }

#ifdef HAVE_LONGLONG
inline _int<longlong_t> intw(longlong_t value, int width = 0, int base = 10, int fill = ' ') { return _int<longlong_t>(value, width, base, fill); }
inline _int<ulonglong_t> intw(ulonglong_t value, int width = 0, int base = 10, int fill = ' ') { return _int<ulonglong_t>(value, width, base, fill); }
#endif

inline DumpStream& operator<<(DumpStream& os, const _dump& value) { return os.dump(value.ptr, value.size, value.style); }
inline _dump dump_mem(const void * ptr, int size, DumpStyle style = DUMP_HEXA) { return _dump(ptr, size, style); }

inline DumpStream& operator<<(DumpStream& os, const _ansistr& value) { return os.ansistr(value.ptr, value.size); }
inline DumpStream& operator<<(DumpStream& os, const _utf8str& value) { return os.utf8str(value.ptr, value.size); }
inline DumpStream& operator<<(DumpStream& os, const _utf16str& value) { return os.utf16str(value.ptr, value.size); }
inline DumpStream& operator<<(DumpStream& os, const _utf32str& value) { return os.utf32str(value.ptr, value.size); }

/** @endcond */

/** Output a debug message. Do not call directly. @see DB_ERROR, DB_OUTPUT */
DBDLL_API DumpStream db_dbgmsg(const char * file, int line, int32_t);

/** Output a debug message. Do not call directly. @see DB_ERROR, DB_OUTPUT */
#define dbdump(fc) (db_dbgmsg(__FILE__, __LINE__, fc).nc())

/** @name Debug Message Output
 * @{ */
/** Log an error message. Ignored when #DB_DEBUG is not defined. */
#define DB_ERROR(msg)             do { dbdump(-1) << msg << '\n'; } while (false)
/** Output an error message to fc. Ignored when #DB_DEBUG is not defined. */
#define DB_OUTPUT(fc,msg)         do { dbdump(fc) << msg << '\n'; } while (false)
/** Output a newline to fc. Ignored when #DB_DEBUG is not defined. */
#define DB_NEWLINE(fc)            do { (db_dbgmsg(NULL, 0, fc).nc()) << '\n'; } while (false)
/**  @} */

//#undef DB_NC_STREAM

#endif /* __cplusplus */

#define DB_TRACE_OUTPUT(x) os_trace_output x
#define DB_TRACE_OUT(facility, ...) os_trace_output(__FILE__, __LINE__, facility, __VA_ARGS__)

#else /* !DB_DEBUG */

#ifdef __cplusplus

#   define DB_ERROR(msg)            do {} while (false)
#   define DB_OUTPUT(fc,msg)        do {} while (false)
#   define DB_NEWLINE(fc)           do {} while (false)

#endif

#define DB_TRACE_OUTPUT(x)
#define DB_TRACE_OUT(facility, ...)

#endif /* !DB_DEBUG */

/** Output message to debug console. In regilar circumstances should
 * be used to report serious errors and per user request only
 */
C_LINKAGE DBDLL_API void DB_CDECL os_dbg_printf(const char *, ...);
C_LINKAGE DBDLL_API void DB_CDECL os_dbg_vprintf(const char *, va_list);

#endif /* OS_DUMP_H */
