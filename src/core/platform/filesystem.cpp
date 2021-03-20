#include "core/platform/filesystem.h"

#include "core/assert.h"
#include "core/logging/logging.h"
#include "core/platform/unicode.h"
#include "core/utility/fixme.h"
#include "core/utility/stb_image.h"
#include <fmt/core.h>

using namespace rk;

fs::Path::Path(uchar const* path) noexcept
{
    RK_ASSERT(path);

    uchar const* p = path;
    s32 i = 0;
    while (*p != '\0') {
        if (i >= (m_path.size() - 1)) {
            LOG_ERROR(UC("Path greater than max path length ({}): \"{}\""), m_path.size(), path);
            RK_ASSERT(0);
        }
        m_path[i] = *p;
        ++i;
        ++p;
    }

    m_path[i] = '\0';
    m_size = i;
}

Status fs::Path::normalize() noexcept { return fs::path_normalize(*this); }

s32 fs::Path::size() noexcept
{
    if (is_size_dirty()) { m_size = unicode::ustrlen(m_path.data()); }
    return m_size;
}

bool fs::Path::empty() noexcept { return size() == 0; }

uchar& fs::Path::back()
{
    RK_ASSERT(size() > 0);
    return m_path[size() - 1];
}

bool fs::Path::has_trailing_separator() noexcept { return !empty() && is_path_separator(back()); }

bool fs::Path::is_windows_extended_path() const noexcept
{
    // NOTE(sdsmith): Windows extended paths are prefixed with "\\?\".
    uchar const* prefix = UC("\\\\?\\");
    return unicode::ustrcmp(m_path.data(), prefix, 4);
}

bool fs::Path::is_windows_unc_path() const noexcept
{
    // NOTE(sdsmith): Windows UNC paths are prefixed with "\\?\UNC\".
    uchar const* prefix = UC("\\\\?\\UNC\\");
    return unicode::ustrcmp(m_path.data(), prefix, 8);
}

fs::Image::~Image() noexcept { free_data(); }

void fs::Image::free_data() noexcept
{
    if (data) {
        stbi_image_free(data);
        data = nullptr;
    }
}

Status fs::path_remove_trailing_slash(Path& path) noexcept
{
    if (!path.empty() &&
#if RK_OS == RK_OS_LINUX
        // Don't remove root
        path.size() > 1 &&
#endif
        is_path_separator(path.back())) {
        path.back() = UC('\0');
        path.size_dirty();
    }
    return Status::ok;
}

Status fs::path_clean(Path& path) noexcept
{
    RK_CHECK(path_normalize(path));
    RK_CHECK(path_remove_dup_separators(path));
    RK_CHECK(path_remove_trailing_slash(path));

    fs::Path clean_path;
    RK_CHECK(path_canonicalize(path.data(), clean_path));

    path = clean_path;
    return Status::ok;
}

Status fs::path_remove_dup_separators(Path& path) noexcept
{
    if (path.empty()) { return Status::ok; }

    uchar* w = path.data(); // write pointer
#if RK_OS == RK_OS_WINDOWS
    // In windows it is valid to have two starting path separators (extended paths, UNC, ...)
    if (unicode::ustrcmp(w, UC("\\\\"), 2)) {
        // Only advance one since the first separator always gets copied. 1 skipped + 1 copied = 2.
        w += 1;
    }
#elif RK_OS == RK_OS_LINUX
    /* NOTE(sdsmith): POSIX standard:
     * > A pathname consisting of a single slash shall resolve to the root
     *   directory of the process. A null pathname shall not be successfully
     *   resolved. A pathname that begins with two successive slashes may be
     *   interpreted in an implementation-defined manner, although more than two
     *   leading slashes shall be treated as a single slash.
     * ref: https://pubs.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap04.html#tag_04_11
     */
    if (unicode::ustrcmp(w, UC("//"), 2)) {
        // Only advance one since the first separator always gets copied. 1 skipped + 1 copied = 2.
        w += 1;
    }
#endif

    uchar* r = w; // read pointer

    while (*r != UC('\0')) {
        *w = *r;
        ++w;
        if (is_path_separator(*r)) {
            do {
                ++r;
            } while (is_path_separator(*r));
        } else {
            ++r;
        }
    }
    *w = UC('\0');

    // TODO(sdsmith): @optimize: update size properly
    path.size_dirty();
    return Status::ok;
}
