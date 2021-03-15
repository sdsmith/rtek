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

/* TODO(sdsmith): Unicode support. A lot of the functions that windows takes use wide characters.
   This sucks. Things to do:
   - only really need support when things aare going to be used the OS, take input from the user,
and output strings to the user. The downside is this is most of the time with Windows.
   - Make an alias to wchar_t on windows (or maybe to their alias TCHAR?) and defined UNICODE. Use
their unicode stuff.
*/

// TODO(sdsmith): there is some file system stuff in platform.h in a filesystem namespace.

namespace rk::fs
{
class Path {
public:
    Path() = default;
    explicit Path(uchar const* path) noexcept;

    [[nodiscard]] constexpr uchar* data() noexcept { return m_path.data(); }

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

bool directory_exists(uchar const* path) noexcept;

/**
 * \brief Adds a backslash to the end of a string to create the correct syntax for a path. If the
 * source path already has a trailing backslash, no backslash will be added.
 *
 * \return A pointer to the modified path. If the backslash could not be appended due to inadequate
 * buffer size, this value is nullptr.
 */
Status path_add_trailing_slash(Path& path) noexcept;

/**
 * \brief Adds a file name extension to a path string.
 *
 * \param extension Extensionto append, with or without the period.
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
 * \brief Converts a path string into a canonical form.
 *
 * \param path_in Path to canonicalize.
 * \param path_out Resulting path.
 */
Status path_canonicalize(uchar const* path_in, Path& path_out) noexcept;

} // namespace rk::fs
