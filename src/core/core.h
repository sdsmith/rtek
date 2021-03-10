#pragma once

#include "core/platform/input_manager.h"
#include "core/platform/window_manager.h"
#include "core/renderer/renderer.h"
#include "core/status.h"
#include "core/types.h"
#include <memory>

namespace rk
{
class Rtek_Engine {
public:
    Status initialize() noexcept;
    Status destroy() noexcept;

    Status run() noexcept;

private:
    static bool m_initialized;
    std::unique_ptr<Window_Manager> m_window_mgr;
    std::unique_ptr<Input_Manager> m_input_mgr;
    std::unique_ptr<Renderer> m_renderer;
};
} // namespace rk
