#include "core/core.h"
#include "core/platform/unicode.h"
#include "core/status.h"
#include "core/utility/no_exception.h"
#include "core/version.h"

int main(int argc, char* argv[])
{
    // TODO(sdsmith): this arg conversion stuff should really be handled in the engine and not
    // exposed here.

    // Convert program arguments to UTF-8
    //
    // NOTE(sdsmith): Now when referring to argc and argv, they will be UTF-8 encoded
    rk::unicode::Args args(argc, argv); // TODO(sdsmith): throws

    rk::Rtek_Engine engine;
    RK_CHECK(engine.initialize());
    RK_CHECK(engine.run());
    RK_CHECK(engine.destroy());

    return 0;
}
