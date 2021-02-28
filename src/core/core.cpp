#include "core/core.h"

#include "core/logging/logging.h"
#include "core/utility/assert.h"

using namespace rk;

Status Rtek_Engine::initialize()
{
    RK_CHECK(Logger::initialize());
    LOG_INFO("Logger initialized");
    LOG_INFO("Initilizing engine...");

    LOG_INFO("Engine initialized");
    m_initialized = true;
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

Status Rtek_Engine::run() { return Status::OK; }

bool Rtek_Engine::m_initialized = false;
