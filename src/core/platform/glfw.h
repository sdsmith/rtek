#pragma once

#include "core/types.h"
#include "core/utility/assert.h"
#include "core/utility/status.h"

// clang-format off
//

#if RK_OS == RK_OS_WINDOWS
// NOTE(sdsmith): windows.h must be included before GLAD. GLAD1 defines
// APIENTRY, which is also defined by windows. GLAD2 solves this problem, but
// there is no easy way around this in GLAD1.
#   include <Windows.h>
#endif

// NOTE(sdsmith): GLAD must be included before any other OpenGL related header.
// GLAD includes the OpenGL headers.
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

// Check that glfw peimitives types match the engine's
RK_STATIC_ASSERT_MSG(sizeof(int) == sizeof(rk::s32), "glfw sizes don't match");

// Check that OpenGL primitives match the engine's
RK_STATIC_ASSERT_MSG(sizeof(GLboolean) == sizeof(bool), "OpenGL sizes don't match");
RK_STATIC_ASSERT_MSG(sizeof(GLchar) == sizeof(char), "OpenGL sizes don't match");
RK_STATIC_ASSERT_MSG(sizeof(GLbyte) == sizeof(rk::s8), "OpenGL sizes don't match");
RK_STATIC_ASSERT_MSG(sizeof(GLubyte) == sizeof(rk::u8), "OpenGL sizes don't match");
RK_STATIC_ASSERT_MSG(sizeof(GLshort) == sizeof(rk::s16), "OpenGL sizes don't match");
RK_STATIC_ASSERT_MSG(sizeof(GLushort) == sizeof(rk::u16), "OpenGL sizes don't match");
RK_STATIC_ASSERT_MSG(sizeof(GLint) == sizeof(rk::s32), "OpenGL sizes don't match");
RK_STATIC_ASSERT_MSG(sizeof(GLuint) == sizeof(rk::u32), "OpenGL sizes don't match");
// GLfixed
RK_STATIC_ASSERT_MSG(sizeof(GLint64) == sizeof(rk::s64), "OpenGL sizes don't match");
RK_STATIC_ASSERT_MSG(sizeof(GLuint64) == sizeof(rk::u64), "OpenGL sizes don't match");
RK_STATIC_ASSERT_MSG(sizeof(GLsizei) == sizeof(rk::s32), "OpenGL sizes don't match");
// GLenum
// GLintptr
// GLsizeiptr
// GLsync
// GLbitfield
// GLhalf
RK_STATIC_ASSERT_MSG(sizeof(GLfloat) == sizeof(rk::f32), "OpenGL sizes don't match");
// GLclampf
RK_STATIC_ASSERT_MSG(sizeof(GLdouble) == sizeof(rk::f64), "OpenGL sizes don't match");
// GLclampd

/**
 * \file glfw.h
 * \brief GLFW wrapper. Initialize here and then use GLFW as normal.
 */

/*
 * NOTE(sdsmith): GLFW errors are always non-fatal. The lib will remain in a valid state as long as
 * glfwInit was successful.
 */

// TODO(sdsmith): use glfwSetCharCallback for unicode input for text

namespace rk::platform::glfw
{
/**
 * \brief Check for a GLFW error. Terminate glfw on error. Return appropriate status code.
 */
Status handle_error() noexcept;

/**
 * \brief Initialize GLFW.
 */
[[nodiscard]] Status initialize() noexcept;

/**
 * \brief Destroy GLFW.
 */
Status destroy() noexcept;

/**
 * \def RK_CHECK_GLFW
 * \brief Check for GLFW error. Handle and return on error.
 */
#define RK_CHECK_GLFW(func_call)                        \
    do {                                                \
        func_call;                                      \
        RK_CHECK(::rk::platform::glfw::handle_error()); \
    } while (0)
} // namespace rk::platform::glfw
