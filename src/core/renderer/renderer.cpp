#include "core/renderer/renderer.h"

#include "GLFW/glfw3.h"
#include "core/logging/logging.h"
#include "core/platform/glfw.h"
#include "core/types.h"
#include "core/utility/assert.h"
#include "core/utility/no_exception.h"
#include "fmt/core.h"
#include "sds/string.h"
#include <cstring>
#include <fstream>
#include <iterator>

// Ask for a high performance renderer
#include "core/renderer/request_high_perf_renderer.h"

/**
 * \def RK_SHADER_BASE_DIR
 * \brief Base path to the shader directory.
 */
#ifndef RK_SHADER_BASE_DIR
#    define RK_SHADER_BASE_DIR "data/shaders"
#endif

using namespace rk;

Status Renderer::initialize() noexcept { return Status::ok; }

Status Renderer::destroy() noexcept { return Status::ok; }

Status Renderer::prepare_window() noexcept
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_ogl_ctx_version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_ogl_ctx_version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef RK_OGL_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
#endif

    return platform::glfw::handle_error();
}

void Renderer::set_window(GLFWwindow* window) noexcept
{
    RK_ASSERT(window);
    m_window = window;
}

/**
 * \brief Convert an OpenGL error to a string.
 */
char const* to_string_ogl_error(GLenum ogl_error) noexcept
{
    switch (ogl_error) {
        case GL_NO_ERROR: return "NO_ERROR";
        case GL_INVALID_ENUM: return "INVALID_ENUM";
        case GL_INVALID_VALUE: return "INVALID_VALUE";
        case GL_INVALID_OPERATION: return "INVALID_OPERATION";
        case GL_STACK_OVERFLOW: return "STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW: return "STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY: return "OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID_FRAMEBUFFER_OP";
        case GL_CONTEXT_LOST: return "CONTEXT_LOST";
    }

    LOG_CRITICAL("Unknown OpenGL error: {}", ogl_error);
    RK_ASSERT(0);
    return "unknown";
}

/**
 * \brief Handle an OpenGL error. Return ok if no error.
 */
Status Renderer::handle_ogl_error() const noexcept
{
    GLenum err = GL_NO_ERROR;
    bool was_err = false;
    while ((err = glGetError()) != GL_NO_ERROR) {
        LOG_ERROR("OpenGL error: {}", to_string_ogl_error(err));
        was_err = true;
    }

    if (was_err) { return Status::renderer_error; }

    return Status::ok;
}

/**
 * \brief Convert OpenGL \A KHR_debug type to string.
 */
RK_INTERNAL
char const* to_string_ogl_debug_type(GLenum type) noexcept
{
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: return "error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "deprecated behaviour";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UB";
        case GL_DEBUG_TYPE_PORTABILITY: return "portability";
        case GL_DEBUG_TYPE_PERFORMANCE: return "performance";
        case GL_DEBUG_TYPE_OTHER: return "other";
    }

    LOG_CRITICAL("Unknown OpenGL debug type: {}", type);
    RK_ASSERT(0);
    return "unknown";
}

/**
 * \brief Convert OpenGL \A KHR_debug severity to string.
 */
RK_INTERNAL
char const* to_string_ogl_debug_severity(GLenum severity) noexcept
{
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: return "high";
        case GL_DEBUG_SEVERITY_MEDIUM: return "medium";
        case GL_DEBUG_SEVERITY_LOW: return "low";
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "notification";
    }

    LOG_CRITICAL("Unknown OpenGL severity type: {}", severity);
    RK_ASSERT(0);
    return "unknown";
}

/**
 * \brief Convert OpenGL \A KHR_debug source to string.
 */
RK_INTERNAL
char const* to_string_ogl_debug_source(GLenum source) noexcept
{
    switch (source) {
        case GL_DEBUG_SOURCE_API: return "api";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "window system";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "shader compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "3rd party";
        case GL_DEBUG_SOURCE_APPLICATION: return "application";
        case GL_DEBUG_SOURCE_OTHER: return "other";
    }

    LOG_CRITICAL("Unknown OpenGL source type: {}", source);
    RK_ASSERT(0);
    return "unknown";
}

/**
 * \brief Callback to process OpenGL \a KHR_debug messages.
 *
 * \see glDebugMessageCallback
 */
RK_INTERNAL
void GLAPIENTRY ogl_debug_callback(GLenum source, GLenum type, u32 id, GLenum severity, s32 length,
                                   char const* message, void const* user_param) noexcept
{
    RK_ASSERT(message);
    RK_ASSERT(!user_param); // warn when user param is set

    // @cleanup: adjust log level of message based on severity based on a param
    // (requires new LOG(level, ...) macro

    if (type == GL_DEBUG_TYPE_ERROR) {
        LOG_ERROR("[OpenGL] type: {}, source: {}, id: {}, severity: {}, message: {}",
                  to_string_ogl_debug_type(type), to_string_ogl_debug_source(source), id,
                  to_string_ogl_debug_severity(severity), message);

        // TODO(sdsmith): Fail?
        RK_ASSERT(!"OGL error");

    } else {
        LOG_DEBUG("[OpenGL] type: {}, source: {}, id: {}, severity: {}, message: {}",
                  to_string_ogl_debug_type(type), to_string_ogl_debug_source(source), id,
                  to_string_ogl_debug_severity(severity), message);
    }
}

// TODO(sdsmith): this feels very platform-y
Status Renderer::setup_gl_api() noexcept
{
    // Renderer's window must be the current rendering context.
    RK_ASSERT(m_window == glfwGetCurrentContext() && platform::glfw::handle_error() == Status::ok);

    // Initialize GLAD with the OS-specific OpenGL function pointers
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        LOG_CRITICAL("Failed to initialize GLAD");
        return Status::renderer_error;
    }

    // Print renderer information
    LOG_INFO(
        "Renderer info:\n"
        "  Vendor       : {}\n"
        "  Renderer     : {}\n"
        "  Graphics API : OpenGL {}",
        glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_VERSION));

    // Check that we are in a debug context
    s32 ctx_flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &ctx_flags);
    bool const is_ogl_debug_ctx = (ctx_flags & GL_CONTEXT_FLAG_DEBUG_BIT);
    LOG_INFO("OpenGL debug context: {}", (is_ogl_debug_ctx ? "true" : "false"));

#ifdef RK_OGL_DEBUG
    RK_ASSERT(is_ogl_debug_ctx);

    // NOTE(sdsmith): @perf: Enabling synchronous debug callbacks is slow, but provides
    // better debugging. By default is is async, which has no guarantees that it
    // is called on the same thread or at the same time that the message was
    // created. Async is faster.
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(ogl_debug_callback, nullptr /*user_param*/);

    // Enable all messages
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    char const* khr_debug_msg = "Setup KHR_debug message callback";
    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, -1,
                         GL_DEBUG_SEVERITY_NOTIFICATION, std::strlen(khr_debug_msg), khr_debug_msg);
    RK_CHECK(handle_ogl_error());
#endif

    LOG_INFO(
        "OpenGL context info:\n"
        "  Debug: {}",
        sds::to_string(is_ogl_debug_ctx));

    s32 window_w = 0;
    s32 window_h = 0;
    glfwGetWindowSize(m_window, &window_w, &window_h);
    RK_CHECK(platform::glfw::handle_error());
    glViewport(0, 0, window_w, window_h);

    LOG_INFO("OpenGL context initialized: {}", get_gl_api_version());
    return Status::ok;
}

Status Renderer::swap_buffers() const noexcept
{
    glfwSwapBuffers(m_window);
    return platform::glfw::handle_error();
}

/**
 * \brief Window resize callback function.
 */
RK_INTERNAL
void framebuffer_size_callback(GLFWwindow* window, s32 width, s32 height) noexcept
{
    RK_ASSERT(window);
    LOG_INFO("Window resized to width: {}, height: {} - adjusting viewport", width, height);
    glViewport(0, 0, width, height);
}

GLFWframebuffersizefun Renderer::get_framebuffer_size_callback() const noexcept
{
    return framebuffer_size_callback;
}

std::string Renderer::get_gl_api_version() const noexcept
{
    return fmt::format("OpenGL {}.{} core", m_ogl_ctx_version_major, m_ogl_ctx_version_minor);
}

std::string Renderer::get_program_info_log(u32 id) const noexcept
{
    s32 log_len = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_len);
    std::string log(log_len, '\0');
    glGetShaderInfoLog(id, log_len, nullptr, log.data());
    return log;
}

Status Renderer::compile_shader(char const* name, u32 shader_id) const noexcept
{
    RK_ASSERT(name);

    // TODO(sdsmith): LOG_ERROR in lambda has bad function name. Make LOG variant that passes
    // the func name?
    auto ret = exception_boundary([&]() {
        std::string shader_path = fmt::format("{}/{}", RK_SHADER_BASE_DIR, name);
        std::ifstream f(shader_path, std::ios::binary);
        if (f.fail()) {
            LOG_ERROR("Failed to open shader file '{}'", shader_path);
            return Status::io_error;
        }

        std::string shader;

        f.seekg(0, std::ios::end);
        if (f.fail()) {
            LOG_ERROR("Failed to seek to EOF of shader file '{}'", shader_path);
            return Status::io_error;
        }
        shader.reserve(f.tellg());
        f.seekg(0, std::ios::beg);
        if (f.fail()) {
            LOG_ERROR("Failed to seek to start of shader file '{}'", shader_path);
            return Status::io_error;
        }

        shader.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

        f.close();
        if (f.fail()) {
            LOG_WARN("Failed to close shader file '{}', but not much we can do!", shader_path);
        }

        char const* shader_contents = shader.c_str();

        glShaderSource(shader_id, 1, &shader_contents, nullptr);
        glCompileShader(shader_id);

        s32 compile_success = 0;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_success);
        if (!compile_success) {
            LOG_ERROR("Failed to compile shader {}: {}", shader_id,
                      get_program_info_log(shader_id));
        }

        LOG_DEBUG("Compiled shader id {} ('{}')", shader_id, shader_path);
        return Status::ok;
    });
    auto p_status = std::get_if<0>(&ret);
    RK_ASSERT(p_status); // must return a status
    return *p_status;
}