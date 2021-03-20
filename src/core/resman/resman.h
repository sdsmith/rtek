#pragma once

#include "core/status.h"
#include "core/types.h"
#include <string>

namespace rk
{
class Resource_Manager {
public:
    using rid = s32; /** Resource ID */

    /* TODO(sdsmith):
       - file paths should be considered case insentitive to support Windows
     */

    Resource_Manager() = default;

    [[nodiscard]] Status load_texture(std::string const& path, rid& id) noexcept {}
    // load_icon;
};
} // namespace rk
