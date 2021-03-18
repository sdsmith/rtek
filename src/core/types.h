#pragma once

#include <cstdint>

/**
 * \def RK_OS
 * \brief Operating system being compiled on.
 *
 * Has a value from one of \a RK_OS_*.
 * Usage: `#if RK_OS == RK_OS_WINDOWS`
 */
#ifndef RK_OS
#    define RK_OS_WINDOWS 1
#    define RK_OS_LINUX 2
#    define RK_OS_MAC 3

#    if defined(_WIN32)
#        define RK_OS RK_OS_WINDOWS
#    elif defined(__linux__)
#        define RK_OS RK_OS_LINUX
#    elif defined(__APPLE__)
#        define RK_OS RK_OS_MAC
#    else
#        error Unknown operating system
#    endif
#endif

#if RK_OS == RK_OS_WINDOWS
// Unicode support
// TODO(sdsmith): define at compilation and check here
#    ifndef _UNICODE
#        define _UNICODE
#    endif
#    ifndef UNICODE
#        define UNICODE
#    endif
#    include <Windows.h>
#endif

namespace rk
{
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s32f = int_fast32_t; /** At least 32bit, larger if faster */
using s64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u32f = uint_fast32_t; /** At least 32bit, larger if faster */
using u64 = uint64_t;

using f32 = float;
using f64 = double;

constexpr auto operator""_KB(u64 s) { return s * 1024; }

constexpr auto operator""_MB(u64 s) { return s * 1024_KB; }

constexpr auto operator""_GB(u64 s) { return s * 1024_MB; }

// ---------------------------------------------------------------------------------------
// Unicode support
// ---------------------------------------------------------------------------------------

#if RK_OS == RK_OS_WINDOWS
/**
 * \brief Unicode character.
 *
 * On Windows, wchar_t is two bytes for UTF-16. Other platforms use wchar_t as 4 bytes to support
 * UTF-32. Use Window's \a WCHAR to support their 'unique' unicode scheme and support their unicode
 * functions.
 *
 * \see _u
 */
using uchar = WCHAR;
#    define RK_I_UC(x) L##x // UTF-16 encoding
#elif RK_OS == RK_OS_LINUX
/**
 * \brief Unicode character.
 *
 * On linux, just use UTF-8 since it's so much easier and better supported.
 *
 * \see _u
 */
using uchar = char;
#    define RK_I_UC(x) = u8##x // UTF-8 encoding
#else
#    error Unsupported OS
#endif

/**
 * \def UC
 * \brief String litteral prefix for unicode strings.
 *
 * Will define the appropriate encoding based on the unicode settings.
 *
 * Usage: `UC("hello world")`
 *
 * \see uchar
 */
#define UC(x) RK_I_UC(x)

// ---------------------------------------------------------------------------------------

/**
 * \def RK_COMPILER
 * \brief Compiler compiling the project.
 *
 * Has a value from one of \a RK_COMPILER_*
 * Usage: `#if RK_COMPILER == RK_COMPILER_MSC`
 */
#ifndef RK_COMPILER
/** Microsoft C/C++ compiler */
#    define RK_COMPILER_MSC 1
#    define RK_COMPILER_GCC 2
#    define RK_COMPILER_CLANG 3

#    if defined(_MSC_VER)
#        define RK_COMPILER RK_COMPILER_MSC
#    elif defined(__GNUC__)
#        define RK_COMPILER RK_COMPILER_GCC
#    elif defined(__clang__)
#        define RK_COMPILER RK_COMPILER_CLANG
#    else
#        error Unknown compiler
#    endif
#endif

/**
 * \def RK_CPLUSPLUS
 * \brief Portable replacement for \a __cplusplus.
 *
 * Microsoft does it again...
 */
#ifndef RK_CPLUSPLUS
#    define RK_CPLUSPLUS_11 201103L
#    define RK_CPLUSPLUS_14 201402L
#    define RK_CPLUSPLUS_17 201703L
#    define RK_CPLUSPLUS_20 202002L

// MSVC incorrectly reports __cplusplus
// ref: https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
#    if RK_COMPILER == RK_COMPILER_MSC
#        define RK_CPLUSPLUS _MSVC_LANG
#    else
#        define RK_CPLUSPLUS __cplusplus
#    endif
#endif

/**
 * \def RK_FILENAME
 * \brief Name of the current file.
 */
#define RK_FILENAME __FILE__

/**
 * \def RK_FUNCNAME
 * \brief Portable function name.
 */
#if RK_COMPILER == RK_COMPILER_MSC
#    define RK_FUNCNAME __FUNCTION__
#else
#    define RK_FUNCNAME __FUNC__
#endif

/**
 * \brief Platform specific path separator.
 */
#if RK_OS == RK_OS_WINDOWS
// NOTE(sdsmith): Latest Windows versions also support '/', although many older Windows API
// functions do not.
#    define RK_PATH_SEPARATOR "\\"
#else
#    define RK_PATH_SEPARATOR "/"
#endif

/**
 * \def RK_RESTRICT
 * \brief Portable `restrict` keyword.
 *
 * NOTE: Don't lie to your compiler!
 *
 * ref: https://en.wikipedia.org/wiki/Restrict
 */
#ifndef RK_RESTRICT
#    if RK_COMPILER == RK_COMPILER_MSC
#        define RK_RESTRICT __restrict
#    elif RK_COMPILER == RK_COMPILER_GCC || RK_COMPILER == RK_COMPILER_CLANG
#        define RK_RESTRICT __restrict__
#    else
#        error Unsupported compiler
#    endif
#endif

/**
 * \def RK_INTERNAL
 * \brief Set internal linkage on symbol.
 *
 * Replaces the use of 'static' to define internal linkage, because 'static' is a poor keyword
 * choice.
 */
#define RK_INTERNAL static

/**
 * \def RK_LIKELY
 * \brief Portable `likely` branch attribute for if statements. Does not work on case statements.
 *
 * Usage: `if RK_LIKELY(...) { ... }`
 */
/**
 * \def RK_UNLIKELY
 * \brief Portable `unlikely` branch attribute for if statements. Does not work on case statements.
 *
 * Usage: `if RK_UNLIKELY(...) { ... }`
 */

#ifndef RK_LIKELY
#    if RK_CPLUSPLUS >= RK_CPLUSPLUS_20
// C++20 supports the attribute
#        define RK_LIKELY(x) (x) [[likely]]
#    elif RK_COMPILER == RK_COMPILER_GCC || RK_COMPILER == RK_COMPILER_CLANG
#        define RK_LIKELY(x) (__builtin_expect(!!(x), 1))
#    else
#        define RK_LIKELY(x) (x)
#    endif
#endif

#ifndef RK_UNLIKELY
#    if RK_CPLUSPLUS >= RK_CPLUSPLUS_20
// C++20 supports the attribute
#        define RK_UNLIKELY(x) (x) [[unlikely]]
#    elif RK_COMPILER == RK_COMPILER_GCC || RK_COMPILER == RK_COMPILER_CLANG
#        define RK_UNLIKELY(x) (__builtin_expect(!!(x), 0))
#    else
#        define RK_UNLIKELY(x) (x)
#    endif
#endif

// TODO(sdsmith): @cpp20: The c++20 attribute [[likely/unlikely]] has a different syntax for usage
// on ifs vs switch cases. It also differs from usage with the previous __builtin_expect. This means
// to support switch cases portably there would need to be a separate marco from
// RK_LIKELY/RK_UNLIKELY and it would need to be duplicated; once on the switch condition and once
// on the case. This is annoying and fragil. There is no easy way to enfornce that this happens or
// that the attributes match (ie likely on the conditional and unlikely on a case statement would be
// problematic!). In lieu of this, I'm going to not make RK_LIKELY_CASE for now. One day we could
// switch to C++20 and be done with it.

/**
 * \def RK_DATA_BASE_DIR
 * \brief Base path to the data directory.
 */
#ifndef RK_DATA_BASE_DIR
#    define RK_DATA_BASE_DIR "data"
#endif

/**
 * \def RK_SHADER_BASE_DIR
 * \brief Base path to the shader directory.
 */
#ifndef RK_SHADER_BASE_DIR
#    define RK_SHADER_BASE_DIR "data/shaders"
#endif

} // namespace rk
