/**************************************************************************/
/*                                                                        */
/*      Copyright (c) 2005-2025 by ITTIA L.L.C. All rights reserved.      */
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

#ifndef ITTIA_TYPES_BIND_TRAITS_H
#define ITTIA_TYPES_BIND_TRAITS_H

#include <string.h>
#include <ittia/db/db_coltype.h>
#include <cstddef>
#include <cstring>

namespace ittia {
namespace types {

/// @brief Bind traits are used to bind a C or C++ data type to a
/// table field. This template class is specialized for each data type.
///
/// Bind traits are provided for most simple C data types. Bind traits can
/// support both simple value types and classes which encapsulate and provide
/// access to a variable of a supported type.
//
/// @tparam T
///   Bind type
template <typename T>
struct BindTraits;

/// Bind traits for simple fixed-size variable types.
///
/// @tparam T
///   Bind type
/// @tparam vartype
///   Variable type code
template <typename T, int vartype>
struct VarBindTraits {
    typedef T bind_type;

    /// Variable type. 
    static const db_vartype_t var_type;

    /// Get the data address for a bind variable.
    /// @param v
    ///   Bind value
    /// @returns
    ///   Address of bind data
    static void* data_ptr(bind_type& v) { return &v; }

    /// Get the constant data address for a bind variable.
    /// @param v
    ///   Bind value
    /// @returns
    ///   Constant address of bind data
    static const void* data_ptr(const bind_type& v) { return &v; }

    /// Get the number of bytes reserved for a bind variable.
    /// @returns
    ///   Size of bind data in bytes
    static db_len_t data_size(const bind_type&) { return sizeof(bind_type); }

    /// Get the length of a bind variable.
    /// @returns
    ///   Size of bind data in bytes
    static db_len_t data_length(const bind_type&) { return sizeof(bind_type); }

    /// Get address of length indicator, if available.
    /// @returns @c NULL
    static db_len_t* ind_ptr(bind_type&) { return NULL; }

    /// Get length indicator scaling constant.
    /// @returns
    ///   Size of bind data in bytes
    static db_len_t ind_scale(const bind_type&) { return sizeof(bind_type); }

    /// Resize has no effect for fixed-size variables.
    static void resize(bind_type&, db_len_t) {}
};
template <typename T, int vartype>
const db_vartype_t VarBindTraits<T, vartype>::var_type = reinterpret_cast<db_vartype_t>(vartype);


/// Bind traits for constant NUL-terminated character arrays of fixed size.
///
/// Constant types are read-only; see @ref BindTraits.
///
/// @tparam T
///   Bind type
/// @tparam vartype
///   Variable type code
/// @tparam N
///   Number of array elements, including NUL terminator
/// @tparam nul
///   Value of a NUL terminator
template <typename T, int vartype, std::size_t N, const T nul>
struct ConstStrBindTraits {
    typedef T bind_type[N];

    /// Variable type. 
    static const db_vartype_t var_type;

    /// Get the constant data address for a bind variable.
    /// @param v
    ///   Bind value
    /// @returns
    ///   Constant address of bind data
    static const void* data_ptr(const bind_type& v) { return &v; }

    /// Get the number of bytes reserved for a bind variable.
    /// @returns
    ///   Size of bind data in bytes
    static db_len_t data_size(const bind_type&) { return sizeof(bind_type); }

    /// Get the length of a bind variable.
    /// @param v
    ///   Bind value
    /// @returns
    ///   Size of bind data in characters
    static db_len_t data_length(const bind_type& v) {
        std::size_t len = 0;
        while (len < N && v[len] != nul) {
            ++len;
        }
        return len;
    }
};
template <typename T, int vartype, std::size_t N, const T nul>
const db_vartype_t ConstStrBindTraits<T, vartype, N, nul>::var_type = reinterpret_cast<db_vartype_t>(vartype);

/// Bind traits for NUL-terminated character arrays of fixed size.
///
/// @tparam T
///   Bind type
/// @tparam vartype
///   Variable type code
/// @tparam N
///   Number of array elements, including NUL terminator
/// @tparam nul
///   Value of a NUL terminator
template <typename T, int vartype, std::size_t N, const T nul>
struct StrBindTraits : public ConstStrBindTraits<T, vartype, N, nul> {
    typedef typename ConstStrBindTraits<T, vartype, N, nul>::bind_type bind_type;

    /// Get the data address for a bind variable.
    /// @param v
    ///   Bind value
    /// @returns
    ///   Address of bind data
    static void* data_ptr(bind_type& v) { return &v; }

    /// Get the constant data address for a bind variable.
    /// @param v
    ///   Bind value
    /// @returns
    ///   Constant address of bind data
    static const void* data_ptr(const bind_type& v) { return &v; }

    /// Get address of length indicator, if available.
    /// @returns @c NULL
    static db_len_t* ind_ptr(bind_type&) { return NULL; }

    /// Get length indicator scaling constant.
    /// @returns @c 1 always
    static db_len_t ind_scale(const bind_type&) { return sizeof(T); }

    /// Resize has no effect for NUL-terminated strings, since the length is
    /// encoded in the data itself.
    static void resize(bind_type&, db_len_t) {}
};

/// Bind traits for pointers of unknown size.
///
/// @tparam T
///   Bind type
/// @tparam vartype
///   Variable type code
template <typename T, int vartype>
struct PtrBindTraits {
    typedef T* bind_type;

    /// Variable type.
    static const db_vartype_t var_type;

    /// Get the constant data address for a bind variable.
    /// @param v
    ///   Bind value
    /// @returns
    ///   Constant address of bind data
    static const void* data_ptr(const bind_type& v) { return v; }
};
template <typename T, int vartype>
const db_vartype_t PtrBindTraits<T, vartype>::var_type = reinterpret_cast<db_vartype_t>(vartype);

/// Bind traits for NUL-terminated character strings.
///
/// Constant types are read-only; see @ref BindTraits.
///
/// @tparam T
///   Bind type
/// @tparam vartype
///   Variable type code
/// @tparam nul
///   Value of a NUL terminator
template <typename T, int vartype, const T nul>
struct CharPtrBindTraits : public PtrBindTraits<T, vartype> {
    typedef typename PtrBindTraits<T, vartype>::bind_type bind_type;

    /// Get the length of a bind variable.
    /// @param v
    ///   Bind value
    /// @returns
    ///   Size of bind data in characters
    static db_len_t data_length(const bind_type& v)
    {
        std::size_t len = 0;
        while (v[len] != nul) {
            ++len;
        }
        return len;
    }
};

/// Bind traits inherited from another type.
///
/// @tparam T
///   Bind type
/// @tparam B
///   Base bind type
template <typename T, typename B>
struct InheritedBindTraits {
    typedef T bind_type;

    static const db_vartype_t var_type;
    static void* data_ptr(bind_type& v) { return ::ittia::types::BindTraits<B>::data_ptr(v); }
    static const void* data_ptr(const bind_type& v) { return ::ittia::types::BindTraits<B>::data_ptr(v); }
    static db_len_t data_size(const bind_type& v)   { return ::ittia::types::BindTraits<B>::data_size(v); }
    static db_len_t data_length(const bind_type& v) { return ::ittia::types::BindTraits<B>::data_length(v); }
    static db_len_t* ind_ptr(bind_type& v) { return ::ittia::types::BindTraits<B>::ind_ptr(v); }
    static db_len_t ind_scale(const bind_type& v) { return ::ittia::types::BindTraits<B>::ind_scale(v); }
    static void resize(bind_type& v, db_len_t len) { ::ittia::types::BindTraits<B>::resize(v, len); }
};
template <typename T, typename B>
const db_vartype_t InheritedBindTraits<T, B>::var_type = ::ittia::types::BindTraits<B>::var_type;

/// Bind traits for a type that may be cast to a bindable type
///
/// @tparam T
///   Bind type
/// @tparam V
///   Cast type
/// @tparam vartype
///   Variable type code for the bind type
template <typename T, typename V, int vartype>
struct CastBindTraits {
    typedef T bind_type;
    static const db_vartype_t var_type;
    static void* data_ptr(bind_type& v) { return &static_cast<V&>(v); }
    static const void* data_ptr(const bind_type& v) { return &static_cast<const V&>(v); }
    static db_len_t data_size(const bind_type&)   { return sizeof(V); }
    static db_len_t data_length(const bind_type&) { return sizeof(V); }
    static db_len_t* ind_ptr(bind_type&) { return NULL; }
    static db_len_t ind_scale(const bind_type&) { return sizeof(V); }
    static void resize(bind_type&, db_len_t) {}
};
template <typename T, typename V, int vartype>
const db_vartype_t CastBindTraits<T, V, vartype>::var_type = (db_vartype_t)vartype;

/// Bind traits for int8_t type
template <> struct BindTraits<int8_t>              : public VarBindTraits<int8_t,              DB_VARTYPE_SINT8_TAG> {};
/// Bind traits for uint8_t type
template <> struct BindTraits<uint8_t>             : public VarBindTraits<uint8_t,             DB_VARTYPE_UINT8_TAG> {};
/// Bind traits for int16_t type
template <> struct BindTraits<int16_t>             : public VarBindTraits<int16_t,             DB_VARTYPE_SINT16_TAG> {};
/// Bind traits for uint16_t type
template <> struct BindTraits<uint16_t>            : public VarBindTraits<uint16_t,            DB_VARTYPE_UINT16_TAG> {};
/// Bind traits for int32_t type
template <> struct BindTraits<int32_t>             : public VarBindTraits<int32_t,             DB_VARTYPE_SINT32_TAG> {};
/// Bind traits for uint32_t type
template <> struct BindTraits<uint32_t>            : public VarBindTraits<uint32_t,            DB_VARTYPE_UINT32_TAG> {};
/// Bind traits for int64_t type
template <> struct BindTraits<int64_t>             : public VarBindTraits<int64_t,             DB_VARTYPE_SINT64_TAG> {};
/// Bind traits for db_largeint_t type
template <> struct BindTraits<db_largeint_t>       : public VarBindTraits<db_largeint_t,       DB_VARTYPE_SINT64_TAG> {};
/// Bind traits for uint64_t type
template <> struct BindTraits<uint64_t>            : public VarBindTraits<uint64_t,            DB_VARTYPE_UINT64_TAG> {};
/// Bind traits for float32_t type
template <> struct BindTraits<db_float32_t>        : public VarBindTraits<db_float32_t,        DB_VARTYPE_FLOAT32_TAG> {};
/// Bind traits for float64_t type
template <> struct BindTraits<db_float64_t>        : public VarBindTraits<db_float64_t,        DB_VARTYPE_FLOAT64_TAG> {};
/// Bind traits for db_date_t type
template <> struct BindTraits<db_date_t>           : public VarBindTraits<db_date_t,           DB_VARTYPE_DATE_TAG> {};
/// Bind traits for db_time_t type
template <> struct BindTraits<db_time_t>           : public VarBindTraits<db_time_t,           DB_VARTYPE_TIME_TAG> {};
/// Bind traits for db_datetime_t type
template <> struct BindTraits<db_datetime_t>       : public VarBindTraits<db_datetime_t,       DB_VARTYPE_DATETIME_TAG> {};
/// Bind traits for db_timestamp_t type
template <> struct BindTraits<db_timestamp_t>      : public VarBindTraits<db_timestamp_t,      DB_VARTYPE_TIMESTAMP_TAG> {};
#ifdef DB_VARTYPE_ODBC_DATE_TAG
/// Bind traits for db_odbc_date_t type
template <> struct BindTraits<db_odbc_date_t>      : public VarBindTraits<db_odbc_date_t,      DB_VARTYPE_ODBC_DATE_TAG> {};
/// Bind traits for db_odbc_time_t type
template <> struct BindTraits<db_odbc_time_t>      : public VarBindTraits<db_odbc_time_t,      DB_VARTYPE_ODBC_TIME_TAG> {};
/// Bind traits for db_odbc_timestamp_t type
template <> struct BindTraits<db_odbc_timestamp_t> : public VarBindTraits<db_odbc_timestamp_t, DB_VARTYPE_ODBC_TIMESTAMP_TAG> {};
#endif
/// Bind traits for fixed size array of db_ansi_t type
template <std::size_t N> struct BindTraits<db_ansi_t[N]> : public StrBindTraits<db_ansi_t, DB_VARTYPE_ANSISTR_TAG, N, '\0'> {};
/// Bind traits for fixed size array of db_utf8_t type
template <std::size_t N> struct BindTraits<db_utf8_t[N]> : public StrBindTraits<db_utf8_t, DB_VARTYPE_UTF8STR_TAG, N, 0> {};
/// Bind traits for fixed size array of db_utf16_t type
template <std::size_t N> struct BindTraits<db_utf16_t[N]> : public StrBindTraits<db_utf16_t, DB_VARTYPE_UTF16STR_TAG, N, 0> {};
/// Bind traits for fixed size array of db_utf32_t type
template <std::size_t N> struct BindTraits<db_utf32_t[N]> : public StrBindTraits<db_utf32_t, DB_VARTYPE_UTF32STR_TAG, N, 0> {};
/// Bind traits for fixed size const array of db_ansi_t type
template <std::size_t N> struct BindTraits<const db_ansi_t[N]> : public ConstStrBindTraits<const db_ansi_t, DB_VARTYPE_ANSISTR_TAG, N, '\0'> {};
/// Bind traits for fixed size const array of db_utf8_t type
template <std::size_t N> struct BindTraits<const db_utf8_t[N]> : public ConstStrBindTraits<const db_utf8_t, DB_VARTYPE_UTF8STR_TAG, N, 0> {};
/// Bind traits for fixed size const array of db_utf16_t type
template <std::size_t N> struct BindTraits<const db_utf16_t[N]> : public ConstStrBindTraits<const db_utf16_t, DB_VARTYPE_UTF16STR_TAG, N, 0> {};
/// Bind traits for fixed size const array of db_utf32_t type
template <std::size_t N> struct BindTraits<const db_utf32_t[N]> : public ConstStrBindTraits<const db_utf32_t, DB_VARTYPE_UTF32STR_TAG, N, 0> {};
/// Bind traits for NUL-terminated array of db_ansi_t type
template <> struct BindTraits<db_ansi_t*> : public CharPtrBindTraits<db_ansi_t, DB_VARTYPE_ANSISTR_TAG, '\0'>{};
/// Bind traits for NUL-terminated array of db_utf8_t type
template <> struct BindTraits<db_utf8_t*> : public CharPtrBindTraits<db_utf8_t, DB_VARTYPE_UTF8STR_TAG, 0>{};
/// Bind traits for NUL-terminated array of db_utf16_t type
template <> struct BindTraits<db_utf16_t*> : public CharPtrBindTraits<db_utf16_t, DB_VARTYPE_UTF16STR_TAG, 0>{};
/// Bind traits for NUL-terminated array of db_utf32_t type
template <> struct BindTraits<db_utf32_t*> : public CharPtrBindTraits<db_utf32_t, DB_VARTYPE_UTF32STR_TAG, 0>{};
/// Bind traits for NUL-terminated array of db_ansi_t type
template <> struct BindTraits<const db_ansi_t*> : public CharPtrBindTraits<const db_ansi_t, DB_VARTYPE_ANSISTR_TAG, '\0'>{};
/// Bind traits for NUL-terminated const array of db_utf8_t type
template <> struct BindTraits<const db_utf8_t*> : public CharPtrBindTraits<const db_utf8_t, DB_VARTYPE_UTF8STR_TAG, 0>{};
/// Bind traits for NUL-terminated const array of db_utf16_t type
template <> struct BindTraits<const db_utf16_t*> : public CharPtrBindTraits<const db_utf16_t, DB_VARTYPE_UTF16STR_TAG, 0>{};
/// Bind traits for NUL-terminated const array of db_utf32_t type
template <> struct BindTraits<const db_utf32_t*> : public CharPtrBindTraits<const db_utf32_t, DB_VARTYPE_UTF32STR_TAG, 0>{};


template <typename T>
static inline std::size_t is_field_null(const T& field)
{
    using Traits = ::ittia::types::BindTraits<T>;
    return Traits::data_length(field) == DB_FIELD_NULL;
}

template <typename T>
static inline std::size_t get_field_size(const T& field)
{
    using Traits = ::ittia::types::BindTraits<T>;
    return Traits::data_length(field) > 0 ? Traits::data_length(field) * Traits::ind_scale(field) : 0;
}

template <typename T>
static inline std::size_t assign_field(void * dest, const T& field)
{
    using Traits = ::ittia::types::BindTraits<T>;
    const std::size_t field_size = get_field_size(field);
    std::memcpy(dest, Traits::data_ptr(field), field_size);
    return field_size;
}

template <typename T>
static inline std::size_t assign_field(T& field, const void * src, std::size_t length)
{
    using Traits = ::ittia::types::BindTraits<T>;
    Traits::resize(field, length);
    const std::size_t field_size = get_field_size(field);
    memcpy(Traits::data_ptr(field), src, field_size);
    return field_size;
}


}
}

#endif // ITTIA_TYPES_BIND_TRAITS_H
