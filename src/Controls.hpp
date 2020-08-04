#pragma once

#include "Buttons.hpp"

#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

#include <Windows.h>

namespace wm
{
    using CallbackFunctionType = std::function<void(bool, bool)>;

    //============================================================================================

    class CommandBase
    {
    public:
        virtual ~CommandBase() = default;
        virtual void execute(bool isPressed) = 0;
    };

    //============================================================================================

    template<typename CallbackFunction,
        std::enable_if_t<std::is_invocable_v<CallbackFunction, bool, bool>, int> = 0>
    class Command final : public CommandBase
    {
    public:
        Command(CallbackFunction callback) noexcept:
            callback_(callback)
        {}

        void execute(bool isPressed, bool resize) override
        {
            callback_(isPressed, resize);
        }

    private:
        CallbackFunction callback_;
    };

    //============================================================================================

    template<typename CallbackFunction>
    class InputHandler
    {
    public:
        InputHandler() = default;

        void addCallback()
        {

        }

        void handleInput() const noexcept
        {
            if (isPressed(Button::WINDOW))
            {
                bool resize = false;
                if (isPressed(Button::ALT))
                {
                    resize = true;
                }


            }
            leftArrowCallback_->execute(isPressed(Button::LEFT_ARROW));
            rightArrowCallback_->execute(isPressed(Button::RIGHT_ARROW));
            downArrowCallback_->execute(isPressed(Button::DOWN_ARROW));
            aButtonCallback_->execute(isPressed(Button::A));
            sButtonCallback_->execute(isPressed(Button::S));
        }

    private:
        std::map<std::unique_ptr<CommandBase>> callbacks_{};

        bool isPressed(Button button) const noexcept
        {
            return (GetKeyState(static_cast<int>(button)) & 0x8000) != 0;
        }
    };
}
