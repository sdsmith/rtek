/**
 * \file win32_include.h
 * \brief Wrapper for Windows.h and related includes for the Windows platform.
 *
 * NOTE(sdsmith): Don't include Windows.h directly. Use this instead to make sure it is defined
 * consistently across the codebase.
 */

// Unicode support
#ifndef _UNICODE
#    error Unicode only. Define _UNICODE.
#endif
#ifndef UNICODE
#    error Unicode only. Define UNICODE.
#endif

// Stop definition of max and min from windows.h
#define NOMINMAX

#include <Windows.h>

#undef NOMINMAX
