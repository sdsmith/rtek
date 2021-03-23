#pragma once

#include "core/assert.h"
#include "core/status.h"
#include <nowide/args.hpp>
#include <string>
#include <string_view>

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

// TODO(sdsmith): rename utf?
namespace unicode
{
// TODO(sdsmith): chracters get replaced with NOWIDE_REPLACEMENT_CHARACTER. Should this be exposed?

/**
 * \brief Replaces standard `main()`'s function args with UTF-8 encoded chracters on Windows for the
 * lifetime of the instance.
 *
 * \see nowide::args
 */
using Args = nowide::args;

/**
 * \brief Convert wide string (UTF-16/32) in range [begin,end) to NULL terminated narrow string
 * (UTF-8) stored in \a output of size \a output_size (including NULL)
 *
 * If there is not enough room NULL is returned, else output is returned.
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
inline char* narrow(char* output, s32 output_size, wchar_t const* begin,
                    wchar_t const* end) noexcept;

/**
 * \brief Convert NULL terminated wide string (UTF-16/32) to NULL terminated narrow string (UTF-8)
 * stored in \a output of size \a output_size (including NULL)
 *
 * If there is not enough room NULL is returned, else output is returned.
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
inline char* narrow(char* output, s32 output_size, wchar_t const* source) noexcept;

/**
 * \brief Convert narrow string (UTF-8) in range [begin,end) to NULL terminated wide string
 * (UTF-16/32) stored in \a output of size \a output_size (including NULL)
 *
 * If there is not enough room NULL is returned, else output is returned.
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
inline wchar_t* widen(wchar_t* output, s32 output_size, char const* begin,
                      char const* end) noexcept;

/**
 * \brief Convert NULL terminated narrow string (UTF-8) to NULL terminated wide string (UTF-16/32)
 * most output_size (including NULL)
 *
 * If there is not enough room NULL is returned, else output is returned.
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
inline wchar_t* widen(wchar_t* output, s32 output_size, char const* source) noexcept;

/**
 * \brief Convert wide string (UTF-16/32) to narrow string (UTF-8).
 *
 * \param s Input string
 * \param count Number of characters to convert
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
inline std::string narrow(wchar_t const* s, s32 count);

/**
 * \brief Convert wide string (UTF-16/32) to narrow string (UTF-8).
 *
 * \param s NULL terminated input string
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
inline std::string narrow(wchar_t const* s);

/**
 * \brief Convert wide string (UTF-16/32) to narrow string (UTF-8).
 *
 * \param s Input string
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
inline std::string narrow(std::wstring const& s);

/**
 * \brief Convert narrow string (UTF-8) to wide string (UTF-16/32).
 *
 * \param s Input string
 * \param count Number of characters to convert
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
inline std::wstring widen(char const* s, s32 count);

/**
 * \brief Convert narrow string (UTF-8) to wide string (UTF-16/32).
 *
 * \param s NULL terminated input string
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
inline std::wstring widen(char const* s);

/**
 * \brief Convert narrow string (UTF-8) to wide string (UTF-16/32).
 *
 * \param s Input string
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
inline std::wstring widen(std::string const& s);

// TODO(sdsmith): --------------------------------------------------------------------------------

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
