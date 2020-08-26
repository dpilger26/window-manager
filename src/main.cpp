#ifndef DEBUG
#define DEBUG
#endif

#include "Config.hpp"
#include "Keys.hpp"

#include <Windows.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

std::string GetActiveWindowTitle()
{
    char wnd_title[256];
    auto hwnd = GetForegroundWindow(); // get handle of currently active window
    GetWindowText(hwnd, wnd_title, sizeof(wnd_title));
    std::cout << wnd_title << '\n';
    return wnd_title;
}

RECT GetActiveMonitorSize()
{
    auto hwnd = GetForegroundWindow(); // get handle of currently active window
    auto hwMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfoA(hwMonitor, &monitorInfo);

    std::cout << monitorInfo.szDevice << " [" << monitorInfo.rcWork.top << ", " << monitorInfo.rcWork.bottom;
    std::cout << ", " << monitorInfo.rcWork.left << ", " << monitorInfo.rcWork.right << "]\n";

    return monitorInfo.rcWork;
}

void setWindowPos()
{
    auto hwnd = GetForegroundWindow(); // get handle of currently active window
    auto hwMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    auto success = GetMonitorInfoA(hwMonitor, &monitorInfo);
    if (success == 0)
    {
        return;
    }

    auto width = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
    auto height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

    ShowWindow(hwnd, SW_RESTORE);
    SetWindowPos(hwnd, HWND_TOP, monitorInfo.rcWork.left, monitorInfo.rcWork.top, width / 2, height, SWP_ASYNCWINDOWPOS);
}

void setWindowPlacement()
{
    auto hwnd = GetForegroundWindow(); // get handle of currently active window
    auto hwMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfoA(hwMonitor, &monitorInfo);

    auto width = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
    auto height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

    WINDOWPLACEMENT windowPlacement;
    windowPlacement.length = sizeof(WINDOWPLACEMENT);
    windowPlacement.flags = WPF_ASYNCWINDOWPLACEMENT;
    windowPlacement.showCmd = SW_RESTORE;
    windowPlacement.rcNormalPosition = { monitorInfo.rcWork.left, monitorInfo.rcWork.top };

    SetWindowPlacement(hwnd, &windowPlacement);
}

//=====================================================================================================================

int main()
{
#ifdef DEBUG
    std::cout << "Starting Window-Manager\n";
#endif

    const auto config = wm::config::Config("C:/Github/window-manager/config/config.yaml");

#ifdef DEBUG
    for (const auto& hotkey : config)
    {
        std::cout << hotkey;
    }
#endif

    while (true)
    {
        //GetActiveWindowTitle();
        //GetActiveMonitorSize();
        //setWindowPos();
        //setWindowPlacement();

        const auto iter = config.getHotKey(wm::keys::get_keys_pressed());
        if (iter != config.end())
        {
            const auto& hotKey = *iter;
            std::cout << hotKey.name << '\n';
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return EXIT_SUCCESS;
}
