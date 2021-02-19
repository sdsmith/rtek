#pragma once

#include "core/types.h"
#include "core/ecs/entity.h"
#include "core/ecs/systems/system.h"
#include "core/utility/time.h"
#include <map>
#include <vector>

namespace rk
{
    namespace Ecs
    {
        class Entity_Manager
        {
            using Time_Step = Time::Time_Step;

        public:
            void add_entity(Entity_Definition const& def) noexcept;

            void update(Time_Step time_step) noexcept;

        private:
            //std::map<Entity_Id, Entity*> m_entity_map;
            std::vector<System*> m_systems;
            //Object_Pool<Component>*
            //std::vector<Component*> components;
        };

    }
}
