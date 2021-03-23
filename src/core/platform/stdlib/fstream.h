#pragma once

#include <nowide/fstream.hpp>

namespace rk
{
/**
 * \brief Same as std::filebuf but accepts UTF-8 strings.
 *
 * \see std::filebuf
 */
using filebuf = nowide::filebuf;

/**
 * \brief Same as std::ifstream but accepts UTF-8 strings.
 *
 * \see std::ifstream
 */
using ifstream = nowide::ifstream;

/**
 * \brief Same as std::ofstream but accepts UTF-8 strings.
 *
 * \see std::ofstream
 */
using ofstream = nowide::ofstream;

/**
 * \brief Same as std::fstream but accepts UTF-8 strings.
 *
 * \see std::fstream
 */
using fstream = nowide::fstream;

}; // namespace rk
