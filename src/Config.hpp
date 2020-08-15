#pragma once

#include "Error.hpp"
#include "Keys.hpp"
#include "Utils.hpp"

#include "wx-3.1/wx/defs.h"
#include "yaml-cpp/yaml.h"

#include <filesystem>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#ifdef DEBUG
#include <iostream>
#include <type_traits>
#endif

//=====================================================================================================================

namespace wm::config
{
    inline constexpr char ACTION_FIELD[] =     "action";
    inline constexpr char HOTKEYS_FIELD[] =    "hotkeys";
    inline constexpr char KEYS_FIELD[] =       "keys";
    inline constexpr char LOCATION_FIELD[] =   "location";
    inline constexpr char NAME_FIELD[] =       "name";
    inline constexpr char SIZE_FIELD[] =       "size";
    inline constexpr char VERTICAL_FIELD[] =   "vertical";
    inline constexpr char HORIZONTAL_FIELD[] = "horizontal";

    //=================================================================================================================

    enum class Vertical
    {
        Top,
        Center,
        Bottom,
        None
    };

    //=================================================================================================================

    inline const std::map<std::string, Vertical> verticalMap = {{"top", Vertical::Top},
                                                                {"center", Vertical::Center},
                                                                {"bottom", Vertical::Bottom},
                                                                {"none", Vertical::None}};

    //=================================================================================================================

    enum class Horizontal
    {
        Left,
        Center,
        Right,
        None
    };

    //=================================================================================================================

    inline const std::map<std::string, Horizontal> horizontalMap = {{"left", Horizontal::Left},
                                                                    {"center", Horizontal::Center},
                                                                    {"right", Horizontal::Right},
                                                                    {"none", Horizontal::None}};

    //=================================================================================================================

    struct Location
    {
        Vertical    vertical{Vertical::None};
        Horizontal  horizontal{Horizontal::None};
    };

    //=================================================================================================================

    template<typename MapType>
    auto getLocation(const YAML::Node& locationNode, const char* fieldName, const MapType& map)
    {
        const auto locationSubNode = locationNode[fieldName];
        if (!locationSubNode || locationSubNode.IsNull())
        {
            return MapType::mapped_type::None;
        }
        else if (!locationSubNode.IsScalar())
        {
            throwError<std::invalid_argument>("'" + std::string(fieldName) + "' should be a scalar value.");
        }

        auto locationSubStr = locationSubNode.as<std::string>();

        try
        {
            utils::tolower(locationSubStr);
            return map.at(locationSubStr);
        }
        catch(const std::out_of_range& err)
        {
            std::string errStr = "'" + locationSubStr + "' is not a valid '"; 
            errStr += std::string(fieldName) + "' '" + std::string(LOCATION_FIELD) + "' option.\n";
            errStr += "Valid options are:\n";
            for (auto& [key, value] : map)
            {
                errStr += '\t' + key + '\n';
            }

            throwError<std::invalid_argument>(errStr);
        }

        return MapType::mapped_type::None; // get rid of compiler warning
    }

    //=================================================================================================================

    inline void operator>>(const YAML::Node& actionNode, Location& location)
    {
#ifdef DEBUG
        std::cout << "Parsing action location...\n";
#endif

        const auto locationNode = actionNode[LOCATION_FIELD];
        if (!locationNode || locationNode.IsNull())
        {
            location.vertical = Vertical::None;
            location.horizontal = Horizontal::None;
            return;
        }

        if (!locationNode.IsMap())
        {
            throwError<std::invalid_argument>("'" + std::string(LOCATION_FIELD) + "' should be a map.");
        }

        location.vertical = getLocation(locationNode, VERTICAL_FIELD, verticalMap);
        location.horizontal = getLocation(locationNode, HORIZONTAL_FIELD, horizontalMap);
    }

    //=================================================================================================================

    struct Size
    {
        int vertical{-1};
        int horizontal{-1};
    };

    //=================================================================================================================

    inline int getSize(const YAML::Node& sizeNode, const char* fieldName)
    {
        const auto sizeSubNode = sizeNode[fieldName];
        if (!sizeSubNode || sizeSubNode.IsNull())
        {
            return -1;
        }
        else if (!sizeSubNode.IsScalar())
        {
            throwError<std::invalid_argument>("'" + std::string(fieldName) + "' should be a scalar value.");
        }

        int size = 0;
        try
        {
            size = sizeSubNode.as<int>();
        }
        catch(const YAML::TypedBadConversion<int>&)
        {
            throwError<std::invalid_argument>("Invalid '" + std::string(SIZE_FIELD) + 
                "' '" + std::string(fieldName) + "' value of '" + sizeSubNode.as<std::string>() + "'.");
        }
        
        if (size < 0 || size > 100)
        {
            throwError<std::invalid_argument>("'" + std::to_string(size) + "' is not a valid '" 
                + std::string(fieldName) + "' '" + std::string(SIZE_FIELD) + "' option [0, 100].");
        }
        
        return size;
    }

    //=================================================================================================================

    inline void operator>>(const YAML::Node& actionNode, Size& size)
    {
#ifdef DEBUG
        std::cout << "Parsing action size...\n";
#endif

        const auto sizeNode = actionNode[SIZE_FIELD];
        if (!sizeNode || sizeNode.IsNull())
        {
            size.vertical = -1;
            size.horizontal = -1;
            return;
        }

        if (!sizeNode.IsMap())
        {
            throwError<std::invalid_argument>("'" + std::string(SIZE_FIELD) + "' should be a map.");
        }

        size.vertical = getSize(sizeNode, VERTICAL_FIELD);
        size.horizontal = getSize(sizeNode, HORIZONTAL_FIELD);
    }

    //=================================================================================================================

    struct Action
    {
        Location    location{};
        Size        size{};
    };

    //=================================================================================================================

    inline void operator>>(const YAML::Node& hotkeyNode, Action& action)
    {
#ifdef DEBUG
        std::cout << "Parsing hotkey action...\n";
#endif

        const auto actionNode = hotkeyNode[ACTION_FIELD];
        if (!actionNode || actionNode.IsNull() || !actionNode.IsMap())
        {
            throwError<std::invalid_argument>("All '" + std::string(HOTKEYS_FIELD) + 
                "' must contain a " + std::string(ACTION_FIELD) + " dictionary.");
        }

        actionNode >> action.location;
        actionNode >> action.size;
    }

    //=================================================================================================================

    using Keys = std::vector<wxKeyCode>;

    //=================================================================================================================

    inline void operator>>(const YAML::Node& hotkeyNode, Keys& keys)
    {
#ifdef DEBUG
        std::cout << "Parsing hotkey keys...\n";
#endif

        auto keyNode = hotkeyNode[KEYS_FIELD];
        if (!keyNode || keyNode.IsNull() || !keyNode.IsSequence())
        {
            throwError<std::invalid_argument>("All '" + std::string(HOTKEYS_FIELD) + 
                "' must contain a '" + std::string(KEYS_FIELD) + "' dictionary.");
        }

        for (const auto& key : keyNode)
        {
            auto keyString = key.as<std::string>();

            try 
            {
                utils::tolower(keyString);
                const auto keyCode = keys::keyMap.at(keyString);
                keys.push_back(keyCode);
            }
            catch (const std::out_of_range& err)
            {
                std::string errStr = "'" + keyString + "' is not a supported key.\nSupported keys are:\n";
                for (auto& [key, value] : keys::keyMap)
                {
                    errStr += '\t' + key + '\n';
                }
                throwError<std::invalid_argument>(errStr);
            }
        }
    }

    //=================================================================================================================

    struct Hotkey
    {
        std::string name{""};
        Keys        keys{};
        Action      action{};
    };

    //=================================================================================================================

#ifdef DEBUG
    inline std::ostream& operator<<(std::ostream& os, const Hotkey& hotkey)
    {
        os << "Hotkey:\n";
        os << "\tname: " << hotkey.name << '\n';
        os << "\tkeys: ";
        for (const auto key : hotkey.keys)
        {
            os << key << " ";
        }
        os << '\n';
        os << "\taction:\n";
        os << "\t\tlocation: " << static_cast<std::underlying_type<Vertical>::type>(hotkey.action.location.vertical);
        os << ", " << static_cast<std::underlying_type<Horizontal>::type>(hotkey.action.location.horizontal) << '\n';
        os << "\t\tsize: " << hotkey.action.size.vertical << ", " << hotkey.action.size.horizontal << '\n';

        return os;
    }
#endif

    //=================================================================================================================

    inline void operator>>(const YAML::Node& hotkeyNode, Hotkey& hotkey)
    {
#ifdef DEBUG
        std::cout << "Parsing hotkey...\n";
#endif

        if (!hotkeyNode.IsMap())
        {
            throwError<std::invalid_argument>("All items of the '" + std::string(HOTKEYS_FIELD) + 
                "' sequence must be a dictionary");
        }

        if (hotkeyNode[NAME_FIELD])
        {
            hotkey.name = hotkeyNode[NAME_FIELD].as<std::string>();
        }
        hotkeyNode >> hotkey.keys;
        hotkeyNode >> hotkey.action;
    }

    //=================================================================================================================

    class Config
    {
    public:
        using iterator = std::vector<Hotkey>::iterator;
        using const_iterator = std::vector<Hotkey>::const_iterator;

        //=============================================================================================================

        Config(const std::string& configFile)
        {
            parseConfigFile(configFile);
        }

        //=============================================================================================================

        iterator begin() noexcept
        {
            return hotkeys_.begin();
        }

        //=============================================================================================================

        const_iterator begin() const noexcept
        {
            return hotkeys_.begin();
        }

        //=============================================================================================================

        iterator end() noexcept
        {
            return hotkeys_.end();
        }

        //=============================================================================================================

        const_iterator end() const noexcept
        {
            return hotkeys_.end();
        }

        //=============================================================================================================

        Hotkey& operator[](std::size_t i) noexcept
        {
            return hotkeys_[i];
        }

        //=============================================================================================================

        const Hotkey& operator[](std::size_t i) const noexcept
        {
            return hotkeys_[i];
        }

        //=============================================================================================================

    private:
        std::vector<Hotkey> hotkeys_;

        //=============================================================================================================

        void parseConfigFile(const std::string& configFile)
        {
#ifdef DEBUG
            std::cout << "Parsing the config.yaml file...\n";
#endif
            if (!std::filesystem::exists(std::filesystem::path(configFile)))
            {
                throwError<std::runtime_error>("configFile not found:\n\t" + configFile);
            }

            const auto config = YAML::LoadFile(configFile);
            const auto hotkeyNodes = config[HOTKEYS_FIELD];

            if (!config.IsMap() || !hotkeyNodes)
            {
                throwError<std::invalid_argument>("First element of the config yaml file should be a dictionary with a single key '" + 
                    std::string(HOTKEYS_FIELD));
            }

            if (!hotkeyNodes.IsSequence())
            {
                throwError<std::invalid_argument>("'" + std::string(HOTKEYS_FIELD) + "' should be a sequence.");
            }

#ifdef DEBUG
            std::cout << "Parsing the hotkey sequence...\n";
#endif

            for (const auto& hotkeyNode : hotkeyNodes)
            {
                Hotkey hotkey;
                hotkeyNode >> hotkey;
                hotkeys_.push_back(hotkey);
            }
        }
    };
}
