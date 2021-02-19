#include "core/core.h"

#include "core/utility/assert.h"
#include "core/logging/logging.h"

using namespace rk;

Status Rtek_Engine::initialize() 
{
    m_initialized = true;

    RTK_CHECK(Logger::initialize());

    LOG_INFO("Engine initialized\n");
    return Status::OK;
}

Status Rtek_Engine::destroy()
{
    if (!m_initialized) {
        LOG_ERROR("Attempt to destroy uninitialized engine\n");
        return Status::API_ERROR;
    }

    m_initialized = false;

    LOG_INFO("Engine destroyed\n");
    Logger::flush();
    return Status::OK;
}

Status Rtek_Engine::run()
{
    return Status::OK;
}

bool Rtek_Engine::m_initialized = false;
