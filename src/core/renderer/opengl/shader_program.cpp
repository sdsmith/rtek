#include "core/renderer/opengl/shader_program.h"

#include "core/assert.h"
#include "core/logging/logging.h"
#include "core/platform/stdlib/fstream.h"
#include "core/utility/no_exception.h"
#include <glad/glad.h>
#include <string>
#include <glm/gtc/type_ptr.hpp>

using namespace rk;
using namespace sds;

RK_INTERNAL
std::string get_program_info_log(u32 id) noexcept
{
    s32 log_len = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_len);
    std::string log(log_len, '\0');
    glGetShaderInfoLog(id, log_len, nullptr, log.data());
    return log;
}

RK_INTERNAL
Status compile_shader(char const* name, s32 shader_id) noexcept
{
    RK_ASSERT(name);

    // TODO(sdsmith): LOG_ERROR in lambda has bad function name. Make LOG variant that passes
    // the func name?
    auto ret = exception_boundary([&]() {
        std::string shader_path = fmt::format("{}/{}", RK_SHADER_BASE_DIR, name);
        rk::ifstream f(shader_path, std::ios::binary);
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
            LOG_ERROR("Failed to compile shader {} ('{}'): {}",
                shader_id,
                shader_path,
                get_program_info_log(shader_id));
        }

        LOG_DEBUG("Compiled shader id {} ('{}')", shader_id, shader_path);
        return Status::ok;
    });
    auto p_status = std::get_if<Status>(&ret);
    RK_ASSERT(p_status); // must return a status
    return *p_status;
}

Shader_Program::Shader_Program(char const* vert_shader_path, char const* frag_shader_path) noexcept
    : m_vert_shader_path(vert_shader_path), m_frag_shader_path(frag_shader_path)
{
    RK_ASSERT(vert_shader_path);
    RK_ASSERT(frag_shader_path);
}

Shader_Program::~Shader_Program() noexcept { destroy(); }

void Shader_Program::destroy() noexcept
{
    if (m_id != invalid_handle) {
        glDeleteProgram(m_id);
        m_id = invalid_handle;
    }

    RK_ASSERT(m_id == invalid_handle);
}

void Shader_Program::use() const noexcept
{
    RK_ASSERT(m_id != invalid_handle); // Must be initialized
    glUseProgram(m_id);
}

bool Shader_Program::is_active() const noexcept {
    RK_ASSERT(m_id != invalid_handle); // Must be initialized
    s64 cur_id = 0;
    glGetInteger64v(GL_CURRENT_PROGRAM, &cur_id);
    return cur_id == m_id;
}

Status Shader_Program::compile() noexcept
{
    if (m_id != invalid_handle) {
        LOG_WARN("Attemping to re-compile shader (id {}), skipping compilation...", m_id);
        return Status::ok;
    }

    m_id = glCreateProgram();

    s32 const vert_shader = glCreateShader(GL_VERTEX_SHADER);
    s32 const frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

    RK_CHECK(compile_shader(m_vert_shader_path, vert_shader));
    RK_CHECK(compile_shader(m_frag_shader_path, frag_shader));

    glAttachShader(m_id, vert_shader);
    glAttachShader(m_id, frag_shader);
    glLinkProgram(m_id);

    s32 link_success = 0;
    glGetProgramiv(m_id, GL_LINK_STATUS, &link_success);
    if (!link_success) {
        LOG_ERROR("Failed to link shader program {}: {}", m_id, get_program_info_log(m_id));
        return Status::renderer_error;
    }

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    return Status::ok;
}

void Shader_Program::set_bool(char const* name, bool v) const noexcept {
    set_s32(name, static_cast<s32>(v));
}

void Shader_Program::set_s32(char const* name, s32 v) const noexcept {
    RK_ASSERT(name);
    RK_ASSERT(is_active());
    glUniform1i(glGetUniformLocation(m_id, name), v);
}

void Shader_Program::set_u32(char const* name, u32 v) const noexcept {
    RK_ASSERT(name);
    RK_ASSERT(is_active());
    glUniform1ui(glGetUniformLocation(m_id, name), v);
}

void Shader_Program::set_f32(char const* name, f32 v) const noexcept {
    RK_ASSERT(name);
    RK_ASSERT(is_active());
    glUniform1f(glGetUniformLocation(m_id, name), v);
}

void Shader_Program::set_vec2(char const* name, f32 x, f32 y) const noexcept {
    RK_ASSERT(name);
    RK_ASSERT(is_active());
    glUniform2f(glGetUniformLocation(m_id, name), x, y);
}

void Shader_Program::set_vec2(char const* name, glm::vec2 const& v) const noexcept {
    RK_ASSERT(name);
    RK_ASSERT(is_active());
    glUniform2fv(glGetUniformLocation(m_id, name), 1, glm::value_ptr(v));
}

void Shader_Program::set_vec3(char const* name, f32 x, f32 y, f32 z) const noexcept {
    RK_ASSERT(name);
    RK_ASSERT(is_active());
    glUniform3f(glGetUniformLocation(m_id, name), x, y, z);
}

void Shader_Program::set_vec3(char const* name, glm::vec3 const& v) const noexcept {
    RK_ASSERT(name);
    RK_ASSERT(is_active());
    glUniform3fv(glGetUniformLocation(m_id, name), 1, glm::value_ptr(v));
}

void Shader_Program::set_vec4(char const* name, f32 x, f32 y, f32 z, f32 w) const noexcept {
    RK_ASSERT(name);
    RK_ASSERT(is_active());
    glUniform4f(glGetUniformLocation(m_id, name), x, y, z, w);
}

void Shader_Program::set_vec4(char const* name, glm::vec4 const& v) const noexcept {
    RK_ASSERT(name);
    RK_ASSERT(is_active());
    glUniform4fv(glGetUniformLocation(m_id, name), 1, glm::value_ptr(v));
}

void Shader_Program::set_mat2(char const* name, glm::mat2 const& v) const noexcept {
    RK_ASSERT(name);
    RK_ASSERT(is_active());
    glUniformMatrix2fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(v));
}

void Shader_Program::set_mat3(char const* name, glm::mat3 const& v) const noexcept {
    RK_ASSERT(name);
    RK_ASSERT(is_active());
    glUniformMatrix3fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(v));
}

void Shader_Program::set_mat4(char const* name, glm::mat4 const& v) const noexcept {
    RK_ASSERT(name);
    RK_ASSERT(is_active());
    glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, glm::value_ptr(v));
}
