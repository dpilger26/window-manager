#pragma once

#include "Error.hpp"
#include "Keys.hpp"
#include "Utils.hpp"

#include "yaml-cpp/yaml.h"

#include <algorithm>
#include <execution>
#include <filesystem>
#include <iterator>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
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
        catch(const std::out_of_range&)
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

    inline void operator>>(const YAML::Node& hotKeyNode, Action& action)
    {
#ifdef DEBUG
        std::cout << "Parsing hotKey action...\n";
#endif

        const auto actionNode = hotKeyNode[ACTION_FIELD];
        if (!actionNode || actionNode.IsNull() || !actionNode.IsMap())
        {
            throwError<std::invalid_argument>("All '" + std::string(HOTKEYS_FIELD) + 
                "' must contain a " + std::string(ACTION_FIELD) + " dictionary.");
        }

        actionNode >> action.location;
        actionNode >> action.size;
    }

    //=================================================================================================================

    using Keys = std::set<int>;

    //=================================================================================================================

    inline void operator>>(const YAML::Node& hotKeyNode, Keys& keys)
    {
#ifdef DEBUG
        std::cout << "Parsing hotKey keys...\n";
#endif

        auto keyNode = hotKeyNode[KEYS_FIELD];
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
                const auto keyCode = keys::KEY_MAP.at(keyString);
                keys.insert(keyCode);
            }
            catch (const std::out_of_range&)
            {
                std::string errStr = "'" + keyString + "' is not a supported key.\nSupported keys are:\n";
                for (auto& [k, v] : keys::KEY_MAP)
                {
                    errStr += '\t' + k + '\n';
                }
                throwError<std::invalid_argument>(errStr);
            }
        }
    }

    //=================================================================================================================

    struct HotKey
    {
        std::string name{""};
        Keys        keys{};
        Action      action{};
    };

    //=================================================================================================================

    bool operator<(const HotKey& lhs, const HotKey& rhs)
    {
        int index = 0;

        if (lhs.keys.size() > rhs.keys.size())
        {
            for (const auto value : rhs.keys)
            {
                const auto diff = std::distance(lhs.keys.begin(), lhs.keys.upper_bound(value)) - index;
                if (diff == 1)
                {
                    ++index;
                    continue;
                }

                return diff > 1;
            }
        }
        else
        {
            for (const auto value : lhs.keys)
            {
                const auto diff = std::distance(rhs.keys.begin(), rhs.keys.upper_bound(value)) - index;
                if (diff == 1)
                {
                    ++index;
                    continue;
                }

                return diff < 1;
            }
        }

        return false;
    }

    //=================================================================================================================

#ifdef DEBUG
    inline std::ostream& operator<<(std::ostream& os, const HotKey& hotKey)
    {
        os << "HotKey:\n";
        os << "\tname: " << hotKey.name << '\n';
        os << "\tkeys: ";
        for (const auto key : hotKey.keys)
        {
            os << key << " ";
        }
        os << '\n';
        os << "\taction:\n";
        os << "\t\tlocation: " << static_cast<std::underlying_type<Vertical>::type>(hotKey.action.location.vertical);
        os << ", " << static_cast<std::underlying_type<Horizontal>::type>(hotKey.action.location.horizontal) << '\n';
        os << "\t\tsize: " << hotKey.action.size.vertical << ", " << hotKey.action.size.horizontal << '\n';

        return os;
    }
#endif

    //=================================================================================================================

    inline void operator>>(const YAML::Node& hotKeyNode, HotKey& hotKey)
    {
#ifdef DEBUG
        std::cout << "Parsing hotKey...\n";
#endif

        if (!hotKeyNode.IsMap())
        {
            throwError<std::invalid_argument>("All items of the '" + std::string(HOTKEYS_FIELD) + 
                "' sequence must be a dictionary");
        }

        if (hotKeyNode[NAME_FIELD])
        {
            hotKey.name = hotKeyNode[NAME_FIELD].as<std::string>();
        }

        hotKeyNode >> hotKey.keys;
        if (hotKey.keys.size() < 1)
        {
            throwError<std::invalid_argument>(std::string(KEYS_FIELD) + " cannot be emtpy.");
        }

        hotKeyNode >> hotKey.action;
    }

    //=================================================================================================================

    class Config
    {
    public:
        using iterator = std::set<HotKey>::iterator;
        using const_iterator = std::set<HotKey>::const_iterator;

        //=============================================================================================================

        Config(const std::string& configFile)
        {
            parseConfigFile(configFile);
        }

        //=============================================================================================================

        iterator begin() noexcept
        {
            return hotKeys_.begin();
        }

        //=============================================================================================================

        const_iterator begin() const noexcept
        {
            return hotKeys_.begin();
        }

        //=============================================================================================================

        iterator end() noexcept
        {
            return hotKeys_.end();
        }

        //=============================================================================================================

        const_iterator end() const noexcept
        {
            return hotKeys_.end();
        }

        //=============================================================================================================

        const_iterator getHotKey(const std::set<Keys::value_type>& keysPressed) const noexcept
        {
            if (!containsKeys(keysPressed))
            {
                return end();
            }


            // DP NOTE: can i make this search faster, instead of starting at begin() use an <algorithm> to 
            // start at a better place?
            auto iter = begin();
            for (; iter != end(); ++iter)
            {
                const auto& hotKey = *iter;
                if (std::includes(std::execution::par_unseq, 
                    keysPressed.begin(), keysPressed.end(),
                    hotKey.keys.begin(), hotKey.keys.end()))
                {
                    return iter;
                }
            }

            return end();
        }

    private:
        std::set<HotKey>            hotKeys_;
        std::set<Keys::value_type>  allKeys_;

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
            const auto hotKeyNodes = config[HOTKEYS_FIELD];

            if (!config.IsMap() || !hotKeyNodes)
            {
                throwError<std::invalid_argument>("First element of the config yaml file should be a dictionary with a single key '" + 
                    std::string(HOTKEYS_FIELD));
            }

            if (!hotKeyNodes.IsSequence())
            {
                throwError<std::invalid_argument>("'" + std::string(HOTKEYS_FIELD) + "' should be a sequence.");
            }

#ifdef DEBUG
            std::cout << "Parsing the hotKey sequence...\n";
#endif

            for (const auto& hotKeyNode : hotKeyNodes)
            {
                HotKey hotKey;
                hotKeyNode >> hotKey;
                hotKeys_.insert(hotKey);
                allKeys_.insert(hotKey.keys.begin(), hotKey.keys.end());
            }
        }

        bool containsKeys(const std::set<Keys::value_type>& keys) const noexcept
        {
            return std::includes(std::execution::par_unseq, 
                allKeys_.begin(), allKeys_.end(), 
                keys.begin(), keys.end());
        }
    };
}
