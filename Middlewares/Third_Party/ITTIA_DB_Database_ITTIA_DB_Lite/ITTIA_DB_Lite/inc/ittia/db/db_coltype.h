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

/** @file db_coltype.h
 *
 * Column types for ITTIA DB.
 */

#ifndef ITTIA_DB_COLTYPE_H
#define ITTIA_DB_COLTYPE_H

#include <ittia/db/db_environment.h>
#include <ittia/os/os_cpu.h>

C_HEADER_BEGIN

/** Database column type. @see CAPIDataTypes @ingroup CAPIDataTypes */
DB_DECLARE_HANDLE(db_coltype_t);
/** Variable type. @see CAPIDataTypes @ingroup CAPIDataTypes */
DB_DECLARE_HANDLE(db_vartype_t);

/** Field index number data type. */
typedef int32_t  db_fieldno_t;

#ifdef __cplusplus
#define DB_COLTYPE_CONST(value) reinterpret_cast<db_coltype_t>(value)
#define DB_VARTYPE_CONST(value) reinterpret_cast<db_vartype_t>(value)
#define DB_LEN_CONST(value) static_cast<db_len_t>(value)
#else
#define DB_COLTYPE_CONST(value) (db_coltype_t)(value)
#define DB_VARTYPE_CONST(value) (db_vartype_t)(value)
#define DB_LEN_CONST(value) (db_len_t)(value)
#endif

/** @name Length Indicator Values
 *  @{ */
/** @ingroup CAPIRow */
#ifndef _DB_OMIT_BIND_NAMES
/** Indicates that the data stored in a field is NULL. */
#define DB_FIELD_NULL            DB_LEN_CONST(-1)
/** The field data is a NUL-terminated string. */
#define DB_NTS                   DB_LEN_CONST(-3)
#endif
/** @} */

/** @name Database Data Type Tags
 *  @{ */
#ifndef _DB_OMIT_COLTYPE_TAG_NAMES
#define DB_COLTYPE_SINT8_TAG         (1)
#define DB_COLTYPE_UINT8_TAG         (2)
#define DB_COLTYPE_SINT16_TAG        (3)
#define DB_COLTYPE_UINT16_TAG        (4)
#define DB_COLTYPE_SINT32_TAG        (5)
#define DB_COLTYPE_UINT32_TAG        (6)
#define DB_COLTYPE_SINT64_TAG        (7)
#define DB_COLTYPE_UINT64_TAG        (8)

#define DB_COLTYPE_FLOAT32_TAG       (9)
#define DB_COLTYPE_FLOAT64_TAG      (10)
#define DB_COLTYPE_CURRENCY_TAG     (12)

#define DB_COLTYPE_DATE_TAG         (13)
#define DB_COLTYPE_TIME_TAG         (14)
#define DB_COLTYPE_DATETIME_TAG     (15)
#define DB_COLTYPE_TIMESTAMP_TAG    (16)

#define DB_COLTYPE_ANSISTR_TAG      (17)
#define DB_COLTYPE_UTF8STR_TAG      (22)
#define DB_COLTYPE_UTF16STR_TAG     (18)
#define DB_COLTYPE_UTF32STR_TAG     (19)

#define DB_COLTYPE_BINARY_TAG       (20)
#define DB_COLTYPE_BLOB_TAG         (21)
#define DB_COLTYPE_TIME_SERIES_TAG  (43)
#if DB_API_VER >= DB_API_111
#define DB_COLTYPE_BOOLEAN_TAG      (23)
#endif

#if DB_API_VER >= DB_API_10C
#define DB_COLTYPE_INTERVAL_DAY_TO_SECOND_TAG            (27)
#define DB_COLTYPE_INTERVAL_YEAR_TO_MONTH_TAG            (28)
#endif

#define DB_COLTYPE_TIMESTAMP_USEC_TAG (42)

#define DB_COLTYPE_UNDEFINED_TAG    (44)

#endif
/** @} */

/** Evaluates to true if the given column type is any UTF-encoded type.
 * @ingroup CAPIDataTypes */
#define DB_COLTYPE_IS_UTFSTR(a) (a == DB_COLTYPE_UTF8STR || a == DB_COLTYPE_UTF16STR || a == DB_COLTYPE_UTF32STR)

/** @name Database Data Type Identifiers
 *  @{ */
/** @ingroup CAPIDataTypes */
#ifndef _DB_OMIT_COLTYPE_NAMES
/** Signed 8-bit integer column type. SQL data type: sint8. @see db_coltype_t */
#define DB_COLTYPE_SINT8         DB_COLTYPE_CONST(DB_COLTYPE_SINT8_TAG)
/** Unsigned 8-bit integer column type. SQL data type: uint8. @see db_coltype_t */
#define DB_COLTYPE_UINT8         DB_COLTYPE_CONST(DB_COLTYPE_UINT8_TAG)
/** Signed 16-bit integer column type. SQL data type: sint16. @see db_coltype_t */
#define DB_COLTYPE_SINT16        DB_COLTYPE_CONST(DB_COLTYPE_SINT16_TAG)
/** Unsigned 16-bit integer column type. SQL data type: uint16. @see db_coltype_t */
#define DB_COLTYPE_UINT16        DB_COLTYPE_CONST(DB_COLTYPE_UINT16_TAG)
/** Signed 32-bit integer column type. SQL data type: sint32. @see db_coltype_t */
#define DB_COLTYPE_SINT32        DB_COLTYPE_CONST(DB_COLTYPE_SINT32_TAG)
/** Unsigned 32-bit integer column type. SQL data type: uint32. @see db_coltype_t */
#define DB_COLTYPE_UINT32        DB_COLTYPE_CONST(DB_COLTYPE_UINT32_TAG)
/** Signed 64-bit integer column type. SQL data type: sint64. @see db_coltype_t */
#define DB_COLTYPE_SINT64        DB_COLTYPE_CONST(DB_COLTYPE_SINT64_TAG)
/** Unsigned 64-bit integer column type. SQL data type: uint64. @see db_coltype_t */
#define DB_COLTYPE_UINT64        DB_COLTYPE_CONST(DB_COLTYPE_UINT64_TAG)

/** 32-bit floating point column type. SQL data type: float32. @see db_coltype_t */
#define DB_COLTYPE_FLOAT32       DB_COLTYPE_CONST(DB_COLTYPE_FLOAT32_TAG)
/** 64-bit floating point column type. SQL data type: float64. @see db_coltype_t */
#define DB_COLTYPE_FLOAT64       DB_COLTYPE_CONST(DB_COLTYPE_FLOAT64_TAG)
/** Fixed-point currency column type. SQL data type: currency. @see db_coltype_t */
#define DB_COLTYPE_CURRENCY      DB_COLTYPE_CONST(DB_COLTYPE_CURRENCY_TAG)

/** Date column type. SQL data type: date. @see db_coltype_t */
#define DB_COLTYPE_DATE          DB_COLTYPE_CONST(DB_COLTYPE_DATE_TAG)
/** Time column type with seconds precision. SQL data type: time. @see db_coltype_t */
#define DB_COLTYPE_TIME          DB_COLTYPE_CONST(DB_COLTYPE_TIME_TAG)
/** Date and time column type with seconds precision. SQL data type: datetime. @see db_coltype_t */
#define DB_COLTYPE_DATETIME      DB_COLTYPE_CONST(DB_COLTYPE_DATETIME_TAG)
/** Timestamp column type with microsecond precision. SQL data type: timestamp. @see db_coltype_t */
#define DB_COLTYPE_TIMESTAMP     DB_COLTYPE_CONST(DB_COLTYPE_TIMESTAMP_TAG)
/** Timestamp column type with microsecond precision. SQL data type: timestamp. @see db_coltype_t */
#define DB_COLTYPE_TIMESTAMP_USEC DB_COLTYPE_CONST(DB_COLTYPE_TIMESTAMP_USEC_TAG)

/** ANSI character string column type with system default encoding. SQL data type: ansistr(n). @see db_coltype_t */
#define DB_COLTYPE_ANSISTR       DB_COLTYPE_CONST(DB_COLTYPE_ANSISTR_TAG)
/** Unicode character string column type with UTF-8 encoding. SQL data type: utf8str(n). @see db_coltype_t */
#define DB_COLTYPE_UTF8STR       DB_COLTYPE_CONST(DB_COLTYPE_UTF8STR_TAG)
/** Unicode character string column type with UTF-16 encoding. SQL data type: utf16str(n). @see db_coltype_t */
#define DB_COLTYPE_UTF16STR      DB_COLTYPE_CONST(DB_COLTYPE_UTF16STR_TAG)
/** Unicode character string column type with UTF-32 encoding. SQL data type: utf32str(n). @see db_coltype_t */
#define DB_COLTYPE_UTF32STR      DB_COLTYPE_CONST(DB_COLTYPE_UTF32STR_TAG)

/** Binary data column. @see db_coltype_t */
#define DB_COLTYPE_BINARY        DB_COLTYPE_CONST(DB_COLTYPE_BINARY_TAG)
/** Binary large object column type. SQL data type: blob. @see db_coltype_t */
#define DB_COLTYPE_BLOB          DB_COLTYPE_CONST(DB_COLTYPE_BLOB_TAG)
/** Time series large object column type. SQL data type: tlob. @see db_coltype_t */
#define DB_COLTYPE_TIME_SERIES   DB_COLTYPE_CONST(DB_COLTYPE_TIME_SERIES_TAG)

#if DB_API_VER >= DB_API_111
/** Boolean column type. @see db_coltype_t */
#define DB_COLTYPE_BOOLEAN       DB_COLTYPE_CONST(DB_COLTYPE_BOOLEAN_TAG)
#endif

#if DB_API_VER >= DB_API_10C
/** Interval day to second column type. @see db_coltype_t */
#define DB_COLTYPE_INTERVAL_DAY_TO_SECOND            DB_COLTYPE_CONST(DB_COLTYPE_INTERVAL_DAY_TO_SECOND_TAG)
/** Interval year to month column type. @see db_coltype_t */
#define DB_COLTYPE_INTERVAL_YEAR_TO_MONTH            DB_COLTYPE_CONST(DB_COLTYPE_INTERVAL_YEAR_TO_MONTH_TAG)
#endif

/** Unknown column type. @see db_coltype_t */
#define DB_COLTYPE_UNDEFINED     DB_COLTYPE_CONST(DB_COLTYPE_UNDEFINED_TAG)

/** Evaluates to the column type that matches wchar_t on this platform.
 *
 * @note Use of this macro on a platform that does not support wchar_t will
 * result in a compile error.
 *
 * @see db_coltype_t, DB_COLTYPE_UTF16STR, DB_COLTYPE_UTF32STR
 */
#define DB_COLTYPE_WCHARSTR (sizeof(wchar_t) == sizeof(db_utf16_t) ? DB_COLTYPE_UTF16STR : DB_COLTYPE_UTF32STR)
#endif
/** @} */

/** @name C Language Data Type Tags
 *  @{ */
#ifndef _DB_OMIT_VARTYPE_TAG_NAMES

#define DB_VARTYPE_SINT8_TAG       (1)
#define DB_VARTYPE_UINT8_TAG       (2)
#define DB_VARTYPE_SINT16_TAG      (3)
#define DB_VARTYPE_UINT16_TAG      (4)
#define DB_VARTYPE_SINT32_TAG      (5)
#define DB_VARTYPE_UINT32_TAG      (6)
#define DB_VARTYPE_SINT64_TAG      (7)
#define DB_VARTYPE_UINT64_TAG      (8)

#define DB_VARTYPE_FLOAT32_TAG     (9)
#define DB_VARTYPE_FLOAT64_TAG    (10)

#define DB_VARTYPE_DATE_TAG       (13)
#define DB_VARTYPE_TIME_TAG       (14)
#define DB_VARTYPE_DATETIME_TAG   (15)
#define DB_VARTYPE_TIMESTAMP_TAG  (16)
#define DB_VARTYPE_TIMESTAMP_USEC_TAG (42)

#define DB_VARTYPE_ANSISTR_TAG    (17)
#define DB_VARTYPE_UTF8STR_TAG    (22)
#define DB_VARTYPE_UTF16STR_TAG   (18)
#define DB_VARTYPE_UTF32STR_TAG   (19)

#define DB_VARTYPE_BINARY_TAG     (20)
#define DB_VARTYPE_BLOB_TAG       (21)
#define DB_VARTYPE_TIME_SERIES_TAG (43)
#define DB_VARTYPE_BOOLEAN_TAG    (23)

#define DB_VARTYPE_INTERVAL_DAY_TO_SECOND_TAG            (27)
#define DB_VARTYPE_INTERVAL_YEAR_TO_MONTH_TAG            (28)

#define DB_VARTYPE_UNDEFINED_TAG  (44)

#endif

/** @} */

/** @name C Language Data Type Identifiers
 *  @{ */
/** @ingroup CAPIDataTypes */
#ifndef _DB_OMIT_VARTYPE_NAMES
/** Signed 8-bit integer variable type. C data type: int8_t. @see db_vartype_t */
#define DB_VARTYPE_SINT8          DB_VARTYPE_CONST(DB_VARTYPE_SINT8_TAG)
/** Unsigned 8-bit integer variable type. C data type: uint8_t. @see db_vartype_t */
#define DB_VARTYPE_UINT8          DB_VARTYPE_CONST(DB_VARTYPE_UINT8_TAG)
/** Signed 16-bit integer variable type. C data type: int16_t. @see db_vartype_t */
#define DB_VARTYPE_SINT16         DB_VARTYPE_CONST(DB_VARTYPE_SINT16_TAG)
/** Unsigned 16-bit integer variable type. C data type: uint16_t. @see db_vartype_t */
#define DB_VARTYPE_UINT16         DB_VARTYPE_CONST(DB_VARTYPE_UINT16_TAG)
/** Signed 32-bit integer variable type. C data type: int32_t. @see db_vartype_t */
#define DB_VARTYPE_SINT32         DB_VARTYPE_CONST(DB_VARTYPE_SINT32_TAG)
/** Unsigned 32-bit integer variable type. C data type: uint32_t. @see db_vartype_t */
#define DB_VARTYPE_UINT32         DB_VARTYPE_CONST(DB_VARTYPE_UINT32_TAG)
/** Signed 64-bit integer variable type. C data type: int64_t. @see db_vartype_t */
#define DB_VARTYPE_SINT64         DB_VARTYPE_CONST(DB_VARTYPE_SINT64_TAG)
/** Unsigned 64-bit integer variable type. C data type: uint64_t. @see db_vartype_t */
#define DB_VARTYPE_UINT64         DB_VARTYPE_CONST(DB_VARTYPE_UINT64_TAG)

/** 32-bit floating point variable type. C data type: #db_float32_t. @see db_vartype_t */
#define DB_VARTYPE_FLOAT32        DB_VARTYPE_CONST(DB_VARTYPE_FLOAT32_TAG)
/** 64-bit floating point variable type. C data type: #db_float64_t. @see db_vartype_t */
#define DB_VARTYPE_FLOAT64        DB_VARTYPE_CONST(DB_VARTYPE_FLOAT64_TAG)

/** Date variable type. C data type: db_date_t. @see db_vartype_t */
#define DB_VARTYPE_DATE           DB_VARTYPE_CONST(DB_VARTYPE_DATE_TAG)
/** Time variable type with seconds precision. C data type: #db_time_t. @see db_vartype_t */
#define DB_VARTYPE_TIME           DB_VARTYPE_CONST(DB_VARTYPE_TIME_TAG)
/** Date and time variable type with seconds precision. C data type: #db_datetime_t. @see db_vartype_t */
#define DB_VARTYPE_DATETIME       DB_VARTYPE_CONST(DB_VARTYPE_DATETIME_TAG)
/** Timestamp variable type with microsecond precision. C data type: #db_timestamp_t. @see db_vartype_t */
#define DB_VARTYPE_TIMESTAMP      DB_VARTYPE_CONST(DB_VARTYPE_TIMESTAMP_TAG)
/** Timestamp variable type with microsecond precision. C data type: #db_timestamp_usec_t. @see db_vartype_t */
#define DB_VARTYPE_TIMESTAMP_USEC DB_VARTYPE_CONST(DB_VARTYPE_TIMESTAMP_USEC_TAG)

/** ANSI character string variable type with system default encoding. C data type: #db_ansi_t*. @see db_vartype_t */
#define DB_VARTYPE_ANSISTR        DB_VARTYPE_CONST(DB_VARTYPE_ANSISTR_TAG)
/** Unicode character string variable type with UTF-8 encoding. C data type: #db_utf8_t*. @see db_vartype_t */
#define DB_VARTYPE_UTF8STR        DB_VARTYPE_CONST(DB_VARTYPE_UTF8STR_TAG)
/** Unicode character string variable type with UTF-16 encoding. C data type: #db_utf16_t*. @see db_vartype_t */
#define DB_VARTYPE_UTF16STR        DB_VARTYPE_CONST(DB_VARTYPE_UTF16STR_TAG)
/** Unicode character string variable type with UTF-32 encoding. C data type: #db_utf32_t*. @see db_vartype_t */
#define DB_VARTYPE_UTF32STR        DB_VARTYPE_CONST(DB_VARTYPE_UTF32STR_TAG)

/** Binary data column. @see db_vartype_t */
#define DB_VARTYPE_BINARY         DB_VARTYPE_CONST(DB_VARTYPE_BINARY_TAG)
/** Binary large object variable type. C data type: #db_blob_t. @see db_vartype_t */
#define DB_VARTYPE_BLOB           DB_VARTYPE_CONST(DB_VARTYPE_BLOB_TAG)
/** Time series large object variable type. C data type: #db_time_series_t. @see db_vartype_t */
#define DB_VARTYPE_TIME_SERIES    DB_VARTYPE_CONST(DB_VARTYPE_TIME_SERIES_TAG)

/** Boolean data column. */
#define DB_VARTYPE_BOOLEAN        DB_VARTYPE_CONST(DB_VARTYPE_BOOLEAN_TAG)

/** Number of microseconds in a day to second time interval. C data type: #db_interval_t. @see db_vartype_t */
#define DB_VARTYPE_INTERVAL_DAY_TO_SECOND            DB_VARTYPE_CONST(DB_VARTYPE_INTERVAL_DAY_TO_SECOND_TAG)
/** Number of months in a year to month time interval. C data type: #db_interval_t. @see db_vartype_t */
#define DB_VARTYPE_INTERVAL_YEAR_TO_MONTH            DB_VARTYPE_CONST(DB_VARTYPE_INTERVAL_YEAR_TO_MONTH_TAG)

/** Unknown variable type. @see db_vartype_t */
#define DB_VARTYPE_UNDEFINED      DB_VARTYPE_CONST(DB_VARTYPE_UNDEFINED_TAG)

/** Evaluates to the variable type that matches wchar_t on this platform.
 *
 * @note Use of this macro on a platform that does not support wchar_t will
 * result in a compile error.
 *
 * @see db_vartype_t, DB_VARTYPE_UTF16STR, DB_VARTYPE_UTF32STR
 */
#define DB_VARTYPE_WCHARSTR (sizeof(wchar_t) == sizeof(db_utf16_t) ? DB_VARTYPE_UTF16STR : DB_VARTYPE_UTF32STR)

#endif
/** @} */


/** @name Data Types
 *  @{ */
/** @ingroup CAPIDataTypes */
typedef char        db_ansi_t;    /**< ANSI string character type. */
typedef uint8_t     db_utf8_t;    /**< UTF8 string character type. */
typedef uint16_t    db_utf16_t;   /**< UTF16 string character type. */
typedef uint32_t    db_utf32_t;   /**< UTF32 string character type. */
typedef float32_t   db_float32_t; /**< 32-bit floating point type. */
typedef float64_t   db_float64_t; /**< 64-bit floating point type. */
typedef db_bool_t   db_boolean_t; /**< Boolean type. */
#if OS_CPU_LEN == 64
typedef int64_t     db_len_t;     /**< Field length data type (usually in bytes). A value of #DB_LEN_FAIL indicates an error. @see DB_LEN_FAIL */
#else
typedef int32_t     db_len_t;     /**< Field length data type (usually in bytes). A value of #DB_LEN_FAIL indicates an error. @see DB_LEN_FAIL */
#endif
/** @} Data Types */

/**
 * Large integer (64-bit) data type.
 * @ingroup CAPIDataTypes
 */
typedef union
{
    BYTE_ORDER_STRUCT( int32_t high, uint32_t low ) int32;

#ifdef HAVE_INT64
    int64_t int64;
#else
    db_AlignType __alignment; /* enforce alignment rules */
#endif
} db_largeint_t;

/* 64-bit integer placeholders, can be used instead of strict int64 */

/**
 * Unsigned large integer (64-bit) data type.
 * @ingroup CAPIDataTypes
 */
typedef union db_ulargeint_t
{
    BYTE_ORDER_STRUCT( uint32_t high, uint32_t low ) uint32;

#ifdef HAVE_INT64
    uint64_t uint64;
#else
    db_AlignType __alignment; /* enforce alignment rules */
#endif
} db_ulargeint_t;

/**
 * Date data type
 * @ingroup CAPIDataTypes
 *
 * @see db_datetime_t
 */
typedef struct db_date_t
{
    uint16_t year;
    uint16_t month;
    uint16_t day;
} db_date_t;

/**
 * Time data type
 * @ingroup CAPIDataTypes
 *
 * @see db_datetime_t
 */
typedef struct db_time_t
{
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
} db_time_t;

/**
 * Date and time data type
 * @ingroup CAPIDataTypes
 *
 * @see db_timestamp_t
 */
typedef struct db_datetime_t
{
    db_date_t date;
    db_time_t time;
} db_datetime_t;

/**
 * Timestamp data type
 * @ingroup CAPIDataTypes
 */
typedef struct db_timestamp_t
{
    db_datetime_t datetime;
    uint32_t      microsec;
} db_timestamp_t;

/** A time interval represented as a number; units depend on the selected variable type. */
typedef int64_t db_interval_t;

/** A timestamp represented as the number of microseconds since 1970-01-01 00:00:00. */
typedef int64_t db_timestamp_usec_t;



/** @name Field Flags
 *  @{ */
/** @ingroup CAPISchemaTable */
#ifndef _DB_OMIT_FIELD_FLAG_NAMES
/** Field can be NULL. */
#define DB_NULLABLE              0u
/** Field cannot be NULL. */
#define DB_NOT_NULL              1u
#define DB_NULL_MASK             (DB_NOT_NULL | DB_NULLABLE)
/** Field is generated by identity. */
#define DB_IDENTITY              2u
#define DB_GENERATED_MASK        (DB_IDENTITY)
#endif
/** @} */

/** Field definition.
 * @ingroup CAPISchemaTable
 *
 * @see db_tabledef_t::fields
 */
typedef struct db_fielddef_t
{
    db_fieldno_t    fieldno;
    char            field_name[DB_MAX_FIELD_NAME + 1];
    db_coltype_t    field_type;
    db_len_t        field_size; /**< Logical field size or scale. */
    db_len_t        field_prec; /**< Logical field precision. */
    db_flags_t      field_flags; /**< @see DB_NULLABLE, DB_NOT_NULL, DB_IDENTITY */
    void *          field_value;   /**< Reserved, should be NULL */

    /***/
    db_len_t        field_width; /**< Physical field width in bytes. */
} db_fielddef_t;


C_HEADER_END

#endif /* ITTIA_DB_COLTYPE_H */
