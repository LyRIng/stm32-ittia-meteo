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

#ifndef OS_NLS_H
#define OS_NLS_H

#include "ittia/os/os_config.h"

C_HEADER_BEGIN

/* db_wchar_to_utfXX: transforn uniform db wchar into corresponding
 * encoding.
 * @returns number of items consumed.
 * when character is wrong (not representable) or there is not enough
 * space in the buffer left, returns 0.
 */

typedef int32_t db_wchar_t;

enum
{
    DB_NLS_WCHAR,
    DB_NLS_ANSI,
    DB_NLS_UTF8,
    DB_NLS_UTF16,
    DB_NLS_UTF16LE,
    DB_NLS_UTF16BE,
    DB_NLS_UTF32,
    DB_NLS_UTF32LE,
    DB_NLS_UTF32BE
};

#ifndef DB_EXCLUDE_UNICODE

/* max size of ANSI multibyte character (bytes) */
int     db_mb_maxsize(void);
#define DB_UTF8_MAX_LEN 5

DBDLL_API int  db_nls_putansi   (char     * buf, int buf_size, db_wchar_t c);
DBDLL_API int  db_nls_putwchar  (wchar_t  * buf, int buf_size, db_wchar_t c);
DBDLL_API int  db_nls_pututf8   (uint8_t  * buf, int buf_size, db_wchar_t c);
DBDLL_API int  db_nls_pututf16  (uint16_t * buf, int buf_size, db_wchar_t c);
DBDLL_API int  db_nls_pututf16be(uint16_t * buf, int buf_size, db_wchar_t c);
DBDLL_API int  db_nls_pututf16le(uint16_t * buf, int buf_size, db_wchar_t c);
DBDLL_API int  db_nls_pututf32  (uint32_t * buf, int buf_size, db_wchar_t c);
DBDLL_API int  db_nls_pututf32be(uint32_t * buf, int buf_size, db_wchar_t c);
DBDLL_API int  db_nls_pututf32le(uint32_t * buf, int buf_size, db_wchar_t c);

DBDLL_API int  db_nls_getansi   (db_wchar_t *, const char     * buf, int buf_size);
DBDLL_API int  db_nls_getwchar  (db_wchar_t *, const wchar_t  * buf, int buf_size);
DBDLL_API int  db_nls_getutf8   (db_wchar_t *, const uint8_t  * buf, int buf_size);
DBDLL_API int  db_nls_getutf16  (db_wchar_t *, const uint16_t * buf, int buf_size);
DBDLL_API int  db_nls_getutf16be(db_wchar_t *, const uint16_t * buf, int buf_size);
DBDLL_API int  db_nls_getutf16le(db_wchar_t *, const uint16_t * buf, int buf_size);
DBDLL_API int  db_nls_getutf32  (db_wchar_t *, const uint32_t * buf, int buf_size);
DBDLL_API int  db_nls_getutf32be(db_wchar_t *, const uint32_t * buf, int buf_size);
DBDLL_API int  db_nls_getutf32le(db_wchar_t *, const uint32_t * buf, int buf_size);

#endif /* DB_EXCLUDE_UNICODE */

#ifndef DB_EXCLUDE_UNICODE

typedef int (DB_CALLBACK * db_nls_filter_t)(db_wchar_t*, void * ptr);
typedef int (*db_nls_getchar_t)(db_wchar_t * c, const void * buf, int buf_size);
typedef int (*db_nls_putchar_t)(void * buf, int buf_size, db_wchar_t c);

typedef struct db_encinfo_t
{
    int code;
    const char * name;
    int char_size;
    db_nls_getchar_t nls_getchar;
    db_nls_putchar_t nls_putchar;
} db_encinfo_t;

DBDLL_API int db_nls_find_encoding(const char * name);
DBDLL_API int db_nls_convert(int dest_enc, void * dest_ptr, int * dest_pos, int dest_size,
                             int src_enc, const void * src_ptr, int * src_pos, int src_size,
                             db_nls_filter_t, void * filter_arg);

DBDLL_API int db_nls_putbuf(int enc, void * buf, int buf_size, db_wchar_t c);
DBDLL_API int db_nls_getbuf(int enc, db_wchar_t *, const void * buf, int buf_size);
DBDLL_API int db_nls_putstr(int enc, void * buf, int buf_size, db_wchar_t c);
DBDLL_API int db_nls_getstr(int enc, db_wchar_t * c, const void * buf, int buf_size);
DBDLL_API int db_nls_char_size(int enc);

/* returns number of bytes requires to convert buffer from src_enc to dest_enc */
DBDLL_API int db_nls_conv_buf(int src_enc, const void * src_buf, size_t * src_size,
                              int dest_enc, void * dest_buf, size_t * dest_size);

DBDLL_API const db_encinfo_t * db_nls_encinfo(int enc);

#endif /* DB_EXCLUDE_UNICODE */

C_HEADER_END

#endif /* OS_NLS_H */
