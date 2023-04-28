#include "core/platform/platform.h"

#include <type_traits>

#include "core/assert.h"
#include "core/logging/logging.h"
#include "core/platform/unicode.h"
#include "core/utility/fixme.h"
#include <sds/array.h>
#include <sds/array/make_array.h>

using namespace rk;
using namespace sds;

#if SDS_OS_WINDOWS
#    include <strsafe.h>

void platform::windows::log_error(HRESULT hresult) noexcept
{
    if (FACILITY_WINDOWS == HRESULT_FACILITY(hresult)) { hresult = HRESULT_CODE(hresult); }

    WCHAR* msg = nullptr;
    DWORD size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                nullptr, hresult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                reinterpret_cast<LPWSTR>(&msg), 0, nullptr);
    if (size == 0) {
        LOG_ERROR("Unable to find description for HRESULT error {}", hresult);
    } else {
        LOG_ERROR(msg);
        LocalFree(msg);
    }
}

void platform::windows::log_last_error(char const* function_name) noexcept
{
    std::array<wchar_t, 256> w_func_name;
    if (!unicode::widen(w_func_name.data(), w_func_name.size(), function_name)) {
        static constexpr auto const errstr = sds::make_array(L"<widen:buf too small>");
        memcpy(w_func_name.data(), errstr.data(), sds::byte_size(errstr));
    }

    // Retrieve error message from system
    //
    LPVOID msg_buf;
    LPVOID display_buf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&msg_buf),
        0, nullptr);

    // Print error message
    //
    display_buf = static_cast<LPVOID>(LocalAlloc(
        LMEM_ZEROINIT,
        (wcslen(static_cast<LPCWSTR>(msg_buf)) + wcslen(static_cast<LPCWSTR>(w_func_name.data())) + 40) *
            sizeof(WCHAR)));

    StringCchPrintf(static_cast<LPWSTR>(display_buf), //-V111 -V576
                    LocalSize(display_buf) / sizeof(WCHAR), TEXT("%s failed with error %d: %s"),
                    function_name, dw, msg_buf);

    LOG_ERROR(static_cast<LPWSTR>(display_buf));

    LocalFree(msg_buf);
    LocalFree(display_buf);
}

#endif // SDS_OS_WINDOWS
