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

}

#include "event.h"