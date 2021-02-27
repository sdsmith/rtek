#pragma once

#include <cstdint>

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
// MSVC incorrectly reports __cplusplus
// ref: https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
#    if RK_COMPILER == RK_COMPILER_MSC
#        define RK_CPLUSPLUS _MSVC_LANG
#    else
#        define RK_CPLUSPLUS __cplusplus
#    endif
#endif

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

} // namespace rk
