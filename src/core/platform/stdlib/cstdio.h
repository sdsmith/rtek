#pragma once

#include "core/types.h"
#include <cstdio>

/**
 * \file cstdio.h
 * \brief Provides standard library functions that are compatible with UTF-8.
 *
 * Windows standard library file related functions either support ANSI or wide character encodings,
 * not UTF-8. The Linux standard library supports UFT-8 out-of-the-box.
 */

namespace rk
{
/**
 * \brief Same as freopen but file_name and mode are UTF-8 strings
 *
 * \see std::freopen
 */
FILE* freopen(char const* file_name, char const* mode, FILE* stream) noexcept;

/**
 * \brief Same as fopen but file_name and mode are UTF-8 strings
 *
 * \see std::fopen
 */
FILE* fopen(char const* file_name, char const* mode) noexcept;

/**
 * \brief Same as rename but old_name and new_name are UTF-8 strings
 *
 * \see std::rename
 */
s32 rename(char const* old_name, char const* new_name) noexcept;

/**
 * \brief Same as remove but name is UTF-8 string
 *
 * \see std::remove
 */
s32 remove(char const* name) noexcept;
} // namespace rk
