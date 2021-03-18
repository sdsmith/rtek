#include "core/platform/platform.h"

#include <type_traits>

#include "core/assert.h"
#include "core/logging/logging.h"
#include "core/utility/fixme.h"

using namespace rk;

#if RK_OS == RK_OS_WINDOWS
#    include <tchar.h>

s32 platform::ustrlen(uchar const* s) noexcept
{
    RK_ASSERT(s);
    return fixme::scast<s32>(_tcsclen(s), "safe cast to narrow signed value");
}

void platform::windows::log_error(HRESULT hresult) noexcept
{
    if (FACILITY_WINDOWS == HRESULT_FACILITY(hresult)) { hresult = HRESULT_CODE(hresult); }
    TCHAR* msg = nullptr;

    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hresult,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&msg), 0,
                      nullptr) != 0) {
        LOG_ERROR(msg);
        LocalFree(msg);
    } else {
        LOG_ERROR("Unable to find description for HRESULT error {}", hresult);
    }
}

void platform::windows::log_last_error(uchar const* function_name)
{
    // Retrieve error message from system
    //
    LPVOID msg_buf;
    LPVOID display_buf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&msg_buf),
        0, nullptr);

    // Print error message
    //
    display_buf = static_cast<LPVOID>(
        LocalAlloc(LMEM_ZEROINIT, (_tcslen(static_cast<LPCTSTR>(msg_buf)) +
                                   _tcslen(static_cast<LPCTSTR>(function_name)) + 40) *
                                      sizeof(TCHAR)));

    StringCchPrintf(static_cast<LPTSTR>(display_buf), //-V111 -V576
                    LocalSize(display_buf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"),
                    function_name, dw, msg_buf);

    LOG_ERROR(static_cast<LPTSTR>(display_buf));

    LocalFree(msg_buf);
    LocalFree(display_buf);
}

#endif // RK_OS_WINDOWS
