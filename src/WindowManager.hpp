#pragma once

#include "Config.hpp"
#include "Constants.hpp"
#include "Keys.hpp"
#include "WindowController.hpp"

#ifdef DEBUG
#include <iostream>
#endif
#include <string>
#include <thread>

//=====================================================================================================================

namespace wm
{
    class WindowManager
    {
    public:
        WindowManager(const std::string& configFile):
            config_(configFile)
        {
#ifdef DEBUG
            std::cout << "Starting Window-Manager version " << constants::VERSION << '\n';

            for (const auto& hotkey : config_)
            {
                std::cout << hotkey;
            }
#endif
        }

        //=============================================================================================================

        void run() const noexcept
        {
            while (true)
            {
                const auto iter = config_.getHotKey(wm::keys::get_keys_pressed());
                if (iter != config_.end())
                {
                    const auto& hotKey = *iter;
#ifdef DEBUG
                    std::cout << hotKey.name << '\n';
#endif
                    controller_.doIt(hotKey.action);
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(constants::LOOP_DELAY_MS));
            }
        }

        //=============================================================================================================
    private:
        config::Config      config_;
        WindowController    controller_{};
    };
}
