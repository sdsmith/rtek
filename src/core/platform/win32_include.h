/**
 * \file win32_include.h
 * \brief Wrapper for Windows.h and related includes for the Windows platform.
 *
 * NOTE(sdsmith): Don't include Windows.h directly. Use this instead to make sure it is defined
 * consistently across the codebase.
 */

// Unicode only. No ANSI allowed.
#ifndef UNICODE
#    define UNICODE
#endif
#ifndef _UNICODE
#    define _UNICODE
#endif

// Stop definition of max and min from windows.h
#ifndef NOMINMAX
#   define NOMINMAX
#       include <Windows.h>
#   undef NOMINMAX
#else
#   include <Windows.h>
#endif
