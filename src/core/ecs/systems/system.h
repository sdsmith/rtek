#include "core/utility/time.h"

namespace rk
{
    namespace Ecs
    {
        class System
        {
            using Time_Step = Time::Time_Step;

        public:
            virtual void update(Time_Step time_step) = 0;

            //void NotifyComponent(Component*)
        };
    }
}
