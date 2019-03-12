#include "rtek.h"

// Engine logging macros should not be defined beyond engine source files.
#if defined(LOG_INFO) || defined(LOG_DEBUG) || defined(LOG_WARN) || defined(LOG_ERROR)
#   error RTEK logging macros are leaking!
#endif

int main(int argc, char* argv[])
{
    RTK_CHECK(Rtek::Initialize());
    RTK_CHECK(Rtek::Run());
    RTK_CHECK(Rtek::Destroy());

    return 0;
}
