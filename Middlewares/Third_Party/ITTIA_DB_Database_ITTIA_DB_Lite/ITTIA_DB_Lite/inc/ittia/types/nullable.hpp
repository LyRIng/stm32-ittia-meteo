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

#ifndef ITTIA_TYPES_NULLABLE_H
#define ITTIA_TYPES_NULLABLE_H

#include <assert.h>
#include <ittia/db/db_coltype.h>
#include <ittia/types/bind_traits.hpp>


namespace ittia {
namespace types {

/// @brief The Nullable class stores an optional value of any bindable type.
/// Nullable objects can be used with ResultField::get, ResultField::set,
/// and ResultRow::bind.
template <typename DataType>
class Nullable {
public:
    /// Initialize to null (is null)
    Nullable();
    /// Initialize to a value (is not null)
    explicit Nullable(const DataType&);

    /// Checks if object is null
    /// @returns
    /// @c true if object is null
    /// @c false if object is not null
    bool is_null() const { return ind == DB_FIELD_NULL; }

    /// Indirection operator overload for const DataType
    const DataType& operator *() const { assert(!is_null()); return value; }
    /// Indirection operator overload for DataType
    DataType& operator *() { assert(!is_null()); return value; }

    /// Sets object to null
    void set_null();
    /// Copies another nullable object of same type
    Nullable& operator=(const DataType&);

protected:
    /// Data type
    DataType value;
    /// Null/length indicator
    db_len_t ind;

    friend struct BindTraits< Nullable<DataType> >;
};

template <typename DataType>
inline Nullable<DataType>::Nullable()
    : ind(DB_FIELD_NULL)
{
}

template <typename DataType>
inline Nullable<DataType>::Nullable(const DataType& v)
    : value(v)
    , ind(::ittia::types::BindTraits<DataType>::data_size(v))
{
}

template <typename DataType>
inline void Nullable<DataType>::set_null()
{
    ind = DB_FIELD_NULL;
}

template <typename DataType>
inline Nullable<DataType> &Nullable<DataType>::operator=(const DataType &v)
{
    ind = ::ittia::types::BindTraits<DataType>::data_size(v);
    value = v;

    return *this;
}
/// Traits for binding a Nullable data type
template <typename DataType>
struct BindTraits< Nullable<DataType> > {
    typedef Nullable<DataType> bind_type;

    static const db_vartype_t var_type;
    static void* data_ptr(bind_type& v) { return ::ittia::types::BindTraits<DataType>::data_ptr(v.value); }
    static const void* data_ptr(const bind_type& v) { return ::ittia::types::BindTraits<DataType>::data_ptr(v.value); }
    static db_len_t data_size(const bind_type& v)   { return ::ittia::types::BindTraits<DataType>::data_size(v.value); }
    static db_len_t data_length(const bind_type& v) { return v.ind; }
    static db_len_t* ind_ptr(bind_type& v) { return &v.ind; }
    static db_len_t ind_scale(const bind_type& v) { return ::ittia::types::BindTraits<DataType>::ind_scale(v.value); }
    static void resize(bind_type& v, db_len_t len) { v.ind = len; }
};
template <typename DataType>
const db_vartype_t BindTraits<Nullable<DataType> >::var_type = ::ittia::types::BindTraits<DataType>::var_type;

}
}

#endif // ITTIA_TYPES_NULLABLE_H
