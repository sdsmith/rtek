#include "core/ecs/systems/system.h"

#include "core/ecs/components/component.h"
#include "core/ecs/components/movement_component.h"
#include "core/ecs/components/transform_component.h"
#include <vector>

namespace Rtek
{
    namespace Ecs
    {
        struct Movement_Component_Tuple : public Component_Tuple
        {
            const Movement_Component* movement;
            Transform_Component* transform;
        };

        class Movement_System : public System
        {
            using Time_Step = Time::Time_Step;

        public:
            std::vector<Movement_Component_Tuple> get_component_tuples() noexcept
            {
                // TODO(sdsmith);
                return {};
            }

            void update(Time_Step time_step) noexcept override
            {
                for (Movement_Component_Tuple& t : get_component_tuples()) {
                    t.transform->position += t.movement->velocity;
                }
            }
        };
    }
}
