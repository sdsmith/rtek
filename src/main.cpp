#include "core/core.h"
#include "core/utility/status.h"
#include "core/utility/version.h"

int main(int argc, char* argv[])
{
    RK_CHECK(rk::Rtek_Engine::initialize());
    RK_CHECK(rk::Rtek_Engine::run());
    RK_CHECK(rk::Rtek_Engine::destroy());

    return 0;
}
