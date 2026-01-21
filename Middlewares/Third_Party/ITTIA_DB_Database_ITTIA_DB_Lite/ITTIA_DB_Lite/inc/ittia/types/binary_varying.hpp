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

#ifndef ITTIA_TYPES_BINARY_VARYING_H
#define ITTIA_TYPES_BINARY_VARYING_H

#include <string.h>
#include <assert.h>
#include <ittia/db/db_coltype.h>
#include <cstddef>

#include <ittia/types/nullable.hpp>

namespace ittia {
namespace types {


/// @brief The BinaryVarying class is a fixed-size buffer for database binary data.
/// @tparam N
///   Maximum number of bytes.
///
/// The binary data has an explicit length.
template <std::size_t N>
class BinaryVarying : public Nullable<unsigned char[N]> {
private:
    typedef Nullable<unsigned char[N]> base_type;

public:
    /// Create a null binary buffer.
    BinaryVarying();
    /// Copy another buffer of the exact same size.
    BinaryVarying(const BinaryVarying&);
    /// Assign from another buffer of the exact same size.
    BinaryVarying& operator=(const BinaryVarying&);

    /// Get the length of the buffer in bytes
    /// @returns
    /// The binary data length
    db_len_t length() const { return base_type::ind; }
    /// Set the length of the buffer in bytes.
    /// @param len
    /// The binary data length
    void set_length(db_len_t len) { assert(len >= 0); assert(len <= max_length()); base_type::ind = len; }
    /// Get the length of the buffer in code units.
    db_len_t max_length() const { return static_cast<db_len_t>(N); }

    template <std::size_t N2>
    bool operator==(const BinaryVarying<N2>& other) const;

    template <std::size_t N2>
    bool operator!=(const BinaryVarying<N2>& other) const;

    /// Copy a NUL-terminated character array of arbitrary size.
    /// @param other
    /// Reference of NUL-terminated character array
    template <std::size_t N2>
    explicit BinaryVarying(const unsigned char(&other)[N2]);

    /// Assign from a NUL-terminated character array of arbitrary size.
    /// @param other
    /// Reference of NUL-terminated character array
    template <std::size_t N2>
    BinaryVarying& operator=(const unsigned char(&other)[N2]);

    template <std::size_t N2>
    bool operator==(const unsigned char(&other)[N2]) const;

    template <std::size_t N2>
    bool operator!=(const unsigned char(&other)[N2]) const;
};


template <std::size_t N>
inline BinaryVarying<N>::BinaryVarying()
    : base_type()
{
}

template <std::size_t N>
inline BinaryVarying<N>::BinaryVarying(const BinaryVarying<N> &other)
    : base_type(other)
{
}

template <std::size_t N>
inline BinaryVarying<N> &BinaryVarying<N>::operator=(const BinaryVarying<N> &other)
{
    (void) base_type::operator =(other);
    return *this;
}

template <std::size_t N>
template <std::size_t N2>
inline bool BinaryVarying<N>::operator==(const BinaryVarying<N2>& other) const
{
    return length() == other.length() && (length() <= 0 || 0 == memcmp(**this, *other, length() * sizeof(unsigned char)));
}

template <std::size_t N>
template <std::size_t N2>
inline bool BinaryVarying<N>::operator!=(const BinaryVarying<N2>& other) const
{
    return !(*this == other);
}

template <std::size_t N>
template <std::size_t N2>
inline BinaryVarying<N>::BinaryVarying(const unsigned char (&other)[N2])
    : base_type()
{
    *this = other;
}

template <std::size_t N>
template <std::size_t N2>
inline BinaryVarying<N>& BinaryVarying<N>::operator=(const unsigned char (&other)[N2])
{
    base_type::ind = N2;
    if (base_type::ind > static_cast<db_len_t>(N)) {
        base_type::ind = N;
    }
    memcpy(base_type::value, other, length() * sizeof(unsigned char));
    return *this;
}

template <std::size_t N>
template <std::size_t N2>
inline bool BinaryVarying<N>::operator==(const unsigned char (&other)[N2]) const
{
    return length() == N2 && (length() <= 0 || 0 == memcmp(**this, other, length() * sizeof(unsigned char)));
}

template <std::size_t N>
template <std::size_t N2>
inline bool BinaryVarying<N>::operator!=(const unsigned char (&other)[N2]) const
{
    return !(*this == other);
}

///Traits to bind fixed-size buffers for database binary data.
template <std::size_t N>
struct BindTraits< BinaryVarying<N> > : public BindTraits< Nullable<unsigned char[N]> > {};

}
}

#endif // ITTIA_TYPES_BINARY_VARYING_H
