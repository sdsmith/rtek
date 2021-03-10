#include "core/ecs/components/component.h"

#include "core/math/vector.h"

namespace rk::ecs
{
struct Movement_Component : public Component {
    Vector3 velocity;
};
} // namespace rk::ecs
