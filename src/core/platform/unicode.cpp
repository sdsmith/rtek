#include "core/platform/unicode.h"

#include "core/assert.h"
#include <nowide/convert.hpp>

using namespace rk;

inline char* unicode::narrow(char* output, s32 output_size, wchar_t const* begin,
                             wchar_t const* end) noexcept
{
    RK_ASSERT(output);
    RK_ASSERT(output_size > 0);
    RK_ASSERT(begin);
    RK_ASSERT(end);
    return nowide::narrow(output, output_size, begin, end);
}

inline char* unicode::narrow(char* output, s32 output_size, wchar_t const* source) noexcept
{
    RK_ASSERT(output);
    RK_ASSERT(output_size > 0);
    RK_ASSERT(source);
    return nowide::narrow(output, output_size, source);
}

inline wchar_t* unicode::widen(wchar_t* output, s32 output_size, char const* begin,
                               char const* end) noexcept
{
    RK_ASSERT(output);
    RK_ASSERT(output_size > 0);
    RK_ASSERT(begin);
    RK_ASSERT(end);
    return nowide::widen(output, output_size, begin, end);
}

inline wchar_t* unicode::widen(wchar_t* output, s32 output_size, char const* source) noexcept
{
    RK_ASSERT(output);
    RK_ASSERT(output_size > 0);
    RK_ASSERT(source);
    return nowide::widen(output, output_size, source);
}

inline std::string unicode::narrow(wchar_t const* s, s32 count)
{
    RK_ASSERT(s);
    RK_ASSERT(count > 0);
    return nowide::narrow(s, count);
}

inline std::string unicode::narrow(wchar_t const* s)
{
    RK_ASSERT(s);
    return nowide::narrow(s);
}

inline std::string unicode::narrow(std::wstring const& s) { return nowide::narrow(s); }

inline std::wstring unicode::widen(char const* s, s32 count)
{
    RK_ASSERT(s);
    RK_ASSERT(count > 0);
    return nowide::widen(s, count);
}

inline std::wstring unicode::widen(char const* s)
{
    RK_ASSERT(s);
    return nowide::widen(s);
}

inline std::wstring unicode::widen(std::string const& s) { return nowide::widen(s); }

bool unicode::ustrcmp(uchar const* s1, uchar const* s2, s32 len) noexcept
{
    RK_ASSERT(s1);
    RK_ASSERT(s2);
    RK_ASSERT(len > 0);

    uchar const* p1 = s1;
    uchar const* p2 = s2;
    s32 n = 0;
    while (n < len) {
        if (*p1 == UC('\0') || *p2 == UC('\0') || *p1 != *p2) { return false; }
        ++n;
        ++p1;
        ++p2;
    }

    return true;
}
