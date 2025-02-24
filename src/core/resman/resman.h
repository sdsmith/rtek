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

    template <typename T>
    class ResourceRef {
        /*
          {
              Resource_Manager resman;
              ResourceRef r = resman->load_texture(ENCODE_STR("texture/reference/path"));
              use(*r);
          }
        */
        Resource() = default;
        ~Resource() = default;
        T& operator*() { return resman->get<T>(h); }

    private:
        Handle<T> h;
        Resource_Manager* resman = nullptr;
    };
};
} // namespace rk
