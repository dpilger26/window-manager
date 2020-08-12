#pragma once

#include "Buttons.hpp"

#include "wx-3.1/wx/defs.h"
#include "yaml-cpp/yaml.h"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include <iostream>

namespace wm::config
{
    enum class Vertical
    {
        Top,
        Center,
        Bottom
    };

    enum class Horizontal
    {
        Left,
        Center,
        Right
    };

    struct Location
    {
        Vertical    vertical{Vertical::Center};
        Horizontal  horizontal{Horizontal::Center};
    };

    struct Size
    {
        int vertical{100};
        int horizontal{100};
    };

    struct Action
    {
        Location    location{};
        Size        size{};
    };

    struct Hotkey
    {
        std::string             name{""};
        std::vector<wxKeyCode>  keys{};
        Action                  action{};
    };

    using Hotkeys = std::vector<Hotkey>;

    Hotkeys parseConfigFile(const std::string& configFile)
    {
        if (!std::filesystem::exists(std::filesystem::path(configFile)))
        {
            throw std::runtime_error("configFile not found:\n\t" + configFile);
        }

        YAML::Node config = YAML::LoadFile(configFile);

        for (const auto& item : config)
        {
            std::cout << item << '\n';
        }

        return {};
    }
}
