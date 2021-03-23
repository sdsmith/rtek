#pragma once

#include <nowide/iostream.hpp>

namespace rk
{
/**
 * \brief Same as std::cin but uses UTF-8.
 *
 * NOTE(sdsmith): stream is not synced with stdio and therefore not affected by
 * std::ios::sync_with_stdio.
 *
 * \see std::cin
 */
using nowide::cin;

/**
 * \brief Same as std::cout but uses UTF-8.
 *
 * NOTE(sdsmith): stream is not synced with stdio and therefore not affected by
 * std::ios::sync_with_stdio.
 *
 * \see std::cout
 */
using nowide::cout;

/**
 * \brief Same as std::cerr but uses UTF-8.
 *
 * NOTE(sdsmith): stream is not synced with stdio and therefore not affected by
 * std::ios::sync_with_stdio.
 *
 * \see std::cerr
 */

using nowide::cerr;

/**
 * \brief Same as std::clog but uses UTF-8.
 *
 * NOTE(sdsmith): stream is not synced with stdio and therefore not affected by
 * std::ios::sync_with_stdio.
 *
 * \see std::clog
 */
using nowide::clog;

} // namespace rk
