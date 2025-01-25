#pragma once

#include <string>
#include <array>
#include <vector>

#include <exception>
#include <stdexcept>

#include <memory>
#include <functional>
#include <utility>

#include <iostream>


#ifndef out
    #include <iostream>
    #define out(text) std::cout << text << "\n"
#endif

namespace maple
{

    namespace configuration
    {

        inline constexpr int opengl_version_major = 4;
        inline constexpr int opengl_version_minor = 5;

    }

    namespace event_type
    {

        using WindowCloseAttempt    = std::function<bool()>;
        using WindowClose           = std::function<void()>;

    }

    struct Point
    {
        int x{ 0 };
        int y{ 0 };
    };

    struct Size
    {
        int width{ 0 };
        int height{ 0 };
    };

}