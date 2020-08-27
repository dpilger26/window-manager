#include "WindowManager.hpp"

#include <cstdlib>

//=====================================================================================================================

int main()
{
    const auto windowManager = wm::WindowManager("C:/Github/window-manager/config/config.yaml");
    windowManager.run();

    return EXIT_SUCCESS;
}
