#include "core/platform/filesystem.h"

#include "core/platform/platform.h"
#include "core/platform/unicode.h"
#include "core/types.h"
#include "core/utility/fixme.h"
#include "core/utility/stb_image.h"
#include <pathcch.h>
#include <shlwapi.h>
#include <array>

using namespace rk;

using wpath_buf = std::array<wchar_t, MAX_PATH>;

Status fs::get_current_directory(Path& path) noexcept
{
    wpath_buf buf;
    DWORD ret = GetCurrentDirectoryW(fixme::scast<s32>(buf.size(), "safe cast"), buf.data());
    if (ret == 0) {
        platform::windows::log_last_error("GetCurrentDirectory");
        return Status::platform_error;
    }

    if (!unicode::narrow(path.data(), path.capacity(), buf.data())) {
        return Status::unicode_error;
    }

    return Status::ok;
}

Status fs::create_directory(char const* directory) noexcept
{
    // TODO(sdsmith): : support creating all parent directories
    RK_ASSERT(directory);

    wpath_buf buf;
    if (!unicode::widen(buf.data(), fixme::scast<s32>(buf.size(), "safe cast"), directory)) {
        return Status::unicode_error;
    }

    if (!CreateDirectoryW(buf.data(), nullptr)) {
        platform::windows::log_last_error("CreateDirectoryA");
        return Status::generic_error;
    }

    return Status::ok;
}

Status fs::directory_exists(char const* path, bool& exists) noexcept
{
    RK_ASSERT(path);
    wpath_buf buf;
    if (!unicode::widen(buf.data(), buf.size(), path)) { return Status::unicode_error; }

    DWORD attribs = GetFileAttributesW(buf.data());
    exists = (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY));
    return Status::ok;
}

Status fs::file_exists(char const* path, bool& exists) noexcept
{
    RK_ASSERT(path);
    wpath_buf buf;
    if (!unicode::widen(buf.data(), buf.size(), path)) { return Status::unicode_error; }
    exists = PathFileExistsW(buf.data());
    return Status::ok;
}

Status fs::path_add_extension(Path& path, char const* extension) noexcept
{
    RK_ASSERT(extension);

    wpath_buf buf_path;
    if (!unicode::widen(buf_path.data(), buf_path.size(), path.data())) {
        return Status::unicode_error;
    }
    wpath_buf buf_ext;
    if (!unicode::widen(buf_ext.data(), buf_ext.size(), extension)) {
        return Status::unicode_error;
    }

    HRESULT hr = PathCchAddExtension(buf_path.data(), buf_path.size(), buf_ext.data());
    switch (hr) {
        case S_FALSE: return Status::invalid_value; // already has an extension
        default: platform::windows::log_error(hr); return Status::generic_error;
        case S_OK: break;
    }

    if (!unicode::narrow(path.data(), path.capacity(), buf_path.data())) {
        return Status::unicode_error;
    }

    path.size_dirty();
    return Status::ok;
}

Status fs::path_append(Path& path, char const* more) noexcept
{
    RK_ASSERT(more);

    // NOTE(sdsmith): Windows allows both '/' and '\' as slashes, but PathCchAddBackslashEx only
    // considers '\' a backslash. Check for ending '/' and convert it to windows format so it
    // doesn't add an extra slash.
    char& c = path[path.size() - 1];
    if (c == '/') { c = '\\'; }

    wpath_buf path_buf;
    if (!unicode::widen(path_buf.data(), path_buf.size(), path.data())) {
        return Status::unicode_error;
    }

    wpath_buf more_buf;
    if (!unicode::widen(more_buf.data(), more_buf.size(), more)) { return Status::unicode_error; }

    HRESULT hr = PathCchAppend(path_buf.data(), path_buf.size(), more_buf.data());
    switch (hr) {
        default: platform::windows::log_error(hr); return Status::generic_error;
        case S_OK: break;
    }

    if (!unicode::narrow(path.data(), path.capacity(), path_buf.data())) {
        return Status::unicode_error;
    }

    path.size_dirty();
    return Status::ok;
}

Status fs::path_canonicalize(char const* path_in, Path& path_out) noexcept
{
    RK_ASSERT(path_in);
    RK_ASSERT(path_in != path_out.data());

    /* NOTE(sdsmith): PathCchCanonicalize returns a single slash if either a) the path contains
     * enough ".." to go past the root, or b) is given the empty string. I find this annoying,
     * because an empty string should undergo no transformations.
     */
    if (*path_in == '\0') {
        path_out = fs::Path("");
        return Status::ok;
    }

    wpath_buf buf;
    if (!unicode::widen(buf.data(), buf.size(), path_in)) { return Status::unicode_error; }

    wpath_buf path_buf;
    HRESULT hr = PathCchCanonicalize(path_buf.data(), path_buf.size(), buf.data());
    switch (hr) {
        default: platform::windows::log_error(hr); return Status::generic_error;
        case S_OK: break;
    }

    if (!unicode::narrow(path_out.data(), path_out.capacity(), path_buf.data())) {
        return Status::unicode_error;
    }

    path_out.size_dirty();
    return Status::ok;
}

Status fs::path_combine(char const* path_in, char const* more, Path& path_out) noexcept
{
    RK_ASSERT(path_in || more); // one of them needs to be present

    wpath_buf path1_buf;
    if (!unicode::widen(path1_buf.data(), path1_buf.size(), path_in)) {
        return Status::unicode_error;
    }
    wpath_buf path2_buf;
    if (!unicode::widen(path1_buf.data(), path1_buf.size(), more)) { return Status::unicode_error; }

    wpath_buf buf;
    HRESULT hr = PathCchCombine(buf.data(), buf.size(), path1_buf.data(), path2_buf.data());
    if (hr != S_OK) {
        platform::windows::log_error(hr);
        return Status::generic_error;
    }

    if (!unicode::narrow(path_out.data(), path_out.capacity(), buf.data())) {
        return Status::unicode_error;
    }

    path_out.size_dirty();
    return Status::ok;
}

Status fs::path_compact(char const* path_in, Path& path_out, s32 max_len) noexcept
{
    RK_ASSERT(path_in);
    RK_ASSERT(max_len > 0);

    wpath_buf in_buf;
    if (!unicode::widen(in_buf.data(), in_buf.size(), path_in)) { return Status::unicode_error; }

    wpath_buf path_buf;
    bool success = PathCompactPathEx(path_buf.data(), in_buf.data(), max_len, 0);

    if (success) {
        if (!unicode::narrow(path_out.data(), path_out.capacity(), path_buf.data())) {
            return Status::unicode_error;
        }

        return Status::ok;
    } else {
        return Status::buffer_length_error;
    }
}
