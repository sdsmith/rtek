#pragma once

#include "core/assert.h"
#include "core/status.h"
#include "core/types.h"
#include <array>

// TODO(sdsmith): how can this be split nicely into the platform impls?
#if RK_OS == RK_OS_WINDOWS
#    include <windef.h> // MAX_PATH
#else
#    error Unsupported OS
#endif

// TODO(sdsmith): there is some file system stuff in platform.h in a filesystem namespace.

namespace rk::fs
{
class Path {
public:
    Path() noexcept = default;
    explicit Path(uchar const* path) noexcept;

    [[nodiscard]] constexpr uchar* data() noexcept { return m_path.data(); }
    [[nodiscard]] constexpr uchar const* data() const noexcept { return m_path.data(); }

    /**
     * \brief Noralize path separators.
     *
     * \see fs::path_normalize Normalization details.
     */
    Status normalize() noexcept;

    /**
     * \brief Update the size value.
     *
     * NOTE(sdsmith): Honor system. Be kind!
     */
    constexpr void update_size(s32 size) { m_size = size; }

    /**
     * \brief Mark the size as needing to be updated.
     */
    constexpr void size_dirty() { m_size = dirty_flag; }
    [[nodiscard]] constexpr s32 capacity() const noexcept
    {
        return static_cast<s32>(m_path.size());
    }
    [[nodiscard]] s32 size() noexcept;
    [[nodiscard]] bool empty() noexcept;

    constexpr uchar operator[](s32 pos) const noexcept { return m_path[pos]; }
    constexpr uchar& operator[](s32 pos) noexcept { return m_path[pos]; }

    /**
     * \brief Return the last character in the path.
     */
    [[nodiscard]] uchar& back();

    /**
     * \brief Check if path is a windows extended path.
     *
     * ref: https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
     */
    // TODO(sdsmith): assert that all UNC paths are windows extended paths
    [[nodiscard]] bool is_windows_extended_path() const noexcept;

    /**
     * \brief Check if path is a windows universal naming convention (UNC) path.
     *
     * ref: https://docs.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation
     */
    [[nodiscard]] bool is_windows_unc_path() const noexcept;

private:
    std::array<uchar, MAX_PATH> m_path;
    s32 m_size = 0;

    static constexpr s32 dirty_flag = -1;
    constexpr bool is_size_dirty() noexcept { return m_size == dirty_flag; }
};

struct Image {
    s32 width = 0;
    s32 height = 0;
    s32 channels_in_file = 0;
    s32 actual_channels = 0;
    u8* data = nullptr;

    ~Image() noexcept;
    void free_data() noexcept;
};

/**
 * \brief Load an image from the given path.
 *
 * \param img Image data if load succeeded. The image data must be freed by the caller.
 *
 * \see Image::free_data
 */
Status load_image(uchar const* path, Image& img) noexcept;

/**
 * \brief Create directory.
 */
Status create_directory(uchar const* directory) noexcept;

/**
 * \brief Check if the given path is a directory.
 */
bool directory_exists(uchar const* path) noexcept;

/**
 * \brief True if the given chracter is a path separator.
 */
constexpr bool is_path_separator(uchar c) noexcept
{
    return c == UC('/')
#if RK_OS == RK_OS_WINDOWS
           // NOTE(sdsmith): Windows excepts either type of separator, although some Windows API
           // functions only accept the original Windows separator, '\'
           || c == UC('\\');
#endif
}

/**
 * \brief Normalize path separators.
 *
 * Windows uses both '\' and '/' as path separators. However, many Windows API functions only
 * accept the orignal '\' as a valid path separator. This function will convert path separator
 * to the original Windows format.
 *
 * On Linux this function is a no-op.
 */
Status path_normalize(Path& path) noexcept;

/**
 * \brief Adds a backslash to the end of the given string. If the source path already has a
 * trailing backslash, no backslash will be added.
 */
Status path_add_trailing_slash(Path& path) noexcept;

/**
 * \brief Adds a backslash to the end of the given string. If the source path already has a
 * trailing backslash, no backslash will be added.
 */
Status path_remove_trailing_slash(Path& path) noexcept;

/**
 * \brief Adds a file name extension to a path string.
 *
 * \param extension Extension to append, with or without the period.
 * \return Ok on success, invalid value if the path already has an extension, error otherwise.
 */
Status path_add_extension(Path& path, uchar const* extension) noexcept;

/**
 * \brief Appends one path to the end of another.
 *
 * \param path Path to append to.
 * \param more Path to append.
 */
Status path_append(Path& path, uchar const* more) noexcept;

/**
 * DOC(sdsmith):
 */
Status path_remove_dup_separators(Path& path) noexcept;

/**
 * \brief Converts a path string into a canonical form, simplifying the path.
 *
 * Example:
 *   C:\name_1\.\name_2\..\name_3             -> C:\name_1\name_3
 *   C:\name_1\..\name_2\.\name_3             -> C:\name_2\name_3
 *   C:\name_1\name_2\.\name_3\..\name_4      -> C:\name_1\name_2\name_4
 *   C:\name_1\.\name_2\.\name_3\..\name_4\.. -> C:\name_1\name_2
 *   C:\name_1\*...                           -> C:\name_1\*.
 *   C:\..                                    -> C:\
 *
 * NOTE(sdsmith): Does not normalize slashes. Should be done before calling this function to do
 * path comparisons. Windows uses both '/' and '\' as path separators, but this function
 * only accepts '\'.
 *
 * \param path_in Path to canonicalize.
 * \param path_out Resulting path.
 */
Status path_canonicalize(uchar const* path_in, Path& path_out) noexcept;

// TODO(sdsmith):
// /**
//  * \brief Transforms the path to its simplest form and normalizes the path separators.
//  *
//  * Trailing slashes are removed.
//  */
// Status path_clean(Path& path) noexcept;

/**
 * \brief Combines two path fragments into a single path. This function also canonicalizes any
 * relative path elements, removing "." and ".." elements to simplify the final path.
 *
 * \param path_in First part of path. Can be nullptr.
 * \param more Second path of path. If it starts with a single slash, then it's combined with
 * only the root of \a path_in. If it is fully qualified, it is copied directly without being
 * combined. Can be nullptr. \param path_out Resulting path. Can be the same buffer as \a
 * path_in or \a more.
 */
Status path_combine(uchar const* path_in, uchar const* more, Path& path_out) noexcept;

/**
 * \brief Get the common prefix of two paths.
 *
 * \param[out] common_prefix Common prefix shared between the paths.
 */
Status path_common_prefix(uchar const* path1, uchar const* path2, Path& common_prefix) noexcept;

/**
 * \brief Get the length common prefix of two paths.
 *
 * \param[out] common_prefix_length Length of the common prefix shared between the paths.
 */
Status path_common_prefix_length(uchar const* path1, uchar const* path2,
                                 s32& common_prefix_length) noexcept;

// TODO(sdsmith):
// /**
//  * \brief Check if a path is a descendant of another path.
//  *
//  * Paths are simplified to their canonical path for comparison.
//  *
//  * NOTE(sdsmith):
//  * - Does not check the existance of the paths in the file system!
//  * - Assumes relative paths stem from the same working directory.
//  * - Assumes that a path ending without a slash is a directory.
//  *
//  * \param child Check if this path decends from \a parent.
//  * \param parent Parent path.
//  */
// Status path_is_descendant(uchar const* child, uchar const* parent, bool& is_descendant) noexcept;

} // namespace rk::fs
