#ifndef DEBUG
#define DEBUG
#endif

#include "Config.hpp"

#include <cstdlib>
#include <iostream>

//=====================================================================================================================

int main()
{
#ifdef DEBUG
    std::cout << "Starting Window-Manager\n";
#endif

    const auto config = wm::config::Config("/home/dpilger26/GitHub/window-manager/config/config.yaml");

#ifdef DEBUG
    for (const auto& hotkey : config)
    {
        std::cout << hotkey;
    }
#endif

    return EXIT_SUCCESS;
}
