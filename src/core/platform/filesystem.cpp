#include "core/platform/filesystem.h"

#include "core/assert.h"
#include "core/logging/logging.h"
#include "core/platform/unicode.h"
#include "core/types.h"
#include "core/utility/fixme.h"
#include "core/utility/stb_image.h"
#include <fmt/core.h>
#include <sds/string.h>

using namespace rk;
using namespace sds;

fs::Path::Path(char const* path) noexcept
{
    RK_ASSERT(path);

    char const* p = path;
    s32 i = 0;
    while (*p != '\0') {
        if (i >= (m_path.size() - 1)) {
            LOG_ERROR("Path greater than max path length ({}): \"{}\"", m_path.size(), path);
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
    if (is_size_dirty()) { m_size = sds::str_size(m_path.data()); }
    return m_size;
}

bool fs::Path::empty() noexcept { return size() == 0; }

s32 fs::Path::available_capacity() noexcept { return capacity() - size() - 1; }

char& fs::Path::back() noexcept
{
    RK_ASSERT(size() > 0);
    return m_path[size() - 1];
}

char* fs::Path::end_ptr() noexcept { return data() + size(); }

bool fs::Path::has_trailing_separator() noexcept { return !empty() && is_path_separator(back()); }

bool fs::Path::is_windows_extended_path() const noexcept
{
    // NOTE(sdsmith): Windows extended paths are prefixed with "\\?\".
    char const* prefix = R"(\\?\)";
    return unicode::ascii_cmp(m_path.data(), prefix, 4);
}

bool fs::Path::is_windows_unc_path() const noexcept
{
    // NOTE(sdsmith): Windows UNC paths are prefixed with "\\?\UNC\".
    char const* prefix = R"(\\?\UNC\)";
    return unicode::ascii_cmp(m_path.data(), prefix, 8);
}

fs::Image::~Image() noexcept { free_data(); }

void fs::Image::free_data() noexcept
{
    if (data) {
        stbi_image_free(data);
        data = nullptr;
    }
}

Status fs::load_image(char const* path, Image& img) noexcept
{
    RK_ASSERT(path);

    std::string const s = fmt::format("{}/{}", RK_DATA_BASE_DIR, path);
    img.data = stbi_load(s.data(), &img.width, &img.height, &img.channels_in_file, 0);
    if (!img.data) {
        LOG_ERROR(stbi_failure_reason());
        return Status::io_error;
    }
    img.actual_channels = img.channels_in_file;

    return Status::ok;
}

Status fs::path_normalize(Path& path) noexcept
{
    if (path.size() > 0) {
        char* p = path.data();
        while (*p != '\0') {
            if (*p == '/') { *p = '\\'; }
            ++p;
        }
    }
    return Status::ok;
}

Status fs::path_add_trailing_separator(Path& path) noexcept
{
    if (is_path_separator(path[path.size() - 1])) { return Status::ok; }
    if (path.available_capacity() < 1) { return Status::buffer_length_error; }

    s32 const size = path.size();
    char* p = path.data() + size;
    *p = RK_PATH_SEPARATOR;
    ++p;
    *p = '\0';
    path.update_size(size + 1);

    return Status::ok;
}

Status fs::path_remove_trailing_separator(Path& path) noexcept
{
    if (!path.empty() &&
#if SDS_OS_LINUX
        // Don't remove root
        path.size() > 1 &&
#endif
        is_path_separator(path.back())) {
        path.back() = '\0';
        path.size_dirty();
    }
    return Status::ok;
}

bool fs::path_has_trailing_separator(char const* path) noexcept
{
    RK_ASSERT(path);
    if (*path == '\0') { return false; }

    char const* p = path;
    while (*p != '\0') { ++p; }
    --p;
    return is_path_separator(*p);
}

char const* fs::path_find_extension(char const* path) noexcept
{
    RK_ASSERT(path);

    char const* last_dot = nullptr;
    char const* p = path;
    while (*p != '\0') {
        if (*p == '.') { last_dot = p; }
        ++p;
    }

    if (last_dot) {
        // Adjust "unfound" return from PathFindExtension
        if (*last_dot == '\0') { return nullptr; }

        // NOTE(sdsmith): Windows considers hidden file names on Linux (filename prefixed with a
        // '.') as extensions when there is no extension additional extension. For instance,
        // ".emacs" on Windows has extension "emacs" and ".emacs.d" has extension "d". To keep
        // behaviour between Windows and Linux consistent, consider hidden files with no extension
        // as having no extension on all platforms.
        if (last_dot == path                      // start of the path
            || is_path_separator(*(last_dot - 1)) // start of the file name
        ) {
            // Is hidden file with no path extension
            return nullptr;
        }

        // Treat files that end in a period as not having an extension
        if (*(last_dot + 1) == '\0') { return nullptr; }

        return last_dot;
    } else {
        return nullptr;
    }
}

char const* fs::path_find_file_name(char const* path) noexcept
{
    RK_ASSERT(path);

    if (*path == '\0') { return nullptr; }

    char const* last_sep = nullptr;
    char const* p = path;
    while (*p != '\0') {
        if (is_path_separator(*p)) { last_sep = p; }
        ++p;
    }

    if (last_sep) {
        if (*(last_sep + 1) != '\0') {
            // filename follows the separator
            return last_sep + 1;
        } else {
            // If last character is a path separator, there's no filename
            return nullptr;
        }

    } else {
        return path;
    }
}

char const* fs::path_find_file_name(Path& path) noexcept
{
    return path_find_file_name(path.data());
}

Status fs::path_clean(Path& path) noexcept
{
    RK_CHECK(path_normalize(path));
    RK_CHECK(path_remove_dup_separators(path));
    RK_CHECK(path_remove_trailing_separator(path));

    fs::Path clean_path;
    RK_CHECK(path_canonicalize(path.data(), clean_path));

    path = clean_path;
    return Status::ok;
}

Status fs::path_remove_dup_separators(Path& path) noexcept
{
    if (path.empty()) { return Status::ok; }

    char* w = path.data(); // write pointer
#if SDS_OS_WINDOWS
    // In windows it is valid to have two starting path separators (extended paths, UNC, ...)
    if (unicode::ascii_cmp(w, "\\\\", 2)) {
        // Only advance one since the first separator always gets copied. 1 skipped + 1 copied = 2.
        w += 1;
    }
#elif SDS_OS_LINUX
    /* NOTE(sdsmith): POSIX standard:
     * > A pathname consisting of a single slash shall resolve to the root
     *   directory of the process. A null pathname shall not be successfully
     *   resolved. A pathname that begins with two successive slashes may be
     *   interpreted in an implementation-defined manner, although more than two
     *   leading slashes shall be treated as a single slash.
     * ref: https://pubs.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap04.html#tag_04_11
     */
    if (unicode::ascii_cmp(w, "//", 2)) {
        // Only advance one since the first separator always gets copied. 1 skipped + 1 copied = 2.
        w += 1;
    }
#endif

    char* r = w; // read pointer

    while (*r != '\0') {
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
    *w = '\0';

    // TODO(sdsmith): @optimize: update size properly
    path.size_dirty();
    return Status::ok;
}

Status fs::path_common_prefix(char const* path1, char const* path2, Path& common_prefix) noexcept
{
    RK_ASSERT(path1);
    RK_ASSERT(path2);

    char const* p1 = path1;
    char const* p2 = path2;
    char* prefix = common_prefix.data();
    RK_ASSERT(prefix);

    while (*p1 != '\0' && *p2 != '\0' && *p1 == *p2) {
        *prefix = *p1;
        ++prefix;
        ++p1;
        ++p2;
    }
    *prefix = '\0';

    // TODO(sdsmith): @optimize: calc size
    common_prefix.size_dirty();
    return Status::ok;
}

Status fs::path_common_prefix_length(char const* path1, char const* path2,
                                     s32& common_prefix_length) noexcept
{
    RK_ASSERT(path1);
    RK_ASSERT(path2);

    char const* p1 = path1;
    char const* p2 = path2;
    common_prefix_length = 0;
    while (*p1 != '\0' && *p2 != '\0' && *p1 == *p2) {
        ++common_prefix_length;
        ++p1;
        ++p2;
    }

    return Status::ok;
}

Status fs::path_is_descendant(char const* child, char const* parent, bool& is_descendant) noexcept
{
    RK_ASSERT(child);
    RK_ASSERT(parent);

    if (*child == '\0' || *parent == '\0') {
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

fs::Path_Component::Path_Component(char const* start_of_component, s32 len) noexcept
    : name{start_of_component, static_cast<size_t>(len)}, path_loc(start_of_component)
{
    RK_ASSERT(start_of_component);
}

std::optional<fs::Path_Component> fs::Path_Component::next() const noexcept
{
    return path_get_component(path_find_next_component(path_loc));
}

std::optional<fs::Path_Component> fs::path_get_component(char const* path) noexcept
{
    if (path == nullptr || *path == '\0') { return {}; }

    char const* component_start = path;

    // Skip beginning path separators to get to the next path segment
    while (*component_start != '\0' && is_path_separator(*component_start)) { ++component_start; }

    // Only path separators means no component
    if (*component_start == '\0') { return {}; }

    // Count length of component
    char const* p = component_start;
    s32 n = 0;
    while (*p != '\0' && !is_path_separator(*p)) {
        ++n;
        ++p;
    }

    return fs::Path_Component(component_start, n);
}

char const* fs::path_find_next_component(char const* path) noexcept
{
    RK_ASSERT(path);

    if (*path == '\0') { return nullptr; }

    char const* p = path;

    if (!is_path_separator(*p)) {
        // Skip path segment to get to path separators
        while (*p != '\0' && !is_path_separator(*p)) { ++p; }
    }

    // Skip path separators to get to the next path segment
    while (*p != '\0' && is_path_separator(*p)) { ++p; }

    return p;
}
