#include "src/core/platform.h"

#include "src/core/assert.h"
#include <type_traits>

using namespace Rtek;

#if defined(_WIN32) || defined(_WIN64)

void Platform::log_windows_error(LPCTSTR function_name)
{
    // Retrieve error message from system
    //
    LPVOID msg_buf;
    LPVOID display_buf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPTSTR>(&msg_buf), 0, nullptr);

    // Print error message
    //
    display_buf = static_cast<LPVOID>(LocalAlloc(LMEM_ZEROINIT,
        (std::strlen(static_cast<LPCTSTR>(msg_buf))
            + std::strlen(static_cast<LPCTSTR>(function_name)) + 40) * sizeof(TCHAR)));

    StringCchPrintf(static_cast<LPTSTR>(display_buf), //-V111 -V576
        LocalSize(display_buf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        function_name, dw, msg_buf);

    static_assert(std::is_same<TCHAR, char>::value,
        "Unicode Windows strings are not compatible with the logger");
    LOG_ERROR(static_cast<char*>(display_buf));

    LocalFree(msg_buf);
    LocalFree(display_buf);
}

Status Platform::create_directory(char const* directory)
{
    // TODO: support creating all parent directories
    RTK_ASSERT(directory);
    RTK_ASSERT(std::strlen(directory) < 248); // 248 character limit on ANSI ver

    if (!CreateDirectoryA(directory, nullptr)) {
        log_windows_error(TEXT("CreateDirectoryA"));
        return Status::GENERIC_ERROR;
    }

    return Status::OK;
}

bool Platform::directory_exists(char const* path)
{
    DWORD attribs = GetFileAttributes(static_cast<LPCTSTR>(path));
    return (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY));
}

#endif // Windows
