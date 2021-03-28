#pragma once

#include "core/assert.h"
#include "core/platform/unicode.h"
#include "core/status.h"
#include "core/types.h"
#include <array>
#include <optional>
#include <string_view>

// TODO(sdsmith): how can this be split nicely into the platform impls?
#if RK_OS == RK_OS_WINDOWS
#    include "core/platform/win32_include.h"
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
    explicit Path(char const* path) noexcept;

    [[nodiscard]] constexpr char* data() noexcept { return m_path.data(); }
    [[nodiscard]] constexpr char const* data() const noexcept { return m_path.data(); }

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

    /**
     * \brief Maximum possible size of the path including the null-terminator.
     */
    [[nodiscard]] constexpr s32 capacity() const noexcept
    {
        return static_cast<s32>(m_path.size());
    }

    /**
     * \brief Current size of the path, not including the null-terminator.
     */
    [[nodiscard]] s32 size() noexcept;
    [[nodiscard]] bool empty() noexcept;
    [[nodiscard]] s32 available_capacity() noexcept;

    constexpr char operator[](s32 pos) const noexcept { return m_path[pos]; }
    constexpr char& operator[](s32 pos) noexcept { return m_path[pos]; }

    /**
     * \brief Return the last character in the path.
     */
    [[nodiscard]] char& back() noexcept;

    /**
     * \brief Return pointer to the null-terminator of the path.
     */
    [[nodiscard]] char* end_ptr() noexcept;

    /**
     * \brief Check if path has a trailing path separator.
     */
    [[nodiscard]] bool has_trailing_separator() noexcept;

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
    std::array<char, MAX_PATH> m_path;
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
Status load_image(char const* path, Image& img) noexcept;

/**
 * \brief Get current working directory.
 */
Status get_current_directory(Path& path) noexcept;

/**
 * \brief Create directory.
 */
Status create_directory(char const* directory) noexcept;

/**
 * \brief Check if the given path is a directory.
 *
 * \param[out] exists True if directory exists.
 */
Status directory_exists(char const* path, bool& exists) noexcept;

/*
TODO(sdsmith):
Status write_sync(); // WriteFile
Status write_async();     // WriteFileEx
Status file_delete();     // DeleteFileW
Status file_flush();      // FlushFileBuffers
Status disk_free_space(); // GetDiskFreeSpaceExW
Status file_size();       // GetFileSizeEx
Status file_time();       // GetFileTime
Status file_time_cmp();   // CompareFileTime
Status file_type();       // GetFileType

GetLongPathNameW
GetShortPathNameW
GetTempFileNameW
GetTempPathW
GetFullPathNameW
LocalFileTimeToFileTime
LockFileEx
ReadFileEx
RemoveDirectoryW
SetEndOfFile
SetFilePointerEx
SetFileAttributesW
SetFileIoOverlappedRange
UnlockFileEx
 */

/**
 * \brief Check if the given path is a file.
 *
 * \param[out] exists True if directory exists.
 */
Status file_exists(char const* path, bool& exists) noexcept;

/**
 * \brief True if the given chracter is a path separator.
 */
constexpr bool is_path_separator(char c) noexcept
{
    return c == '/'
#if RK_OS == RK_OS_WINDOWS
           // NOTE(sdsmith): Windows excepts either type of separator, although some Windows API
           // functions only accept the original Windows separator, '\'
           || c == '\\';
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
 * \brief Adds a path separator to the end of the given string. If the source path already has a
 * trailing separator, no separator will be added.
 */
Status path_add_trailing_separator(Path& path) noexcept;

/**
 * \brief Adds a backslash to the end of the given string. If the source path already has a
 * trailing backslash, no backslash will be added.
 */
Status path_remove_trailing_separator(Path& path) noexcept;

/**
 * \brief True if path ends in a path separator.
 */
bool path_has_trailing_separator(char const* path) noexcept;

/**
 * \brief Adds a file name extension to a path string.
 *
 * \param extension Extension to append, with or without the period.
 * \return Ok on success, invalid value if the path already has an extension, error otherwise.
 */
Status path_add_extension(Path& path, char const* extension) noexcept;

/**
 * \brief Get the period corresponding to the file extension in the given path.
 *
 * Hidden files - files prefixed with a single period - are considered to not have an extension
 * if they only start with a period and have no other extension tacked on. This deviates from
 * Windows platform behaviour where they would be considered extensions.
 *
 * \return Nullptr if not found.
 */
char const* path_find_extension(char const* path) noexcept;

/**
 * \brief Get the first character corresponding to the filename in the given path.
 *
 * \return Nullptr if not found.
 */
char const* path_find_file_name(char const* path) noexcept;
char const* path_find_file_name(Path& path) noexcept;

/**
 * \brief Appends one path to the end of another.
 *
 * \param path Path to append to.
 * \param more Path to append.
 */
Status path_append(Path& path, char const* more) noexcept;

/**
 * \brief Remove duplicate path separators.
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
 * NOTE(sdsmith): Does not remove duplicate path separators.
 *
 * \param path_in Path to canonicalize.
 * \param path_out Resulting path.
 *
 * \see path_normalize
 * \see path_remove_dup_separators
 */
Status path_canonicalize(char const* path_in, Path& path_out) noexcept;

/**
 * \brief Transforms the path to its simplest form and normalizes the path separators.
 *
 * Trailing slashes are removed.
 */
Status path_clean(Path& path) noexcept;

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
Status path_combine(char const* path_in, char const* more, Path& path_out) noexcept;

/**
 * \brief Get the common prefix of two paths.
 *
 * \param[out] common_prefix Common prefix shared between the paths.
 */
Status path_common_prefix(char const* path1, char const* path2, Path& common_prefix) noexcept;

/**
 * \brief Get the length common prefix of two paths.
 *
 * \param[out] common_prefix_length Length of the common prefix shared between the paths.
 */
Status path_common_prefix_length(char const* path1, char const* path2,
                                 s32& common_prefix_length) noexcept;

/**
 * \brief Check if a path is a descendant of another path.
 *
 * Paths are simplified to their canonical path for comparison.
 *
 * NOTE(sdsmith):
 * - Does not check the existance of the paths in the file system!
 * - Assumes relative paths stem from the same working directory.
 * - Assumes that a path ending without a slash is a directory.
 *
 * \param child Check if this path descends from \a parent.
 * \param parent Parent path.
 */
Status path_is_descendant(char const* child, char const* parent, bool& is_descendant) noexcept;

/**
 * \brief Compacts the given path into the given maximum length.
 *
 * The path is not guaranteed to be a usable filesystem path.
 *
 * \param max_len Maximum number of characters the path can be, including the null-terminator.
 * May take less than this. \return True if the path was compacted in the given space.
 */
Status path_compact(char const* path_in, Path& path_out, s32 max_len) noexcept;

struct Path_Component {
    /** Path component */
    std::string_view name;
    /** Points to start of component in path */
    char const* path_loc = nullptr;

    Path_Component(char const* start_of_component, s32 len) noexcept;

    /**
     * \brief Return the next component in the path, if it exists.
     */
    [[nodiscard]] std::optional<Path_Component> next() const noexcept;
};

/**
 * \brief Get a path component from the given path.
 *
 * \see Path_Component
 */
std::optional<Path_Component> path_get_component(char const* path) noexcept;

/**
 * \brief Return the next component in path.
 *
 * Ex:
 *   Windows: `c:\path1\path2\file.txt` is 4 components `c:`, `path1`, `path2`, and `file.txt`.
 *   Linux  : `/path1/path2/file.txt` is 3 components `path1`, `path2`, and `file.txt`.
 *
 * NOTE(sdsmith): Works with any path separator supported on the platform.
 *
 * \return Pointer to the start of the next component. Component ends when either a path
 * separator or null terminator is encountered. Return nullptr on error.
 *
 * \see is_path_separator
 */
char const* path_find_next_component(char const* path) noexcept;

} // namespace rk::fs
