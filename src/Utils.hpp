#pragma once

#include <algorithm>
#include <cctype>
#include <execution>
#include <string>

//=====================================================================================================================

namespace wm::utils
{
    inline void tolower(std::string& str) noexcept
    {
        std::for_each(std::execution::par_unseq, str.begin(), str.end(), 
            [](auto& character)
            {
                character = std::tolower(character);
            });
    }
}
