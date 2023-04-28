#pragma once

#define SDS_ASSERT RK_ASSERT
#define SDS_FORWARD_TYPES_TO_NAMESPACE rk
#include "sds/types.h"
#define RK_INTERNAL SDS_INTERNAL
#define RK_FILENAME SDS_FILENAME
#define RK_FUNCNAME SDS_FUNCNAME
#define RK_STATIC_ASSERT SDS_STATIC_ASSERT
#define RK_STATIC_ASSERT_MSG SDS_STATIC_ASSERT_MSG
#define RK_LIKELY SDS_LIKELY
#define RK_UNLIKELY SDS_UNLIKELY
#define RK_RESTRICT SDS_RESTRICT
#define RK_PATH_SEPARATOR SDS_PATH_SEPARATOR
#define RK_PATH_SEPARATOR_STR SDS_PATH_SEPARATOR_STR

namespace rk
{

/**
 * \def RK_DATA_BASE_DIR
 * \brief Base path to the data directory.
 */
#ifndef RK_DATA_BASE_DIR
#    define RK_DATA_BASE_DIR "data"
#endif

/**
 * \def RK_SHADER_BASE_DIR
 * \brief Base path to the shader directory.
 */
#ifndef RK_SHADER_BASE_DIR
#    define RK_SHADER_BASE_DIR "data/shaders"
#endif

} // namespace rk
