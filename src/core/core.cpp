#include "src/core/core.h"

#include "src/core/logging.h"

using namespace Rtek;

Status RtekEngine::Initialize() 
{
    m_initialized = true;

    LOG_INFO("Engine initialized\n");
    return Status::OK;
}

Status RtekEngine::Destroy()
{
    if (!m_initialized) {
        LOG_ERROR("Attempt to destroy uninitialized engine\n");
        return Status::API_ERROR;
    }

    m_initialized = false;

    LOG_INFO("Engine destroyed\n");
    return Status::OK;
}

Status RtekEngine::Run()
{
    return Status::OK;
}

bool RtekEngine::m_initialized = false;