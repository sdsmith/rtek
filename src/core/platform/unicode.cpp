#include "core/platform/unicode.h"

#include "core/assert.h"
#include "core/utility/fixme.h"
#include "sds/string.h"
#include <nowide/convert.hpp>

using namespace rk;
using namespace sds;

// TODO(sdsmith): chracters get replaced with NOWIDE_REPLACEMENT_CHARACTER. Should this be exposed?

char* unicode::narrow(char* output, s32 output_size, wchar_t const* begin,
                      wchar_t const* end) noexcept
{
    RK_ASSERT(output);
    RK_ASSERT(output_size > 0);
    RK_ASSERT(begin);
    RK_ASSERT(end);
    return nowide::narrow(output, output_size, begin, end);
}

char* unicode::narrow(char* output, s32 output_size, wchar_t const* source) noexcept
{
    RK_ASSERT(output);
    RK_ASSERT(output_size > 0);
    RK_ASSERT(source);
    return nowide::narrow(output, output_size, source);
}

char* unicode::narrow(char* output, size_t output_size, wchar_t const* source) noexcept
{
    return narrow(output, fixme::scast<s32>(output_size, "safe cast + own stdlib"), source);
}

wchar_t* unicode::widen(wchar_t* output, s32 output_size, char const* begin,
                        char const* end) noexcept
{
    RK_ASSERT(output);
    RK_ASSERT(output_size > 0);
    RK_ASSERT(begin);
    RK_ASSERT(end);
    return nowide::widen(output, output_size, begin, end);
}

wchar_t* unicode::widen(wchar_t* output, s32 output_size, char const* source) noexcept
{
    RK_ASSERT(output);
    RK_ASSERT(output_size > 0);
    RK_ASSERT(source);
    return nowide::widen(output, output_size, source);
}

wchar_t* unicode::widen(wchar_t* output, size_t output_size, char const* source) noexcept
{
    return widen(output, fixme::scast<s32>(output_size, "safe cast + stdlib"), source);
}

std::string unicode::narrow(wchar_t const* s, s32 count)
{
    RK_ASSERT(s);
    RK_ASSERT(count > 0);
    return nowide::narrow(s, count);
}

std::string unicode::narrow(wchar_t const* s)
{
    RK_ASSERT(s);
    return nowide::narrow(s);
}

std::string unicode::narrow(std::wstring const& s) { return nowide::narrow(s); }

std::wstring unicode::widen(char const* s, s32 count)
{
    RK_ASSERT(s);
    RK_ASSERT(count > 0);
    return nowide::widen(s, count);
}

std::wstring unicode::widen(char const* s)
{
    RK_ASSERT(s);
    return nowide::widen(s);
}

std::wstring unicode::widen(std::string const& s) { return nowide::widen(s); }

bool unicode::ascii_cmp(char const* s, char const* ascii, s32 len) noexcept
{
    RK_ASSERT(s);
    RK_ASSERT(ascii);
    RK_ASSERT(len > 0);

    char const* p1 = s;
    char const* p2 = ascii;
    s32 n = 0;
    while (n < len) {
        RK_ASSERT(is_ascii(*p2));
        if (*p1 == '\0' || *p2 == '\0' || *p1 != *p2) { return false; }
        ++n;
        ++p1;
        ++p2;
    }

    return true;
}

bool unicode::ascii_cmp(char const* s, char const* ascii) noexcept
{
    return ascii_cmp(s, ascii, sds::str_size(ascii));
}
