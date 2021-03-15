#include "core/platform/filesystem.h"

#include "core/platform/platform.h"
#include "core/utility/fixme.h"
#include "core/utility/stb_image.h"
#include <pathcch.h>

//#    include <shlwapi.h>

using namespace rk;

Status fs::load_image(uchar const* path, Image& img) noexcept
{
    RK_ASSERT(path);

    std::wstring const s = fmt::format(UC("{}/{}"), UC(RK_DATA_BASE_DIR), path);

    RK_ASSERT(s.size() < MAX_PATH);
    char utf8_path[MAX_PATH];
    if (stbi_convert_wchar_to_utf8(utf8_path, MAX_PATH, s.data()) == 0) {
        platform::windows::log_last_error(UC("WideCharToMultiByte"));
        return Status::unicode_error;
    }

    img.data = stbi_load(utf8_path, &img.width, &img.height, &img.channels_in_file, 0);
    if (!img.data) {
        LOG_ERROR(stbi_failure_reason());
        return Status::io_error;
    }
    img.actual_channels = img.channels_in_file;

    return Status::ok;
}

Status fs::create_directory(uchar const* directory) noexcept
{
    // TODO: support creating all parent directories
    RK_ASSERT(directory);
    // TODO(sdsmith): check validity of this: RK_ASSERT(platform::ustrlen(directory) < 248); // 248
    // character limit on ANSI ver

    if (!CreateDirectory(directory, nullptr)) {
        platform::windows::log_last_error(UC("CreateDirectoryA"));
        return Status::generic_error;
    }

    return Status::ok;
}

bool fs::directory_exists(uchar const* path) noexcept
{
    DWORD attribs = GetFileAttributes(static_cast<LPCTSTR>(path));
    return (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY));
}

Status fs::path_add_trailing_slash(Path& path) noexcept
{
    uchar* p = nullptr;
    size_t unused_chars = 0;
    HRESULT hr = PathCchAddBackslashEx(path.data(), path.capacity(), &p, &unused_chars);
    switch (hr) {
        case S_FALSE: return Status::ok; // already a trailing slash
        default: platform::windows::log_error(hr); return Status::generic_error;
        case S_OK: break;
    }

    RK_ASSERT(p >= path.data());
    path.update_size(fixme::scast<s32>(p - path.data(), "safe cast"));
    return Status::ok;
}

Status fs::path_add_extension(Path& path, uchar const* extension) noexcept
{
    RK_ASSERT(extension);

    HRESULT hr = PathCchAddExtension(path.data(), path.capacity(), extension);
    switch (hr) {
        case S_FALSE: return Status::ok; // already a trailing slash
        default: platform::windows::log_error(hr); return Status::generic_error;
        case S_OK: break;
    }

    path.size_dirty();
    return Status::ok;
}

Status fs::path_append(Path& path, uchar const* more) noexcept
{
    RK_ASSERT(more);

    HRESULT hr = PathCchAppend(path.data(), path.capacity(), more);
    switch (hr) {
        case S_FALSE: return Status::ok; // already a trailing slash
        default: platform::windows::log_error(hr); return Status::generic_error;
        case S_OK: break;
    }

    path.size_dirty();
    return Status::ok;
}

Status fs::path_canonicalize(uchar const* path_in, Path& path_out) noexcept
{
    RK_ASSERT(path_in);
    RK_ASSERT(path_in != path_out.data());

    HRESULT hr = PathCchCanonicalize(path_out.data(), path_out.capacity(), path_in);
    switch (hr) {
        case S_FALSE: return Status::ok; // already a trailing slash
        default: platform::windows::log_error(hr); return Status::generic_error;
        case S_OK: break;
    }

    path_out.size_dirty();
    return Status::ok;
}
