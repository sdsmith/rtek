#pragma once

#include "core/assert.h"

#include <cstring>
#include <type_traits>

namespace rk
{
/**
 * \brief Perform type punning without UB.
 *
 * Usage: `float a = 0; int b = type_pun<int>(a);`
 * ref: https://www.youtube.com/watch?v=sCjZuvtJd-k
 */
template <typename ToT, typename FromT>
ToT type_pun(FromT a)
{
    RK_STATIC_ASSERT(std::is_trivially_constructible_v<FromT>);
    RK_STATIC_ASSERT(std::is_trivially_constructible_v<ToT>);
    RK_STATIC_ASSERT_MSG(sizeof(FromT) >= sizeof(ToT),
                         "destination type too small to hold source type");

    ToT ret{};
    std::memcpy(&ret, &a, sizeof(ret));
    return ret;
}

} // namespace rk
