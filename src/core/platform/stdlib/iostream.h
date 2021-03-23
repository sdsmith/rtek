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
using cin = nowide::cin;

/**
 * \brief Same as std::cout but uses UTF-8.
 *
 * NOTE(sdsmith): stream is not synced with stdio and therefore not affected by
 * std::ios::sync_with_stdio.
 *
 * \see std::cout
 */
using cout = nowide::cout;

/**
 * \brief Same as std::cerr but uses UTF-8.
 *
 * NOTE(sdsmith): stream is not synced with stdio and therefore not affected by
 * std::ios::sync_with_stdio.
 *
 * \see std::cerr
 */

using cerr = nowide::cerr;

/**
 * \brief Same as std::clog but uses UTF-8.
 *
 * NOTE(sdsmith): stream is not synced with stdio and therefore not affected by
 * std::ios::sync_with_stdio.
 *
 * \see std::clog
 */
using clog = nowide::clog;

} // namespace rk
