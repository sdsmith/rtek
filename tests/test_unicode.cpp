#include <gtest/gtest.h>

#include "core/platform/platform.h"
#include "tests/common.h"

using namespace rk;

TEST(Unicode, ustrlen)
{
    EXPECT_EQ(platform::ustrlen(UC("hello")), 5);
    EXPECT_EQ(platform::ustrlen(UC("")), 0);
    EXPECT_EQ(platform::ustrlen(UC("hello\n")), 6);
    EXPECT_EQ(platform::ustrlen(UC("hello world\n")), 12);
}
