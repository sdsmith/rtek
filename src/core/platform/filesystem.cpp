#include "core/platform/filesystem.h"

#include "core/assert.h"
#include "core/logging/logging.h"
#include "core/platform/platform.h"
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

s32 fs::Path::size() noexcept
{
    if (is_size_dirty()) { m_size = platform::ustrlen(m_path.data()); }
    return m_size;
}

fs::Image::~Image() noexcept { free_data(); }

void fs::Image::free_data() noexcept
{
    if (data) {
        stbi_image_free(data);
        data = nullptr;
    }
}
