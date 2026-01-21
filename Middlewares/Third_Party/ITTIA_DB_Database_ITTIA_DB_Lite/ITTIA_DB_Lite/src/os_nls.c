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

#include "ittia/os/os_config.h"

#ifndef DB_EXCLUDE_UNICODE

#include "ittia/os/os_nls.h"
#include "ittia/os/os_byte_order.h"
#include "ittia/os/os_debug.h"
#include "ittia/os/os_error.h"
#include "ittia/os/std/string.h"
#include "ittia/os/std/types.h"
#include "ittia/os/std/malloc.h"

#include "os/os_mockup.h"

#define VALID_UNICODE(c) ((c >= 0x0000 && c < 0xD800) || (c > 0xDFFF && c <= 0x10FFFF))
#define UTF8_MAX 6

#ifdef OS_WINCE
/* limited support for CE. Assume windows supports DBCS only! */

#include "ittia/os/std/windows.h"

static const CPINFO *
get_cpinfo()
{
	static CPINFO cpinfo;
	static BOOL   init_cpinfo = TRUE;

	if (init_cpinfo) {
		GetCPInfo(CP_ACP, &cpinfo);
		init_cpinfo = FALSE;
	}
	return &cpinfo;
}

#undef MB_CUR_MAX
#define MB_CUR_MAX get_mb_cur_max()

static
int get_mb_cur_max()
{
	return get_cpinfo()->MaxCharSize;
}

static
int wctomb(char * mb, wchar_t wc)
{
	int rc, mbmax;
	BOOL defchar = FALSE;

	if (mb == NULL) /* no state dependency */
		return 0;

	mbmax = MB_CUR_MAX;
	rc = WideCharToMultiByte(CP_ACP,
		0, /** @todo Should WC_COMPOSITECHECK be specified? */
		&wc, 1,
		mb, mbmax,
		NULL, &defchar);

	if (rc == 0 || defchar)
		return -1;

	return rc;
}

static
int mbtowc(wchar_t * wc, const char * mb, size_t count)
{
	int rc;

	if (mb == NULL) /* no state dependency */
		return 0;

	if (IsDBCSLeadByteEx(CP_ACP, *mb)) {
		int mbmax = MB_CUR_MAX;

		/* DBCS only */
		ASSERT(mbmax == 1 || mbmax == 2);

		/* check if number of characters is enough */
		if (count < (size_t)mbmax)
			return -1;

		rc = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			(LPCSTR)mb, mbmax,
			wc, 1);

		if (rc == 0)
			return -1;

		return mbmax;
	} else { /* SBCS */
		rc = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			(LPCSTR)mb, 1, wc, 1);

		if (rc == 0)
			return -1;
		return 1;
	}
}

#else

#ifndef HAVE_WCTOMB
static
int basic_wctomb(char * mb, wchar_t wc)
{
	if (mb == NULL) /* no state dependency */
		return 0;

  /* Copy only 7-bit ANSI characters, verbatim. */
  if (wc < 0x80) {
    *mb = wc;
    return 1;
  }
  else {
    return -1;
  }
}
#define wctomb basic_wctomb
#endif

#ifndef HAVE_MBTOWC
static
int basic_mbtowc(wchar_t * wc, const char * mb, size_t count)
{
	if (mb == NULL) /* no state dependency */
		return 0;

  /* Copy only 7-bit ANSI characters, verbatim. */
  if (*mb < 0x80) {
    *wc = *mb;
    return 1;
  }
  else {
    return -1;
  }
}
#define mbtowc basic_mbtowc
#endif

#endif

#if !defined(DB_POINTER_ALIGNMENT) || (defined(DB_ALIGNMENT) && DB_ALIGNMENT == 1)
/* Pointers are always aligned on this platform - no test is required. */
#define MEM_IS_ALIGNED(ptr, T) 1
#else
#ifdef DB_ALIGNMENT
/* Align to number of bytes specified by DB_ALIGNMENT. */
#define MEM_IS_ALIGNED(ptr, T) (((uintptr_t)(ptr) & (DB_ALIGNMENT - 1)) == 0)
#else
/* Align to number of bytes in data type. */
#define MEM_IS_ALIGNED(ptr, T) (((uintptr_t)(ptr) & (sizeof(T) - 1)) == 0)
#endif
#endif

/**
 * Fetch a value from an unaligned buffer to a variable of given type.
 *
 * @param dest
 *   Destination for assignment (l-value of type T). Expressions with side
 *   effects are allowed for this parameter only.
 * @param source
 *   Pointer to source data.
 * @parama T
 *   Name of type to cast source as.
 */
#ifdef OS_MEMCPY_WILL_ALWAYS_BYTE_COPY
#define MEM_ASSIGN_ALIGN_SOURCE(dest, source, T) \
    do { \
        if (MEM_IS_ALIGNED(source, T)) \
            (dest) = *(T*)(source); \
        else \
            memcpy(&(dest), (source), sizeof(T)); \
    } while (0)
#else
#define MEM_ASSIGN_ALIGN_SOURCE(dest, source, T) \
    do { \
        if (sizeof(T) == 1 || MEM_IS_ALIGNED(source, T)) \
            (dest) = *(T*)(source); \
        else \
        switch (sizeof(T)) { \
            case 2: \
                ((char*)&(dest))[0] = ((char*)(source))[0]; \
                ((char*)&(dest))[1] = ((char*)(source))[1]; \
                break; \
            case 4: \
                ((char*)&(dest))[0] = ((char*)(source))[0]; \
                ((char*)&(dest))[1] = ((char*)(source))[1]; \
                ((char*)&(dest))[2] = ((char*)(source))[2]; \
                ((char*)&(dest))[3] = ((char*)(source))[3]; \
                break; \
            default: \
                memcpy(&(dest), (source), sizeof(T)); \
        } \
    } while (0)
#endif

/**
 * Store a value of given type to an unaligned buffer.
 * 
 * @param dest
 *   Pointer to destination.
 * @param source
 *   Value to assign (of type T). Expressions with side effects are
 *   allowed for this parameter only.
 * @parama T
 *   Name of type to cast dest as.
 */
#ifdef OS_MEMCPY_WILL_ALWAYS_BYTE_COPY
#define MEM_ASSIGN_ALIGN_DEST(dest, source, T) \
    do { \
        if (MEM_IS_ALIGNED(dest, T)) \
            *(T*)dest = source; \
        else \
            memcpy(dest, &source, sizeof(T)); \
    } while (0)
#else
#define MEM_ASSIGN_ALIGN_DEST(dest, source, T) \
    do { \
        if (sizeof(T) == 1 || MEM_IS_ALIGNED((dest), T)) \
            *(T*)(dest) = (source); \
        else \
        switch (sizeof(T)) { \
            case 2: \
                ((char*)(dest))[0] = ((char*)&(source))[0]; \
                ((char*)(dest))[1] = ((char*)&(source))[1]; \
                break; \
            case 4: \
                ((char*)(dest))[0] = ((char*)&(source))[0]; \
                ((char*)(dest))[1] = ((char*)&(source))[1]; \
                ((char*)(dest))[2] = ((char*)&(source))[2]; \
                ((char*)(dest))[3] = ((char*)&(source))[3]; \
                break; \
            default: \
                memcpy((dest), &(source), sizeof(T)); \
        } \
    } while (0)
#endif

 static C_INLINE_SPECIFIER void
UTF16LE_WRITE(void * buf, uint16_t x)
{
    uint8_t * p = (uint8_t*)buf;

    p[0] = (uint8_t)(x & 0xffu);
    p[1] = (uint8_t)(x >> 8);
}

static C_INLINE_SPECIFIER uint16_t
UTF16LE_READ(const void * buf)
{
    const uint8_t * p = (const uint8_t*)buf;

    return ((uint16_t) p[0])
        |  ((uint16_t) p[1] << 8)
        ;
}

static C_INLINE_SPECIFIER void
UTF16BE_WRITE(void * buf, uint16_t x)
{
    uint8_t * p = (uint8_t*)buf;

    p[1] = (uint8_t)(x & 0xffu);
    p[0] = (uint8_t)(x >> 8);
}

static C_INLINE_SPECIFIER uint16_t
UTF16BE_READ(const void * buf)
{
    const uint8_t * p = (const uint8_t*)buf;

    return ((uint16_t) p[1])
        |  ((uint16_t) p[0] << 8)
        ;
}

static C_INLINE_SPECIFIER void
UTF32LE_WRITE(void * buf, uint32_t x)
{
    uint8_t * p = (uint8_t*)buf;

    p[0] = (uint8_t)((x)       & 0xffu);
    p[1] = (uint8_t)((x >> 8)  & 0xffu);
    p[2] = (uint8_t)((x >> 16) & 0xffu);
    p[3] = (uint8_t)((x >> 24) & 0xffu);
}

static C_INLINE_SPECIFIER uint32_t
UTF32LE_READ(const void * buf)
{
    const uint8_t * p = (const uint8_t*)buf;

    return ((uint32_t) p[0])
        |  ((uint32_t) p[1] << 8)
        |  ((uint32_t) p[2] << 16)
        |  ((uint32_t) p[3] << 24)
        ;
}

static C_INLINE_SPECIFIER void
UTF32BE_WRITE(void * buf, uint32_t x)
{
    uint8_t * p = (uint8_t*)buf;

    p[3] = (uint8_t)((x)       & 0xffu);
    p[2] = (uint8_t)((x >> 8)  & 0xffu);
    p[1] = (uint8_t)((x >> 16) & 0xffu);
    p[0] = (uint8_t)((x >> 24) & 0xffu);
}

static C_INLINE_SPECIFIER uint32_t
UTF32BE_READ(const void * buf)
{
    const uint8_t * p = (const uint8_t*)buf;

    return ((uint32_t) p[3])
        |  ((uint32_t) p[2] << 8)
        |  ((uint32_t) p[1] << 16)
        |  ((uint32_t) p[0] << 24)
        ;
}



/*********************************************************************/
/* Source: Wikipedia (http://en.wikipedia.org/wiki/UTF-8)
 *
 * Unicode UTF-8 Table
 * 0x00000000 .. 0x0000007F: 0xxxxxxx
 * 0x00000080 .. 0x000007FF: 110xxxxx 10xxxxxx
 * 0x00000800 .. 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
 * 0x00010000 .. 0x001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * Possible, but not the part of standard:
 * 0x00200000 .. 0x03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 * 0x04000000 .. 0x7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 */

DBDLL_API int
db_nls_getutf8(db_wchar_t * c, const uint8_t  * buf, int buf_size)
{
    db_wchar_t c1, c2, c3, c4/*, c5, c6*/;

    if (buf_size < 1)
        return DB_ENLSSPACE;

    c1 = *buf;
    ++buf;
    if (c1 < 0x80) {
        /* single byte sequence */
        *c = c1;
        return 1;
    }

    if (buf_size < 2)
        return DB_EINVCHAR;
    /* valid characters: from 10000000 .. 10111111 */
    c2 = *buf;
    ++buf;
    if (c2 < 0x80u || c2 > 0xBFu) {
        return DB_EINVCHAR;
    }
    c2 &= 0x3f;
    if ((c1 & 0xE0) == 0xC0) {
        /* two byte sequence */
        *c = ((c1 & 0x1f) << 6)
           | c2;

        if (*c < UINT32_C(0x0000080))
            return DB_EINVCHAR;
        return 2;
    }

    /* */
    if (buf_size < 3)
        return DB_EINVCHAR;
    c3 = *buf;
    ++buf;
    if (c3 < 0x80 || c3 > 0xBF) {
        return DB_EINVCHAR;
    }
    c3 &= 0x3f;
    if ((c1 & 0xF0) == 0xE0) {
        /* three byte sequence */
        *c = ((c1 & 0x0f) << 12)
            | (c2 << 6)
            | c3;

        /* code points for overlong encoding and code points reserved for UTF-16
         * surrogate halves are not permitted in any Unicode encoding. */
        if (*c < UINT32_C(0x0000800) || (*c >= UINT32_C(0xD800) && *c <= UINT32_C(0xDFFF))) {
            return DB_EINVCHAR;
        }

        return 3;
    }

    /* */
    if (buf_size < 4)
        return DB_EINVCHAR;
    c4 = *buf;
    /* variable used in later state, false-positive */
    //cppcheck-suppress uselessAssignmentPtrArg
    //cppcheck-suppress unreadVariable 
    ++buf;
    if (c4 < 0x80 || c4 > 0xBF)
        return DB_EINVCHAR;
    c4 &= 0x3f;
    if ((c1 & 0xF8) == 0xF0) {
        /* four byte sequence */
        *c = ((c1 & 0x07) << 18)
            | (c2 << 12)
            | (c3 << 6)
            | c4;
        /* code points for overlong encoding are not permitted in any Unicode encoding. */
        if (*c < UINT32_C(0x0010000))
            return DB_EINVCHAR;
        /* As of November 2003, UTF-8 is restricted by RFC 3629 to end at U+10FFFF. */
        if (*c > UINT32_C(0x010FFFF))
            return DB_EINVCHAR;
        return 4;
    }

#if 0
    /* As of November 2003, UTF-8 is restricted by RFC 3629 to end at U+10FFFF. */
    if (buf_size < 5)
        return DB_EINVCHAR; // DB_ENLSSPACE;
    if (((c5 = *buf++) < 0x80) || c5 > 0xBF)
        return DB_EINVCHAR;
    c5 &= 0x3f;
    if ((c1 & 0xFC) == 0xF8) {
        /* five byte sequence */
        *c = ((c1 & 0x03) << 24)
            | (c2 << 18)
            | (c3 << 12)
            | (c4 << 6)
            | c5;
        if (*c < INT32_C(0x0200000))
            return DB_EINVCHAR;
        return 5;
    }

    /* */
    if (buf_size < 6)
        return DB_EINVCHAR; // DB_ENLSSPACE;
    if (((c6 = *src++) < 0x80) || c6 > 0xBF)
        return DB_EINVCHAR;
    c6 &= 0x3f;
    if ((c1 & 0xFE) == 0xFC) {
        /* six byte sequence */
        *dest = ((c1 & 0x01) << 30)
               | (c2 << 24)
               | (c3 << 18)
               | (c4 << 12)
               | (c5 << 6)
               | c6;
        if (*dest < INT32_C(0x4000000))
            return DB_EINVCHAR;
        return 6;
    }
#endif
    /* bad character */
    return DB_EINVCHAR;
}

DBDLL_API int
db_nls_pututf8(uint8_t  * buf, int buf_size, db_wchar_t c)
{
    int len;

    if (!VALID_UNICODE(c))
        return DB_EINVCHAR;

         if (c < 0x0000080) len = 1;
    else if (c < 0x0000800) len = 2;
    else if (c < 0x0010000) len = 3;
    else if (c < 0x0200000) len = 4;
/*
    else if (c <  0x4000000) len = 5;
    else if (c <= 0x7ffffff) len = 6;
*/
    else
        return DB_EINVCHAR;

    if (buf == NULL)
        return len;

    if (buf_size < len)
        return DB_ENLSSPACE;

    switch(len) {
    case 1:
        *buf = (uint8_t)c;
        //cppcheck-suppress unreadVariable
        ++buf;
        break;

    case 2:
        *buf = 0xC0 | (uint8_t)(c >> 6);
        ++buf;
        *buf = 0x80 | (uint8_t)(c & 0x3F);
        //cppcheck-suppress unreadVariable
        ++buf;
        break;

    case 3:
        *buf = 0xE0 | (uint8_t)((c >> 12));
        ++buf;
        *buf = 0x80 | (uint8_t)((c >>  6) & 0x3F);
        ++buf;
        *buf = 0x80 | (uint8_t)((c      ) & 0x3F);

        /* variable used in later state, false-positive */
        //cppcheck-suppress uselessAssignmentPtrArg
        //cppcheck-suppress unreadVariable 
        ++buf;
        break;

    case 4:
        *buf = 0xF0 | (uint8_t)(c >> 18);
        ++buf;
        *buf = 0x80 | (uint8_t)((c >> 12) & 0x3F);
        ++buf;
        *buf = 0x80 | (uint8_t)((c >>  6) & 0x3F);
        ++buf;
        *buf = 0x80 | (uint8_t)((c      ) & 0x3F);
        /* variable used in later state, false-positive */
        //cppcheck-suppress uselessAssignmentPtrArg
        //cppcheck-suppress unreadVariable 
        ++buf;
        break;
/*
    case 5:
        *buf = 0xF8 | (uint8_t)(c >> 24);
        ++buf;
        *buf = 0x80 | (uint8_t)((c >> 18) & 0x3F);
        ++buf;
        *buf = 0x80 | (uint8_t)((c >> 12) & 0x3F);
        ++buf;
        *buf = 0x80 | (uint8_t)((c >>  6) & 0x3F);
        ++buf;
        *buf = 0x80 | (uint8_t)((c      ) & 0x3F);
        ++buf;
        break;
    case 6
        *buf = 0xFC | (uint8_t) (c >> 30);
        ++buf;
        *buf = 0x80 | (uint8_t)((c >> 24) & 0x3F);
        ++buf;
        *buf = 0x80 | (uint8_t)((c >> 18) & 0x3F);
        ++buf;
        *buf = 0x80 | (uint8_t)((c >> 12) & 0x3F);
        ++buf;
        *buf = 0x80 | (uint8_t)((c >>  6) & 0x3F);
        ++buf;
        *buf = 0x80 | (uint8_t)((c      ) & 0x3F);
        ++buf;
        break;
*/
    }

    return len;
}

/*********************************************************************/
DBDLL_API int
db_nls_pututf16(uint16_t * str, int buf_size, db_wchar_t c)
{
	/* sanity check */
	if (!VALID_UNICODE(c))
		/* not valid unicode char, would break the UTF-16 */
		return DB_EINVCHAR;

	if (c < 0x10000) {
		/* one word representation */
        if (str) {
    		if (buf_size < 1)
                return DB_ENLSSPACE;
		    *str = (uint16_t)c;
        }
		return 1;
	} else {
		/* two words representation */
        if (str) {
    		if (buf_size < 2)
                return DB_ENLSSPACE;
            c -= 0x10000;
            *str = (uint16_t)((c >> 10) + 0xD800);
            ++str;
            *str = (uint16_t)((c & 0x3FF) + 0xDC00);
            
            /* variable used in later state, false-positive */
        //cppcheck-suppress uselessAssignmentPtrArg
            //cppcheck-suppress unreadVariable 
            ++str;
        }
		return 2;
	}
}

DBDLL_API int
db_nls_pututf16le(uint16_t * str, int buf_size, db_wchar_t c)
{
	/* sanity check */
	if (!VALID_UNICODE(c))
		/* not valid unicode char, would break the UTF-16 */
		return DB_EINVCHAR;

	if (c < 0x10000) {
		/* one word representation */
        if (str) {
            if (buf_size < 1)
                return DB_ENLSSPACE;
            UTF16LE_WRITE(str, (uint16_t)c );
        }
		return 1;
	} else {
		/* two words representation */
        if (str) {
    		if (buf_size < 2)
                return DB_ENLSSPACE;
            c -= 0x10000;
            UTF16LE_WRITE(str, (uint16_t)((c >> 10) + 0xD800));
            ++str;
            UTF16LE_WRITE(str, (uint16_t)((c & 0x3FF) + 0xDC00));

            /* variable used in later state, false-positive */
            //cppcheck-suppress unreadVariable 
            //cppcheck-suppress uselessAssignmentPtrArg
            ++str;
        }
		return 2;
	}
}

DBDLL_API int
db_nls_pututf16be(uint16_t * str, int buf_size, db_wchar_t c)
{
	/* sanity check */
	if (!VALID_UNICODE(c))
		/* not valid unicode char, would break the UTF-16 */
		return DB_EINVCHAR;

	if (c < 0x10000) {
		/* one word representation */
        if (str) {
            if (buf_size < 1)
                return DB_ENLSSPACE;
            UTF16BE_WRITE(str, (uint16_t)c);
        }
		return 1;
	} else {
		/* two words representation */
        if (str) {
            if (buf_size < 2)
                return DB_ENLSSPACE;
            c -= 0x10000;
            UTF16BE_WRITE(str, (uint16_t)((c >> 10) + 0xD800));
            ++str;
            UTF16BE_WRITE(str, (uint16_t)((c & 0x3FF) + 0xDC00));

            /* variable used in later state, false-positive */
            //cppcheck-suppress uselessAssignmentPtrArg
            //cppcheck-suppress unreadVariable 
            ++str;
        }
		return 2;
	}
}

/*********************************************************************/
DBDLL_API int
db_nls_getutf16(db_wchar_t * c, const uint16_t * str, int buf_size)
{
	int32_t t;
	if (buf_size < 1)
		return DB_ENLSSPACE;

    t = *str;
    ++str;
	if (t >= 0xD800 && t <= 0xDBFF) {
		/* composite */
		if (buf_size < 2)
			return DB_EINVCHAR;
		int32_t t1 = *str;

		if (t1 < 0xDC00 || t1 > 0xDFFF)
			return DB_EINVCHAR;
		*c = ((t & 0x3FF) << 10 | (t1 & 0x3FF)) + 0x10000;
		return 2;
	} else {
		/* single */
		if (t >= 0xDC00 && t <= 0xDFFF)
			/* broken string */
			return DB_EINVCHAR;

		*c = t;
		return 1;
	}
}

DBDLL_API int
db_nls_getutf16le(db_wchar_t * c, const uint16_t * str, int buf_size)
{
	int32_t t;
	if (buf_size < 1)
		return DB_ENLSSPACE;

	t = UTF16LE_READ(str);
    ++str;
	if (t >= 0xD800 && t <= 0xDBFF) {
		/* composite */
		if (buf_size < 2)
			return DB_EINVCHAR;
		int32_t t1 = UTF16LE_READ(str);

		if (t1 < 0xDC00 || t1 > 0xDFFF)
			return DB_EINVCHAR;
		*c = ((t & 0x3FF) << 10 | (t1 & 0x3FF)) + 0x10000;
		return 2;
	} else {
		/* single */
		if (t >= 0xDC00 && t <= 0xDFFF)
			/* broken string */
			return DB_EINVCHAR;

		*c = t;
		return 1;
	}
}

DBDLL_API int
db_nls_getutf16be(db_wchar_t * c, const uint16_t * str, int buf_size)
{
	int32_t t;
	if (buf_size < 1)
		return DB_ENLSSPACE;

	t = UTF16BE_READ(str);
    ++str;
	if (t >= 0xD800 && t <= 0xDBFF) {
		/* composite */
		if (buf_size < 2)
			return DB_EINVCHAR;
		int32_t t1 = UTF16BE_READ(str);

		if (t1 < 0xDC00 || t1 > 0xDFFF)
			return DB_EINVCHAR;
		*c = ((t & 0x3FF) << 10 | (t1 & 0x3FF)) + 0x10000;
		return 2;
	} else {
		/* single */
		if (t >= 0xDC00 && t <= 0xDFFF)
			/* broken string */
			return DB_EINVCHAR;

		*c = t;
		return 1;
	}
}

/*********************************************************************/
DBDLL_API int
db_nls_getutf32(db_wchar_t * c, const uint32_t * buf, int buf_size)
{
	if (buf_size < 1)
		return DB_ENLSSPACE;
	MEM_ASSIGN_ALIGN_SOURCE(*c, buf, db_wchar_t);
    return 1;
}

DBDLL_API int
db_nls_getutf32be(db_wchar_t * c, const uint32_t * buf, int buf_size)
{
	if (buf_size < 1)
		return DB_ENLSSPACE;

    *c = UTF32BE_READ( buf );
    return 1;
}

DBDLL_API int
db_nls_getutf32le(db_wchar_t * c, const uint32_t * buf, int buf_size)
{
	if (buf_size < 1)
		return DB_ENLSSPACE;

    *c = UTF32LE_READ( buf );
    return 1;
}

DBDLL_API int
db_nls_pututf32(uint32_t * buf, int buf_size, db_wchar_t c)
{
    if (!VALID_UNICODE(c))
        return DB_EINVCHAR;
    if (buf) {
        if (buf_size < 1)
            return DB_ENLSSPACE;
        //cppcheck-suppress objectIndex
        MEM_ASSIGN_ALIGN_DEST(buf, c, db_wchar_t);
    }
    return 1;
}

DBDLL_API int
db_nls_pututf32be(uint32_t * buf, int buf_size, db_wchar_t c)
{
    if (!VALID_UNICODE(c))
        return DB_EINVCHAR;
    if (buf) {
        if (buf_size < 1)
            return DB_ENLSSPACE;
        UTF32BE_WRITE( buf, c );
    }
    return 1;
}

DBDLL_API int
db_nls_pututf32le(uint32_t * buf, int buf_size, db_wchar_t c)
{
    if (!VALID_UNICODE(c))
        return DB_EINVCHAR;
    if (buf) {
        if (buf_size < 1)
            return DB_ENLSSPACE;
        UTF32LE_WRITE( buf, c );
    }
    return 1;
}

/* ANSI */
DBDLL_API int
db_nls_putansi(char * buf, int buf_size, db_wchar_t c)
{
    int sz;
    int mbmax = MB_CUR_MAX;

    if (!VALID_UNICODE(c))
        return DB_EINVCHAR;

    if (buf != NULL && buf_size >= mbmax) {
        /* convert in place */
        sz = wctomb(buf, (wchar_t)c);
        if (sz < 0)
            return DB_EINVCHAR;
    } else {
        char mb[20];
        DB_ASSERT(sizeof(mb)/sizeof(mb[0]) > mbmax);
        /* buffer could be overrun */

        sz = wctomb(mb, (wchar_t)c);
        if (buf) {
            if (sz > buf_size)
                return DB_ENLSSPACE;
            else if (sz < 0)
                return DB_EINVCHAR;
            else
                memcpy(buf, mb, sz);
        }
    }

    return sz;
}

DBDLL_API int
db_nls_getansi(db_wchar_t * c, const char * buf, int buf_size)
{
    wchar_t wc;
    int sz;

    /* handle '\0' separately */
    if (buf_size > 0 && buf[0] == '\0') {
        *c = 0;
        return 1;
    }
    sz = mbtowc(&wc, buf, buf_size);
    if (sz < 0)
        return DB_EINVCHAR;
    *c = wc;
    return sz;
}

DBDLL_API int
db_nls_getwchar(db_wchar_t * c, const wchar_t * buf, int buf_size)
{
    switch(sizeof(wchar_t)) {
    case sizeof(uint8_t): /* assume UTF encoding */
        return db_nls_getutf8(c, (const uint8_t*)buf, buf_size);
    case sizeof(uint16_t):
        return db_nls_getutf16(c, (const uint16_t*)buf, buf_size);
    case sizeof(uint32_t):
        return db_nls_getutf32(c, (const uint32_t*)buf, buf_size);

    default:
        if (buf_size < 1)
            return DB_ENLSSPACE;
        
        *c = *buf;

        /* variable used in later state, false-positive */
        //cppcheck-suppress unreadVariable 
        //cppcheck-suppress uselessAssignmentPtrArg
        ++buf;
        return 1;
    }
}

DBDLL_API int
db_nls_putwchar(wchar_t * buf, int buf_size, db_wchar_t c)
{
    switch(sizeof(wchar_t)) {
    case sizeof(uint8_t): /* assume UTF encoding */
        return db_nls_pututf8((uint8_t*)buf, buf_size, c);

    case sizeof(uint16_t):
        return db_nls_pututf16((uint16_t*)buf, buf_size, c);

    case sizeof(uint32_t):
        return db_nls_pututf32((uint32_t*)buf, buf_size, c);

    default:
        if (buf) {
            if (buf_size < 1)
                return DB_ENLSSPACE;
        
            *buf = (wchar_t)c;

            /* variable used in later state, false-positive */
            //cppcheck-suppress uselessAssignmentPtrArg
            //cppcheck-suppress unreadVariable 
            ++buf;
        }
        return 1;
    }
}

static db_encinfo_t encodings[] =
{
    { DB_NLS_WCHAR,   "wchar",    sizeof(wchar_t),  (db_nls_getchar_t)&db_nls_getwchar,   (db_nls_putchar_t)&db_nls_putwchar   },
    { DB_NLS_ANSI,    "ansi",     sizeof(char),     (db_nls_getchar_t)&db_nls_getansi,    (db_nls_putchar_t)&db_nls_putansi    },
    { DB_NLS_UTF8,    "utf-8",    sizeof(uint8_t),  (db_nls_getchar_t)&db_nls_getutf8,    (db_nls_putchar_t)&db_nls_pututf8    },

    { DB_NLS_UTF16,   "utf-16",   sizeof(uint16_t), (db_nls_getchar_t)&db_nls_getutf16,   (db_nls_putchar_t)&db_nls_pututf16   },
    { DB_NLS_UTF16LE, "utf-16le", sizeof(uint16_t), (db_nls_getchar_t)&db_nls_getutf16le, (db_nls_putchar_t)&db_nls_pututf16le },
    { DB_NLS_UTF16BE, "utf-16be", sizeof(uint16_t), (db_nls_getchar_t)&db_nls_getutf16be, (db_nls_putchar_t)&db_nls_pututf16be },

    { DB_NLS_UTF32,   "utf-32",   sizeof(int32_t),  (db_nls_getchar_t)&db_nls_getutf32,   (db_nls_putchar_t)&db_nls_pututf32   },
    { DB_NLS_UTF32LE, "utf-32le", sizeof(int32_t),  (db_nls_getchar_t)&db_nls_getutf32le, (db_nls_putchar_t)&db_nls_pututf32le },
    { DB_NLS_UTF32BE, "utf-32be", sizeof(int32_t),  (db_nls_getchar_t)&db_nls_getutf32be, (db_nls_putchar_t)&db_nls_pututf32be },
};

#define NENCODINGS (sizeof(encodings)/sizeof(encodings[0]))

static C_INLINE_SPECIFIER struct db_encinfo_t *
get_enc(int enc)
{
    DB_VALIDATE( enc >= 0 && enc < NENCODINGS, NULL );
    DB_ASSERT( encodings[enc].code == enc );
    return &encodings[enc];
}

#if 0
static int
char_size(int enc)
{
    return get_enc(enc)->char_size;
}
#endif

DBDLL_API int db_nls_convert(int               _dest_enc,
                             void            * _dest_buf,
                             int             * _dest_pos,
                             int               _dest_size,
                             int               _src_enc,
                             const void      * _src_buf,
                             int             * _src_pos,
                             int               _src_size,
                             db_nls_filter_t   _filter,
                             void            * _filter_arg)

/*
int db_nls_convert(int dest_enc, void * _dest_buf, int * _dest_size,
                   int src_enc, const void * _src_buf, int * _src_size)
*/
{
    struct db_encinfo_t * src_enc_info = get_enc(_src_enc);
    struct db_encinfo_t * dest_enc_info = get_enc(_dest_enc);
    int cp = 0, rc;
    db_wchar_t c;
    int src_pos = _src_pos ? *_src_pos : 0;
    int dest_pos = _dest_pos ? *_dest_pos : 0;
    const char * src_buf = (const char*)_src_buf + src_pos;
    char * dest_buf = (_dest_buf != NULL ? (char*)_dest_buf + dest_pos : 0);
#ifndef HAVE_ALLOCA
    char dest_buf_static[32];
#endif
    int src_char_size, dest_char_size;
    int src_left, dest_left;
    db_nls_getchar_t nls_getchar;
    db_nls_putchar_t nls_putchar;

    if (src_enc_info == NULL || dest_enc_info == NULL)
        return DB_EINVENC;

    src_char_size = src_enc_info->char_size;
    dest_char_size = dest_enc_info->char_size;

    nls_getchar = src_enc_info->nls_getchar;
    nls_putchar = dest_enc_info->nls_putchar;

    DB_ASSERT(src_char_size != 0 && dest_char_size != 0);
    DB_ASSERT(nls_getchar != NULL && nls_putchar != NULL);

    src_left  = (_src_size - src_pos) / src_char_size;
    
    /* if destination buffer is null, just check the necessary amount of space */
    if (dest_buf == NULL) {
#if defined(HAVE_ALLOCA) || defined(DB_DEBUG)
        int mbmax = MB_CUR_MAX;
        if (mbmax < UTF8_MAX)
            mbmax = UTF8_MAX;
        mbmax = (mbmax + dest_char_size - 1) / dest_char_size * dest_char_size;
#endif

#ifdef HAVE_ALLOCA
        /* false positive */
        //cppcheck-suppress allocaCalled
        dest_buf = alloca(mbmax);
        dest_left = mbmax / dest_char_size;
#else
        DB_ASSERT(sizeof(dest_buf_static) >= mbmax);
        dest_buf = dest_buf_static;
        dest_left = sizeof(dest_buf_static) / dest_char_size;
#endif
    } else
        dest_left = (_dest_size - dest_pos) / dest_char_size;

    rc = 0;

    while(src_left > 0) {
        rc = (*nls_getchar)(&c, src_buf + src_pos, src_left);
        if (rc < 0)
            break;

        if (_filter && (rc = (*_filter)(&c, _filter_arg)) <= 0)
            break;

        src_left -= rc;
        src_pos += src_char_size * rc;

        rc = (*nls_putchar)(dest_buf, dest_left, c);
        if (rc < 0)
            break;

        if (_dest_buf != NULL) { /* just count the number of bytes in the dest string */
            dest_left -= rc;
            dest_buf += rc * dest_char_size;
        }
        dest_pos += rc * dest_char_size;
        cp++;
    }

/*
    if (_src_size) *_src_size = src_size;
    if (_dest_size) *_dest_size = dest_size;
*/

    if (_src_pos)
        *_src_pos = src_pos;
    if (_dest_pos)
        *_dest_pos = dest_pos;

    return (rc < 0) ? rc : cp;
}

DBDLL_API int db_nls_find_encoding(const char * name)
{
    int j;
    for(j = 0; j < NENCODINGS; j++) {
        if (strcmp(name, encodings[j].name) == 0)
            return encodings[j].code;
    }
    return -1;
}

/* buffer size ans result are in bytes */
DBDLL_API int db_nls_putbuf(int enc, void * buf, int buf_size, db_wchar_t c)
{
    struct db_encinfo_t * enc_info = get_enc(enc);
    int rc;

    if (enc_info == NULL)
        return DB_EINVENC;

    rc = enc_info->nls_putchar(buf, buf_size / enc_info->char_size, c);
    if (rc <= 0)
        return rc;

    return rc * enc_info->char_size;
}

/* buffer size ans result are in bytes */
DBDLL_API int db_nls_getbuf(int enc, db_wchar_t * c, const void * buf, int buf_size)
{
    struct db_encinfo_t * enc_info = get_enc(enc);
    int rc;

    if (enc_info == NULL)
        return DB_EINVENC;

    rc = enc_info->nls_getchar(c, buf, buf_size / enc_info->char_size);
    if (rc <= 0)
        return rc;

    return rc * enc_info->char_size;
}

/* buffer size ans result are in characters */
DBDLL_API int db_nls_putstr(int enc, void * buf, int buf_size, db_wchar_t c)
{
    struct db_encinfo_t * enc_info = get_enc(enc);
    int rc;

    if (enc_info == NULL)
        return DB_EINVENC;

    rc = enc_info->nls_putchar(buf, buf_size, c);
    if (rc <= 0)
        return rc;

    return rc * enc_info->char_size;
}

/* buffer size ans result are in characters */
DBDLL_API int db_nls_getstr(int enc, db_wchar_t * c, const void * buf, int buf_size)
{
    struct db_encinfo_t * enc_info = get_enc(enc);
    int rc;

    if (enc_info == NULL)
        return DB_EINVENC;

    rc = enc_info->nls_getchar(c, buf, buf_size);
    if (rc <= 0)
        return rc;

    return rc * enc_info->char_size;
}

DBDLL_API int db_nls_char_size(int enc)
{
    struct db_encinfo_t * enc_info = get_enc(enc);
    if (enc_info == NULL)
        return DB_EINVENC;
    return enc_info->char_size;
}

DBDLL_API const db_encinfo_t *
db_nls_encinfo(int enc)
{
    if ( enc < 0 || enc >= NENCODINGS)
        return NULL;

    return get_enc(enc);
}

#endif
