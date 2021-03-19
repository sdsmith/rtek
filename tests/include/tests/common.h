#pragma once

#include "core/status.h"
#include <gtest/gtest.h>

/**
 * \brief Expect ok status. Googletest macro wrapper.
 */
#define RK_EXPECT_OK(x) EXPECT_EQ((x), Status::ok)
