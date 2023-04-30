#pragma once

#include "core/status.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

namespace rk
{
class Shader_Program {
    /*
     * @optimization: Might be worth it to have all the shaders compiled and
     * then link them all at once instead of compiling, linking, deleting per
     * program. Would save time if shaders are reused in many programs. Can
     * facilitate this by using a resource manager to manage the lifetime of the
     * shaders and then doing a mass "delete all shaders" at some specific
     * interval.
     */

private:
    /** Shader ID */
    u32 m_id = 0;

    // TODO(sdsmith): risky to hold string pointers since they might be temps. This solves itself
    // once we have resource ids.
    const char* m_vert_shader_path = nullptr;
    const char* m_frag_shader_path = nullptr;

public:
    static constexpr const u32 invalid_handle = 0;

    /**
     * \brief Set the shaders to be used with the shader prorgam.
     */
    // TODO(sdsmith): @perf: replace with resource id
    Shader_Program(char const* vert_shader_path, char const* frag_shader_path) noexcept;
    ~Shader_Program() noexcept;

    /**
     * Destroy the object and free resources.
     */
    void destroy() noexcept;

    /**
     * \brief Set as the active shader program.
     */
    void use() const noexcept;

    /**
     * \brief True if the active program (ie. in use).
     *
     * \see use
     */
    [[nodiscard]] bool is_active() const noexcept;

    /**
     * \brief Handle/ID of shader program.
     *
     * Program is not compiled if value is invalid.
     *
     * \see invalid_handle Value of invalid handle.
     * \see compile
     */
    [[nodiscard]] u32 handle() const noexcept { return m_id; }

    /**
     * \brief Compile the shader program.
     */
    [[nodiscard]] Status compile() noexcept;

    void set_bool(char const* name, bool v) const noexcept;
    void set_s32(char const* name, s32 v) const noexcept;
    void set_u32(char const* name, u32 v) const noexcept;
    void set_f32(char const* name, f32 v) const noexcept;
    void set_vec2(char const* name, f32 x, f32 y) const noexcept;
    void set_vec2(char const* name, glm::vec2 const& v) const noexcept;
    void set_vec3(char const* name, f32 x, f32 y, f32 z) const noexcept;
    void set_vec3(char const* name, glm::vec3 const& v) const noexcept;
    void set_vec4(char const* name, f32 x, f32 y, f32 z, f32 w) const noexcept;
    void set_vec4(char const* name, glm::vec4 const& v) const noexcept;
    void set_mat2(char const* name, glm::mat2 const& v) const noexcept;
    void set_mat3(char const* name, glm::mat3 const& v) const noexcept;
    void set_mat4(char const* name, glm::mat4 const& v) const noexcept;
};
} // namespace rk
