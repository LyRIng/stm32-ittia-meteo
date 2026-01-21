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

#ifndef ITTIA_TYPES_CHARACTER_VARYING_H
#define ITTIA_TYPES_CHARACTER_VARYING_H

#include <string.h>
#include <assert.h>
#include <cstddef>
#include <ittia/db/db_coltype.h>
#include <ittia/types/nullable.hpp>

namespace ittia {
namespace types {


/// @brief The CharacterVarying class is a fixed-size buffer for database character strings.
/// @tparam N
///   Maximum number of bytes, not including NUL-terminator.
/// @tparam charT 
///   A character type corresponding to the string encoding.
///
/// The character string has an explicit length and may optionally be NUL-terminated.
template <std::size_t N, class charT = db_ansi_t>
class CharacterVarying : public Nullable<charT[N+1]> {
private:
    typedef Nullable<charT[N+1]> base_type;

public:
    /// Create a null string.
    CharacterVarying();
    /// Copy another buffer of the exact same size.
    CharacterVarying(const CharacterVarying&);
    /// Assign from another buffer of the exact same size.
    CharacterVarying& operator=(const CharacterVarying&);

    /// Get the length of the string in code units.
    /// @returns
    /// The string length
    db_len_t length() const { return base_type::ind; }
    /// Set the length of the string in code units.
    /// @param len
    /// The string length
    void set_length(db_len_t len) { assert(len >= 0); assert(len <= max_length()); BindTraits<base_type>::resize(*this, len); base_type::value[len] = charT(); }
    /// Get the length of the buffer in code units.
    db_len_t max_length() const { return static_cast<db_len_t>(N); }

    template <std::size_t N2>
    bool operator==(const CharacterVarying<N2, charT>& other) const;

    template <std::size_t N2>
    bool operator!=(const CharacterVarying<N2, charT>& other) const;

    /// Copy a NUL-terminated character array of arbitrary size.
    /// @param other
    /// Reference of NUL-terminated character array
    template <std::size_t N2>
    explicit CharacterVarying(const charT(&other)[N2]);

    /// Assign from a NUL-terminated character array of arbitrary size.
    /// @param other
    /// Reference of NUL-terminated character array
    template <std::size_t N2>
    CharacterVarying& operator=(const charT(&other)[N2]);

    template <std::size_t N2>
    bool operator==(const charT(&other)[N2]) const;

    template <std::size_t N2>
    bool operator!=(const charT(&other)[N2]) const;

    /// Copy a NUL-terminated character string.
    /// @param other
    /// Reference of NUL-terminated character string
    explicit CharacterVarying(const charT* const &other);

    /// Assign from a NUL-terminated character string.
    /// @param other
    /// Reference of NUL-terminated character string
    CharacterVarying& operator=(const charT* const &other);

    bool operator==(const charT* &other) const;

    bool operator!=(const charT* &other) const;
};

/// @brief The Utf16Varying class is a fixed-size buffer for database character strings with UTF-16 encoding.
/// @tparam N maximum number of UTF-16 code units, not including NUL-terminator.
template <std::size_t N> using Utf16Varying = CharacterVarying<N, db_utf16_t>;
/// @brief The Utf32Varying class is a fixed-size buffer for database character strings with UTF-32 encoding.
/// @tparam N maximum number of UTF-32 code units, not including NUL-terminator.
template <std::size_t N> using Utf32Varying = CharacterVarying<N, db_utf32_t>;

/// @brief The Utf8Varying class is a fixed-size buffer for database character strings with UTF-8 encoding.
/// @tparam N maximum number of UTF-8 code units, not including NUL-terminator.
template <std::size_t N>
class Utf8Varying : public CharacterVarying<N, db_utf8_t> {
public:
    using CharacterVarying<N, db_utf8_t>::CharacterVarying;

    /// Copy another buffer of the exact same size.
    Utf8Varying(const CharacterVarying<N, db_utf8_t>&);
    /// Assign from another buffer of the exact same size.
    Utf8Varying& operator=(const CharacterVarying<N, db_utf8_t>&);

    /// Copy a a character string into a @c Utf8Varying object.
    /// @param other A UTF-8 encoded NUL-terminated character string.
    /// @note No validation is performed on the string encoding.
    static Utf8Varying from_utf8_char(const char * const other);

    /// Get the value of a @c Utf8Varying object as a constant UTF-8 encoded character string.
    /// @returns A pointer to a UTF-8 encoded NUL-terminated character string.
    const char * as_utf8_char() const;
    /// Get the value of a @c Utf8Varying object as a UTF-8 encoded character string.
    /// @returns A pointer to a UTF-8 encoded NUL-terminated character string.
    /// Modifying the character string will modify the @c Utf8Varying object.
    char * as_utf8_char();
};


template <std::size_t N, class charT>
inline CharacterVarying<N, charT>::CharacterVarying()
    : base_type()
{
    base_type::value[0] = charT();
}

template <std::size_t N, class charT>
inline CharacterVarying<N, charT>::CharacterVarying(const CharacterVarying<N, charT> &other)
    : base_type(other)
{
}

template <std::size_t N, class charT>
inline CharacterVarying<N, charT> &CharacterVarying<N, charT>::operator=(const CharacterVarying<N, charT> &other)
{
    (void) base_type::operator =(other);
    return *this;
}

template <std::size_t N, class charT>
template <std::size_t N2>
inline bool CharacterVarying<N, charT>::operator==(const CharacterVarying<N2, charT>& other) const
{
    return length() == other.length() && (length() <= 0 || 0 == memcmp(**this, *other, length() * sizeof(charT)));
}

template <std::size_t N, class charT>
template <std::size_t N2>
inline bool CharacterVarying<N, charT>::operator!=(const CharacterVarying<N2, charT>& other) const
{
    return !(*this == other);
}

template <std::size_t N, class charT>
template <std::size_t N2>
inline CharacterVarying<N, charT>::CharacterVarying(const charT (&other)[N2])
    : base_type()
{
    *this = other;
}

template <std::size_t N, class charT>
inline CharacterVarying<N, charT>::CharacterVarying(const charT* const &other)
    : base_type()
{
    *this = other;
}

template <std::size_t N, class charT>
template <std::size_t N2>
inline CharacterVarying<N, charT>& CharacterVarying<N, charT>::operator=(const charT (&other)[N2])
{
    base_type::ind = ::ittia::types::BindTraits<const charT[N2]>::data_length(other);
    assert(base_type::ind >= 0);
    if (base_type::ind > static_cast<db_len_t>(N)) {
        base_type::ind = N;
    }
    memcpy(base_type::value, other, length() * sizeof(charT));
    base_type::value[length()] = charT();
    return *this;
}

template <std::size_t N, class charT>
inline CharacterVarying<N, charT>& CharacterVarying<N, charT>::operator=(const charT * const &other)
{
    base_type::ind = ::ittia::types::BindTraits<const charT*>::data_length(other);
    assert(base_type::ind >= 0);
    if (base_type::ind > static_cast<db_len_t>(N)) {
        base_type::ind = N;
    }
    memcpy(base_type::value, other, length() * sizeof(charT));
    base_type::value[length()] = charT();
    return *this;
}

template <std::size_t N, class charT>
template <std::size_t N2>
inline bool CharacterVarying<N, charT>::operator==(const charT (&other)[N2]) const
{
    const db_len_t other_length = ::ittia::types::BindTraits<const charT[N2]>::data_length(other);
    return length() == other_length && (length() <= 0 || 0 == memcmp(**this, other, length() * sizeof(charT)));
}

template <std::size_t N, class charT>
inline bool CharacterVarying<N, charT>::operator==(const charT * &other) const
{
    const db_len_t other_length = ::ittia::types::BindTraits<const charT*>::data_length(other);
    return length() == other_length && (length() <= 0 || 0 == memcmp(**this, other, length() * sizeof(charT)));
}

template <std::size_t N, class charT>
template <std::size_t N2>
inline bool CharacterVarying<N, charT>::operator!=(const charT (&other)[N2]) const
{
    return !(*this == other);
}

template <std::size_t N, class charT>
inline bool CharacterVarying<N, charT>::operator!=(const charT * &other) const
{
    return !(*this == other);
}

template <std::size_t N>
inline Utf8Varying<N>::Utf8Varying(const CharacterVarying<N, db_utf8_t> &other)
    : CharacterVarying<N, db_utf8_t>(other)
{
}

template <std::size_t N>
inline Utf8Varying<N> &Utf8Varying<N>::operator=(const CharacterVarying<N, db_utf8_t> &other)
{
    (void) CharacterVarying<N, db_utf8_t>::operator =(other);
    return *this;
}

template <std::size_t N>
Utf8Varying<N> Utf8Varying<N>::from_utf8_char(const char * const other)
{
    Utf8Varying<N> result;
    result.ind = ::ittia::types::BindTraits<const char*>::data_length(other);
    assert(result.ind >= 0);
    if (result.ind > static_cast<db_len_t>(N)) {
        result.ind = N;
    }
    memcpy(result.value, other, result.length() * sizeof(db_utf8_t));
    result.value[result.length()] = db_utf8_t();
    return result;
}

template <std::size_t N>
const char * Utf8Varying<N>::as_utf8_char() const
{
    return reinterpret_cast<const char*>(this->operator*());
}

template <std::size_t N>
char * Utf8Varying<N>::as_utf8_char()
{
    return reinterpret_cast<char*>(this->operator*());
}


///Traits to bind fixed sized nullable database strings
template <std::size_t N, class charT>
struct BindTraits< CharacterVarying<N, charT> > : public BindTraits< Nullable<charT[N+1]> > {
    static void resize(CharacterVarying<N, charT>& v, db_len_t len) { v.set_length(len); }
};

///Traits to bind fixed sized nullable database strings with UTF-8 encoding.
template <std::size_t N>
struct BindTraits< Utf8Varying<N> > : public BindTraits< CharacterVarying<N, db_utf8_t> > {};

}
}

#endif // ITTIA_TYPES_CHARACTER_VARYING_H
