#include "yaml-cpp/yaml.h"

#include <cstdlib>
#include <iostream>

int main()
{
    YAML::Node hotkeys = YAML::LoadFile("/home/dpilger26/GitHub/window-manager/config/config.yaml");




    return EXIT_SUCCESS;
}
