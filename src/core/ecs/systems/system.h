#include "core/utility/time.h"

namespace rk::ecs
{
class System {
    using Time_Step = time::Time_Step;

public:
    virtual void update(Time_Step time_step) = 0;

    // void NotifyComponent(Component*)
};
} // namespace rk::ecs
