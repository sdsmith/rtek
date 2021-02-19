#include "core/ecs/components/component.h"

#include "core/math/vector.h"

namespace rk
{
namespace Ecs
{
struct Movement_Component : public Component {
    Vector3 velocity;
};
} // namespace Ecs
} // namespace rk
