#include "core/ecs/components/component.h"

#include "core/math/vector.h"

namespace Rtek
{
    namespace Ecs
    {
        struct Movement_Component : public Component
        {
            Vector3 velocity;
        };
    }
}
