#include "Config.hpp"

#include <cstdlib>
#include <iostream>

int main()
{
    auto hotkeys = wm::config::parseConfigFile("/home/dpilger26/GitHub/window-manager/config/config.yaml");
    
    return EXIT_SUCCESS;
}
