#include "core/core.h"
#include "core/utility/status.h"
#include "core/utility/version.h"

int main(int argc, char* argv[])
{
    rk::Rtek_Engine engine;
    RK_CHECK(engine.initialize());
    RK_CHECK(engine.run());
    RK_CHECK(engine.destroy());

    return 0;
}
