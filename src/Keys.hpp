#pragma once

#include <Windows.h>

#include <map>
#include <set>
#include <string>

//=====================================================================================================================

namespace wm::keys
{
    // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    inline const std::map<std::string, int> KEY_MAP = {{"a", 0x41},
                                                      {"b", 0x42},
                                                      {"c", 0x43},
                                                      {"d", 0x44},
                                                      {"e", 0x45},
                                                      {"f", 0x46},
                                                      {"g", 0x47},
                                                      {"h", 0x48},
                                                      {"i", 0x49},
                                                      {"j", 0x4A},
                                                      {"k", 0x4B},
                                                      {"l", 0x4C},
                                                      {"m", 0x4D},
                                                      {"n", 0x4E},
                                                      {"o", 0x4F},
                                                      {"p", 0x50},
                                                      {"q", 0x51},
                                                      {"r", 0x52},
                                                      {"s", 0x53},
                                                      {"t", 0x54},
                                                      {"u", 0x55},
                                                      {"v", 0x56},
                                                      {"w", 0x57},
                                                      {"x", 0x58},
                                                      {"y", 0x59},
                                                      {"z", 0x5A},
                                                      {"backspace", 0x08},
                                                      {"tab", 0x09},
                                                      {"return", 0x0D},
                                                      {"esc", 0x1B},
                                                      {"space", 0x20},
                                                      {"delete", 0x2E},
                                                      {"shift", 0x10},
                                                      {"alt", 0x12},
                                                      {"ctrl", 0x11},
                                                      {"menu", 0xA5},
                                                      {"end", 0x23},
                                                      {"home", 0x24},
                                                      {"left", 0x25},
                                                      {"up", 0x26},
                                                      {"right", 0x27},
                                                      {"down", 0x28},
                                                      {"insert", 0x2D},
                                                      {"0", 0x60},
                                                      {"1", 0x61},
                                                      {"2", 0x62},
                                                      {"3", 0x63},
                                                      {"4", 0x64},
                                                      {"5", 0x65},
                                                      {"6", 0x66},
                                                      {"7", 0x67},
                                                      {"8", 0x68},
                                                      {"9", 0x69},
                                                      {"+", 0x6B},
                                                      {"-", 0x6D},
                                                      {"*", 0x6A},
                                                      {"/", 0x6F},
                                                      {"decimal", 0x6E},
                                                      {"numlock", 0x90},
                                                      {"f1", 0x60},
                                                      {"f2", 0x61},
                                                      {"f3", 0x62},
                                                      {"f4", 0x63},
                                                      {"f5", 0x64},
                                                      {"f6", 0x65},
                                                      {"f7", 0x66},
                                                      {"f8", 0x67},
                                                      {"f9", 0x68},
                                                      {"f10", 0x69},
                                                      {"f11", 0x6A},
                                                      {"f12", 0x6B},
                                                      {"pageup", 0x21},
                                                      {"pagedown", 0x22},
                                                      {"window", 0x5B}};

    std::set<int> get_keys_pressed()
    {
        std::set<int> pressed_keys;
        for (auto& [key, value] : KEY_MAP)
        {
            if ((GetKeyState(value) & 0x8000) != 0)
            {
                pressed_keys.insert(value);
            }
        }

        return pressed_keys;
    }
}
