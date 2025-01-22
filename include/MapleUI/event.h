#pragma once

#include <functional>

namespace maple
{

    namespace event_type
    {

        using WindowCloseAttempt            = std::function<bool()>;
        using WindowClose                   = std::function<void()>;

    }

}