#include "core/ecs/components/component.h"

#include "core/math/vector.h"

namespace rk::Ecs
{
struct Transform_Component : public Component {
    Vector3 position;
};
} // namespace rk
