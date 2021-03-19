#include <gtest/gtest.h>

#include "core/platform/unicode.h"
#include "tests/common.h"

using namespace rk;

TEST(Unicode, ustrlen)
{
    EXPECT_EQ(unicode::ustrlen(UC("hello")), 5);
    EXPECT_EQ(unicode::ustrlen(UC("")), 0);
    EXPECT_EQ(unicode::ustrlen(UC("hello\n")), 6);
    EXPECT_EQ(unicode::ustrlen(UC("hello world\n")), 12);
}
