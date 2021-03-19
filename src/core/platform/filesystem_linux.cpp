#include "core/platform/filesystem.h"

Status fs::path_normalize(Path& path) noexcept
{
    // NOTE(sdsmith): On Linux there is only one path separator, '/'. '\' is a balid character in a
    // path.
    return Status::ok;
}
