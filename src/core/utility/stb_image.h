#pragma once

/**
 * \file stb_image.h
 * \brief Header wrapper that configures stb_image.
 *
 * NOTE(sdsmith): Include this file instead of the actual stb_image.h directly!
 */

#if SDS_OS_WINDOWS
// Use unicode filenames
//
// NOTE(sdsmith): Call stbi_convert_wchar_to_utf8 to convert Windows wchar_t to utf8.
#    define STBI_WINDOWS_UTF8
#endif

#include <stb/stb_image.h>
