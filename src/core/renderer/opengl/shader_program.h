#pragma once

#include "core/status.h"

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

    u32 handle() const noexcept { return m_id; }

    /**
     * \brief Compile the shader program.
     */
    [[nodiscard]] Status compile() noexcept;
};
} // namespace rk
