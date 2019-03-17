#include "core/ecs/entity_manager.h"

#include "core/utility/assert.h"

using namespace Rtek::Ecs;

void Entity_Manager::add_entity(const Entity_Definition& def) noexcept
{
    RTK_ASSERT(!"unimplemented");
}

void Entity_Manager::update(Time_Step time_step) noexcept
{
    for (System* sys : m_systems) {
        sys->update(time_step);
    }
}
