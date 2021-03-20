#include "core/platform/filesystem.h"

#include "core/platform/platform.h"
#include "core/utility/fixme.h"
#include "core/utility/stb_image.h"
#include <pathcch.h>
#include <shlwapi.h>

using namespace rk;

Status fs::load_image(uchar const* path, Image& img) noexcept
{
    RK_ASSERT(path);

    std::wstring const s = fmt::format(UC("{}/{}"), UC(RK_DATA_BASE_DIR), path);

    RK_ASSERT(s.size() < MAX_PATH);
    std::array<char, MAX_PATH> utf8_path;
    if (stbi_convert_wchar_to_utf8(utf8_path.data(), MAX_PATH, s.data()) == 0) {
        platform::windows::log_last_error(UC("WideCharToMultiByte"));
        return Status::unicode_error;
    }

    img.data = stbi_load(utf8_path.data(), &img.width, &img.height, &img.channels_in_file, 0);
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
    RK_ASSERT(path);
    DWORD attribs = GetFileAttributes(static_cast<LPCTSTR>(path));
    return (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY));
}

bool fs::file_exists(uchar const* path) noexcept
{
    RK_ASSERT(path);
    return PathFileExists(path);
}

Status fs::path_normalize(Path& path) noexcept
{
    if (path.size() > 0) {
        uchar* p = path.data();
        while (*p != UC('\0')) {
            if (*p == UC('/')) { *p = UC('\\'); }
            ++p;
        }
    }
    return Status::ok;
}

Status fs::path_add_trailing_slash(Path& path) noexcept
{
    // NOTE(sdsmith): Windows allows both '/' and '\' as slashes, but PathCchAddBackslashEx only
    // considers '\' a backslash. Check for ending '/' so it doesn't add a windows slash to the end.
    if (path[path.size() - 1] == UC('/')) { return Status::ok; }

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

bool fs::path_has_trailing_slash(uchar const* path) noexcept
{
    RK_ASSERT(path);
    if (*path == RK_NULL_TERM) { return false; }

    uchar const* p = path;
    while (*p != RK_NULL_TERM) { ++p; }
    --p;
    return is_path_separator(*p);
}

Status fs::path_add_extension(Path& path, uchar const* extension) noexcept
{
    RK_ASSERT(extension);

    HRESULT hr = PathCchAddExtension(path.data(), path.capacity(), extension);
    switch (hr) {
        case S_FALSE: return Status::invalid_value; // already has an extension
        default: platform::windows::log_error(hr); return Status::generic_error;
        case S_OK: break;
    }

    path.size_dirty();
    return Status::ok;
}

uchar const* fs::path_find_extension(uchar const* path) noexcept
{
    RK_ASSERT(path)
    uchar const* p = PathFindExtension(path);

    if (p) {
        // Adjust "unfound" return from PathFindExtension
        if (*p == UC('\0')) { return nullptr; }

        // NOTE(sdsmith): Windows considers hidden file names on Linux (filename prefixed with a
        // '.') as extensions when there is no extension additional extension. For instance,
        // ".emacs" on Windows has extension "emacs" and ".emacs.d" has extension "d". To keep
        // behaviour between Windows and Linux consistent, consider hidden files with no extension
        // as having no extension on all platforms.
        if (p == path                      // start of the path
            || is_path_separator(*(p - 1)) // start of the file name
        ) {
            // Is hidden file with no path extension
            return nullptr;
        }

        // Treat files that end in a period as not having an extension
        if (*(p + 1) == UC('\0')) { return nullptr; }

        return p;
    } else {
        return nullptr;
    }
}

uchar const* fs::path_find_file_name(uchar const* path) noexcept
{
    RK_ASSERT(path);

    if (*path == RK_NULL_TERM ||
        // If last character is a path separator, there's no filename
        path_has_trailing_slash(path)) {
        return nullptr;
    }

    uchar const* p = PathFindFileName(path);

    if (p) {
        // Path separators aren't a file name
        while (*p != RK_NULL_TERM && is_path_separator(*p)) { ++p; }

        // If there is only path separators, there is no file name in the path
        if (*p == RK_NULL_TERM) { return nullptr; }
    }

    return p;
}

uchar const* fs::path_find_file_name(Path& path) noexcept
{
    if (path.empty() || path.has_trailing_separator()) { return nullptr; }

    uchar const* p = PathFindFileName(path.data());
    if (p) {
        // Path separators aren't a file name
        while (*p != RK_NULL_TERM && is_path_separator(*p)) { ++p; }

        // If there is only path separators, there is no file name in the path
        if (*p == RK_NULL_TERM) { return nullptr; }
    }

    return p;
}

Status fs::path_append(Path& path, uchar const* more) noexcept
{
    RK_ASSERT(more);

    // NOTE(sdsmith): Windows allows both '/' and '\' as slashes, but PathCchAddBackslashEx only
    // considers '\' a backslash. Check for ending '/' and convert it to windows format so it
    // doesn't add an extra slash.
    uchar& c = path[path.size() - 1];
    if (c == UC('/')) { c = UC('\\'); }

    HRESULT hr = PathCchAppend(path.data(), path.capacity(), more);
    switch (hr) {
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

    /* NOTE(sdsmith): PathCchCanonicalize returns a single slash if either a) the path contains
     * enough ".." to go past the root, or b) is given the empty string. I find this annoying,
     * because an empty string should undergo no transformations.
     */
    if (*path_in == UC('\0')) {
        path_out = fs::Path(UC(""));
        return Status::ok;
    }

    HRESULT hr = PathCchCanonicalize(path_out.data(), path_out.capacity(), path_in);
    switch (hr) {
        default: platform::windows::log_error(hr); return Status::generic_error;
        case S_OK: break;
    }
    path_out.size_dirty();
    return Status::ok;
}

Status fs::path_combine(uchar const* path_in, uchar const* more, Path& path_out) noexcept
{
    RK_ASSERT(path_in || more); // one of them needs to be present

    HRESULT hr = PathCchCombine(path_out.data(), path_out.capacity(), path_in, more);
    if (hr != S_OK) {
        platform::windows::log_error(hr);
        return Status::generic_error;
    }

    path_out.size_dirty();
    return Status::ok;
}

Status fs::path_common_prefix(uchar const* path1, uchar const* path2, Path& common_prefix) noexcept
{
    RK_ASSERT(path1);
    RK_ASSERT(path2);

    // s32 const len = PathCommonPrefix(path1, path2, common_prefix.data());
    // common_prefix.update_size(len + 1);

    uchar const* p1 = path1;
    uchar const* p2 = path2;
    uchar* prefix = common_prefix.data();
    RK_ASSERT(prefix);

    while (*p1 != UC('\0') && *p2 != UC('\0') && *p1 == *p2) {
        *prefix = *p1;
        ++prefix;
        ++p1;
        ++p2;
    }
    *prefix = UC('\0');

    // TODO(sdsmith): @optimize: calc size
    common_prefix.size_dirty();
    return Status::ok;
}

Status fs::path_common_prefix_length(uchar const* path1, uchar const* path2,
                                     s32& common_prefix_length) noexcept
{
    RK_ASSERT(path1);
    RK_ASSERT(path2);

    // NOTE(sdsmith): PathCommonPrefix returns len 3 when given "\\a\\b\\c" and "\\a". It should
    // be 2! common_prefix_length = PathCommonPrefix(path1, path2, nullptr);
    uchar const* p1 = path1;
    uchar const* p2 = path2;
    common_prefix_length = 0;
    while (*p1 != UC('\0') && *p2 != UC('\0') && *p1 == *p2) {
        ++common_prefix_length;
        ++p1;
        ++p2;
    }

    return Status::ok;
}

Status fs::path_is_descendant(uchar const* child, uchar const* parent, bool& is_descendant) noexcept
{
    RK_ASSERT(child);
    RK_ASSERT(parent);

    if (*child == UC('\0') || *parent == UC('\0')) {
        is_descendant = false;
        return Status::ok;
    }

    Path clean_child(child);
    RK_CHECK(path_clean(clean_child));
    Path clean_parent(parent);
    RK_CHECK(path_clean(clean_parent));

    if (clean_child.empty() || clean_parent.empty()) {
        is_descendant = false;
        return Status::ok;
    }

    s32 len = 0;
    RK_CHECK(path_common_prefix_length(clean_parent.data(), clean_child.data(), len));
    if (is_path_separator(clean_parent.back()) && len > 1) {
        // path_common_prefix does not include trailing slashes in the common prefix
        len++;
    }

    bool const parent_is_common_prefix = len == clean_parent.size();
    bool const child_is_common_prefix = len == clean_child.size();

    if (!parent_is_common_prefix ||                         // parent must be contained in child
        (parent_is_common_prefix && child_is_common_prefix) // path can't descend from itself
    ) {
        is_descendant = false;
        return Status::ok;
    }

    is_descendant = true;
    return Status::ok;
}

bool fs::path_compact(uchar const* path_in, Path& path_out, s32 max_len) noexcept
{
    RK_ASSERT(path_in);
    RK_ASSERT(max_len > 0);

    return PathCompactPathEx(path_out.data(), path_in, max_len, 0);
}
