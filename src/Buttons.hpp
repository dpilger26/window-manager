#pragma once

#include <type_traits>

namespace wm
{
    // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    enum class Button
    {
        WINDOW = 0x5B,
        ALT = 0x12,
        NUMPAD_0 = 0x60,
        NUMPAD_1 = 0x61,
        NUMPAD_2 = 0x62,
        NUMPAD_3 = 0x63,
        NUMPAD_4 = 0x64,
        NUMPAD_5 = 0x65,
        NUMPAD_6 = 0x66,
        NUMPAD_7 = 0x67,
        NUMPAD_8 = 0x68,
        NUMPAD_9 = 0x69,
    };

    template<typename CallbackFunction,
        std::enable_if_t<std::is_invocable_v<CallbackFunction>, int> = 0>
    class ButtonCallback
    {
    public:
        ButtonCallback(CallbackFunction callbackFunction) noexcept:
            callbackFunction_(callbackFunction)
        {}

        void operator()(bool isKeyDown)
        {
            if (isKeyDown)
            {
                callbackFunction_();
            }
        }

    private:
        CallbackFunction callbackFunction_{};
    };
}
