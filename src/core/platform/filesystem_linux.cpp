#include "core/platform/filesystem.h"

#include "core/platform/platform.h"
#include "core/types.h"
#include "sds/types.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace rk;
using namespace sds;


Status fs::path_normalize(Path& path) noexcept
{
    // NOTE(sdsmith): On Linux there is only one path separator, '/'. '\' is a balid character in a
    // path.
    return Status::ok;
}

Status fs::create_directory(char const* directory) noexcept {
    RK_ASSERT(directory);

    // TODO(sdsmith): explicitly report perm issues

    struct stat st = {0};
    bool exists = false;
    RK_CHECK(fs::directory_exists(directory, exists));
    if (!exists) {
	    // directory doesn't exist, create it
        if (mkdir(directory, 0700) == -1) {
            LOG_OS_LAST_ERROR("mkdir");
            return Status::platform_error;
        }
    }

    return Status::ok;
}

Status fs::directory_exists(char const* path, bool& exists) noexcept {
    RK_ASSERT(path);

    struct stat st = {0};
    const int ret = stat(path, &st);
    exists = (ret != -1);
    if (!exists) {
	switch (ret) {
        case ENOENT:
	case ENOTDIR:
	    // nothing bad happened when checking
	    break;
	default:
        LOG_OS_LAST_ERROR("stat");
	    return Status::platform_error;
	}
    }

    return Status::ok;
}
