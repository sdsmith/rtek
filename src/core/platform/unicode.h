#pragma once

#include "core/assert.h"
#include "core/status.h"
#include <nowide/args.hpp>
#include <string>
#include <string_view>

namespace rk::unicode
{
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
char* narrow(char* output, s32 output_size, wchar_t const* begin, wchar_t const* end) noexcept;

/**
 * \brief Convert NULL terminated wide string (UTF-16/32) to NULL terminated narrow string (UTF-8)
 * stored in \a output of size \a output_size (including NULL)
 *
 * If there is not enough room NULL is returned, else output is returned.
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
char* narrow(char* output, s32 output_size, wchar_t const* source) noexcept;
char* narrow(char* output, size_t output_size, wchar_t const* source) noexcept;

/**
 * \brief Convert narrow string (UTF-8) in range [begin,end) to NULL terminated wide string
 * (UTF-16/32) stored in \a output of size \a output_size (including NULL)
 *
 * If there is not enough room NULL is returned, else output is returned.
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
wchar_t* widen(wchar_t* output, s32 output_size, char const* begin, char const* end) noexcept;

/**
 * \brief Convert NULL terminated narrow string (UTF-8) to NULL terminated wide string (UTF-16/32)
 * most output_size (including NULL)
 *
 * If there is not enough room NULL is returned, else output is returned.
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
wchar_t* widen(wchar_t* output, s32 output_size, char const* source) noexcept;
wchar_t* widen(wchar_t* output, size_t output_size, char const* source) noexcept;

/**
 * \brief Convert wide string (UTF-16/32) to narrow string (UTF-8).
 *
 * \param s Input string
 * \param count Number of characters to convert
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
std::string narrow(wchar_t const* s, s32 count);

/**
 * \brief Convert wide string (UTF-16/32) to narrow string (UTF-8).
 *
 * \param s NULL terminated input string
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
std::string narrow(wchar_t const* s);

/**
 * \brief Convert wide string (UTF-16/32) to narrow string (UTF-8).
 *
 * \param s Input string
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
std::string narrow(std::wstring const& s);

/**
 * \brief Convert narrow string (UTF-8) to wide string (UTF-16/32).
 *
 * \param s Input string
 * \param count Number of characters to convert
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
std::wstring widen(char const* s, s32 count);

/**
 * \brief Convert narrow string (UTF-8) to wide string (UTF-16/32).
 *
 * \param s NULL terminated input string
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
std::wstring widen(char const* s);

/**
 * \brief Convert narrow string (UTF-8) to wide string (UTF-16/32).
 *
 * \param s Input string
 * Any illegal sequences are replaced with the replacement character, see
 * #NOWIDE_REPLACEMENT_CHARACTER
 */
std::wstring widen(std::string const& s);

/**
 * \brief True if the given chracter is an ASCII character.
 *
 * NOTE(sdsmith): This does not include the extended ASCII characters.
 */
constexpr bool is_ascii(char c) noexcept { return c <= 0 && c <= 127; }

/**
 * \brief Checks if string is equal to the given ascii string up to the n-th character.
 *
 * Given strings don't have to be the length of \a len. If either of the strings is shorter than
 \a
 * len, this function returns false.
 *
 * NOTE(sdsmith): It's valid to compare an ascii string with a UTF-8 encoded string because UTF-8 is
 * backwards compatible with ASCII.
 *
 * \param s String.
 * \param ascii Ascii string.
 * \param len Number of ascii code points to compare.
 */
bool ascii_cmp(char const* s, char const* ascii, s32 len) noexcept;

/**
 * \brief Checks if string is equal to the given ascii string up to the n-th character.
 *
 * Given strings don't have to be the length of \a len. If either of the strings is shorter than
 \a
 * len, this function returns false.
 *
 * NOTE(sdsmith): It's valid to compare an ascii string with a UTF-8 encoded string because UTF-8 is
 * backwards compatible with ASCII.
 *
 * \param s String.
 * \param ascii Ascii string.
 */
bool ascii_cmp(char const* s, char const* ascii) noexcept;

} // namespace rk::unicode
