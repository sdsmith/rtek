#include "core/core.h"

#include "core/utility/assert.h"
#include "core/logging/logging.h"

using namespace rk;

Status Rtek_Engine::initialize() 
{
    m_initialized = true;

    RK_CHECK(Logger::initialize());

    LOG_INFO("Engine initialized");
    return Status::OK;
}

Status Rtek_Engine::destroy()
{
    if (!m_initialized) {
        LOG_ERROR("Attempt to destroy uninitialized engine");
        return Status::API_ERROR;
    }

    m_initialized = false;

    LOG_INFO("Engine destroyed");
    Logger::flush();
    return Status::OK;
}

Status Rtek_Engine::run()
{
    return Status::OK;
}

bool Rtek_Engine::m_initialized = false;
