#include "core/platform/unicode.h"

#include "core/utility/fixme.h"
#include "core/utility/no_exception.h"

using namespace rk;

Status unicode::wide_to_ansi(std::wstring const& ws, std::string& s) noexcept
{
    auto ret = exception_boundary([&]() {
        s32 const count =
            WideCharToMultiByte(CP_ACP, 0, ws.c_str(), fixme::scast<s32>(ws.size(), "safe cast"),
                                nullptr, 0, nullptr, nullptr);
        if (count == 0) {
            platform::windows::log_last_error(UC("WideCharToMultiByte"));
            return Status::unicode_error;
        }
        s.resize(count, '\0');
        if (WideCharToMultiByte(CP_ACP, 0, ws.c_str(), -1, s.data(), count, nullptr, nullptr) ==
            0) {
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
    auto ret = exception_boundary([&]() {
        s32 const count = MultiByteToWideChar(CP_ACP, 0, s.c_str(),
                                              fixme::scast<s32>(s.size(), "safe cast"), nullptr, 0);
        if (count == 0) {
            platform::windows::log_last_error(UC("MultiByteToWideChar"));
            return Status::unicode_error;
        }
        ws.resize(count, '\0');
        if (MultiByteToWideChar(CP_ACP, 0, s.c_str(), fixme::scast<s32>(s.size(), "safe cast"),
                                ws.data(), count) == 0) {
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
    auto ret = exception_boundary([&]() {
        s32 const count =
            WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), fixme::scast<s32>(ws.size(), "safe cast"),
                                nullptr, 0, nullptr, nullptr);
        if (count == 0) {
            platform::windows::log_last_error(UC("WideCharToMultiByte"));
            return Status::unicode_error;
        }
        s.resize(count, '\0');
        if (WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, s.data(), count, nullptr, nullptr) ==
            0) {
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
    auto ret = exception_boundary([&]() {
        s32 const count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(),
                                              fixme::scast<s32>(s.size(), "safe cast"), nullptr, 0);
        if (count == 0) {
            platform::windows::log_last_error(UC("MultiByteToWideChar"));
            return Status::unicode_error;
        }
        ws.resize(count, 0);
        if (MultiByteToWideChar(CP_UTF8, 0, s.c_str(), fixme::scast<s32>(s.size(), "safe cast"),
                                ws.data(), count) == 0) {
            platform::windows::log_last_error(UC("MultiByteToWideChar"));
            return Status::unicode_error;
        }
        return Status::ok;
    });
    auto p_status = std::get_if<Status>(&ret);
    RK_ASSERT(p_status);
    return *p_status;
}
