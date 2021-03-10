#include "core/ecs/components/component.h"
#include "core/types.h"
#include <map>
#include <vector>

namespace rk::ecs
{
/*
using Entity_Id_Base_Type = s32;
struct Entity_Id
    : type_safe::strong_typedef<Entity_Id, Entity_Id_Base_Type>,
    type_safe::strong_typedef_op::equality_comparison<Entity_Id>,
    type_safe::strong_typedef_op::relational_comparison<Entity_Id>
{
    using strong_typedef::strong_typedef;

    constexpr Entity_Id_Base_Type get() const noexcept { return
static_cast<Entity_Id_Base_Type>(*this); }
};

struct Entity
{
    Entity_Id id;
    std::vector<Component*> components;
};
*/

class Entity_Definition {}; // TODO(sdsmith):
} // namespace rk::ecs
