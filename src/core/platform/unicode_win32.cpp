#include "core/platform/unicode.h"

#include "core/utility/fixme.h"
#include "core/utility/no_exception.h"
#include <sds/string.h>

using namespace rk;

Status unicode::wide_to_ansi(std::wstring const& ws, std::string& s) noexcept
{
    return wide_to_ansi(ws.c_str(),
                        fixme::scast<s32>(ws.size(), "safe cast") + 1, // count null-terminator
                        s);
}

Status unicode::wide_to_ansi(uchar const* ws, std::string& s) noexcept
{
    RK_ASSERT(ws);
    return wide_to_ansi(ws, platform::ustrlen(ws), s);
}

Status unicode::wide_to_ansi(uchar const* ws, s32 ws_char_len, std::string& s) noexcept
{
    RK_ASSERT(ws);
    RK_ASSERT(ws > 0);

    auto ret = exception_boundary([&]() {
        s32 const count =
            WideCharToMultiByte(CP_ACP, 0, ws, ws_char_len, nullptr, 0, nullptr, nullptr);
        if (count == 0) {
            platform::windows::log_last_error(UC("WideCharToMultiByte"));
            return Status::unicode_error;
        }
        s.resize(count, '\0');
        if (WideCharToMultiByte(CP_ACP, 0, ws, -1, s.data(), count, nullptr, nullptr) == 0) {
            platform::windows::log_last_error(UC("WideCharToMultiByte"));
            return Status::unicode_error;
        }
        return Status::ok;
    });
    auto p_status = std::get_if<Status>(&ret);
    RK_ASSERT(p_status);
    return *p_status;
}

Status unicode::ansi_to_wide(std::string const& s, std::wstring& ws) noexcept
{
    return ansi_to_wide(s.c_str(),
                        fixme::scast<s32>(s.size(), "safe cast") + 1, // count null-terminator
                        ws);
}

Status unicode::ansi_to_wide(char const* s, std::wstring& ws) noexcept
{
    RK_ASSERT(s);
    return ansi_to_wide(s, sds::byte_size(s), ws);
}

Status unicode::ansi_to_wide(char const* s, s32 s_byte_size, std::wstring& ws) noexcept
{
    RK_ASSERT(s);
    RK_ASSERT(s_byte_size > 0);

    auto ret = exception_boundary([&]() {
        s32 const count = MultiByteToWideChar(CP_ACP, 0, s, s_byte_size, nullptr, 0);
        if (count == 0) {
            platform::windows::log_last_error(UC("MultiByteToWideChar"));
            return Status::unicode_error;
        }
        ws.resize(count, '\0');
        if (MultiByteToWideChar(CP_ACP, 0, s, s_byte_size, ws.data(), count) == 0) {
            platform::windows::log_last_error(UC("MultiByteToWideChar"));
            return Status::unicode_error;
        }
        return Status::ok;
    });
    auto p_status = std::get_if<Status>(&ret);
    RK_ASSERT(p_status);
    return *p_status;
}

Status unicode::wide_to_utf8(std::wstring const& ws, std::string& s) noexcept
{
    return wide_to_utf8(ws.c_str(),
                        fixme::scast<s32>(ws.size(), "safe cast") + 1, // count null-terminator
                        s);
}

Status unicode::wide_to_utf8(uchar const* ws, std::string& s) noexcept
{
    RK_ASSERT(ws);
    return wide_to_utf8(ws, platform::ustrlen(ws), s);
}

Status unicode::wide_to_utf8(uchar const* ws, s32 ws_char_len, std::string& s) noexcept
{
    RK_ASSERT(ws);
    RK_ASSERT(ws_char_len > 0);

    auto ret = exception_boundary([&]() {
        s32 const count =
            WideCharToMultiByte(CP_UTF8, 0, ws, ws_char_len, nullptr, 0, nullptr, nullptr);
        if (count == 0) {
            platform::windows::log_last_error(UC("WideCharToMultiByte"));
            return Status::unicode_error;
        }
        s.resize(count, '\0');
        if (WideCharToMultiByte(CP_UTF8, 0, ws, -1, s.data(), count, nullptr, nullptr) == 0) {
            platform::windows::log_last_error(UC("WideCharToMultiByte"));
            return Status::unicode_error;
        }
        return Status::ok;
    });
    auto p_status = std::get_if<Status>(&ret);
    RK_ASSERT(p_status);
    return *p_status;
}

Status unicode::utf8_to_wide(std::string const& s, std::wstring& ws) noexcept
{
    return utf8_to_wide(s.c_str(),
                        fixme::scast<s32>(s.size(), "safe cast") + 1, // count null-terminator
                        ws);
}

Status unicode::utf8_to_wide(char const* s, std::wstring& ws) noexcept
{
    RK_ASSERT(s);
    return utf8_to_wide(s, sds::byte_size(s), ws);
}

Status unicode::utf8_to_wide(char const* s, s32 s_byte_size, std::wstring& ws) noexcept
{
    RK_ASSERT(s);
    RK_ASSERT(s_byte_size > 0);

    auto ret = exception_boundary([&]() {
        s32 const count = MultiByteToWideChar(CP_UTF8, 0, s, s_byte_size, nullptr, 0);
        if (count == 0) {
            platform::windows::log_last_error(UC("MultiByteToWideChar"));
            return Status::unicode_error;
        }
        ws.resize(count, 0);
        if (MultiByteToWideChar(CP_UTF8, 0, s, s_byte_size, ws.data(), count) == 0) {
            platform::windows::log_last_error(UC("MultiByteToWideChar"));
            return Status::unicode_error;
        }
        return Status::ok;
    });
    auto p_status = std::get_if<Status>(&ret);
    RK_ASSERT(p_status);
    return *p_status;
}
