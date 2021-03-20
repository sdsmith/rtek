#pragma once

#include "core/assert.h"
#include "core/status.h"
#include <string>
#include <string_view>

// #define RK_CHAR_WIDE 0x1000
// #define RK_CHAR_NARROW 0x1001

// /**
//  * \def RK_UNICODE
//  * \brief Unicode encoding being used for unicode characters and strings.
//  *
//  * Has a value from one of \a RK_UNICODE_*.
//  * Usage: `#if RK_UNICODE == RK_UNICODE_UTF16`
//  */

// #ifndef RK_UNICODE
// #    define RK_UNICODE_UTF8 1
// #    define RK_UNICODE_UTF16 2

// #    if RK_OS == RK_OS_WINDOWS
// #        define RK_UNICODE RK_UNICODE_UTF16
// #        define RK_UNICODE_CHAR RK_CHAR_WIDE
// #    elif RK_OS == RK_OS_LINUX
// #        define RK_UNICODE RK_UNICODE_UTF8
// #        define RK_UNICODE_CHAR RK_CHAR_NARROW
// #    endif
// #endif
// RK_STATIC_ASSERT(RK_OS == RK_OS_WINDOWS && RK_UNICODE == RK_UNICODE_UTF16 &&
//                  RK_UNICODE_CHAR == RK_CHAR_WIDE);

namespace rk
{
#if RK_OS == RK_OS_WINDOWS
/**
 * \brief Unicode string.
 *
 * \see rk::uchar
 */
using ustring = std::wstring;
using ustring_view = std::wstring_view;
#elif RK_OS == RK_OS_LINUX
/**
 * \brief Unicode string.
 *
 * \see rk::uchar
 */
using ustring = std::string;
using ustring = std::string_view;
#else
#    error Unsupported OS
#endif

namespace unicode
{
/**
 * \brief Number of characters in a unicode string.
 *
 * For wide encoded strings, this is the number of wide chracters.
 * For multi-byte encoded strings this is the number of encoded characters, not the number of bytes.
 */
s32 ustrlen(uchar const* s) noexcept;

/**
 * \brief True if the given strings/substrings are equal.
 *
 * Given strings don't have to be the length of \a len. If either of the strings is shorter than \a
 * len, this function returns false.
 *
 * \param s1 String.
 * \param s2 String.
 * \param len Number of characters to compare.
 */
bool ustrcmp(uchar const* s1, uchar const* s2, s32 len) noexcept;

Status wide_to_ansi(std::wstring const& ws, std::string& s) noexcept;
Status wide_to_ansi(uchar const* ws, std::string& s) noexcept;
Status wide_to_ansi(uchar const* ws, s32 ws_char_len, std::string& s) noexcept;

Status ansi_to_wide(std::string const& s, std::wstring& ws) noexcept;
Status ansi_to_wide(char const* s, std::wstring& ws) noexcept;
Status ansi_to_wide(char const* s, s32 s_byte_size, std::wstring& ws) noexcept;

Status wide_to_utf8(std::wstring const& ws, std::string& s) noexcept;
Status wide_to_utf8(uchar const* ws, std::string& s) noexcept;
Status wide_to_utf8(uchar const* ws, s32 ws_char_len, std::string& s) noexcept;

Status utf8_to_wide(std::string const& s, std::wstring& ws) noexcept;
Status utf8_to_wide(char const* s, std::wstring& ws) noexcept;
Status utf8_to_wide(char const* s, s32 s_byte_size, std::wstring& ws) noexcept;
} // namespace unicode
} // namespace rk
