#pragma once

#include "Config.hpp"

#include <Windows.h>

#include <cmath>
#ifdef DEBUG
#include <iostream>
#endif

//=====================================================================================================================

namespace wm
{
    //std::string GetActiveWindowTitle()
    //{
    //    char wnd_title[255];
    //    auto hwnd = GetForegroundWindow(); // get handle of currently active window
    //    GetWindowText(hwnd, wnd_title, sizeof(wnd_title));
    //    std::cout << wnd_title << '\n';
    //    return wnd_title;
    //}

    //RECT GetActiveMonitorSize()
    //{
    //    auto hwnd = GetForegroundWindow(); // get handle of currently active window
    //    auto hwMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    //    MONITORINFOEX monitorInfo;
    //    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    //    GetMonitorInfoA(hwMonitor, &monitorInfo);

    //    std::cout << monitorInfo.szDevice << " [" << monitorInfo.rcWork.top << ", " << monitorInfo.rcWork.bottom;
    //    std::cout << ", " << monitorInfo.rcWork.left << ", " << monitorInfo.rcWork.right << "]\n";

    //    return monitorInfo.rcWork;
    //}

    //void setWindowPos()
    //{
    //    auto hwnd = GetForegroundWindow(); // get handle of currently active window
    //    auto hwMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    //    MONITORINFOEX monitorInfo;
    //    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    //    auto success = GetMonitorInfoA(hwMonitor, &monitorInfo);
    //    if (success == 0)
    //    {
    //        return;
    //    }

    //    auto width = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
    //    auto height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

    //    ShowWindow(hwnd, SW_RESTORE);
    //    SetWindowPos(hwnd, HWND_TOP, monitorInfo.rcWork.left, monitorInfo.rcWork.top, width / 2, height, SWP_ASYNCWINDOWPOS);
    //}

    //void setWindowPlacement()
    //{
    //    auto hwnd = GetForegroundWindow(); // get handle of currently active window
    //    auto hwMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    //    MONITORINFOEX monitorInfo;
    //    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    //    GetMonitorInfoA(hwMonitor, &monitorInfo);

    //    auto width = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
    //    auto height = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

    //    WINDOWPLACEMENT windowPlacement;
    //    windowPlacement.length = sizeof(WINDOWPLACEMENT);
    //    windowPlacement.flags = WPF_ASYNCWINDOWPLACEMENT;
    //    windowPlacement.showCmd = SW_RESTORE;
    //    windowPlacement.rcNormalPosition = { monitorInfo.rcWork.left, monitorInfo.rcWork.top };

    //    SetWindowPlacement(hwnd, &windowPlacement);
    //}

    class WindowController
    {
    public:
        WindowController() = default;

        //=============================================================================================================

        void doIt(const config::Action& action) const noexcept
        {
            auto info = Info();
            if (!info.window.success || !info.monitor.success)
            {
                return;
            }

            const auto width = calcWindowWidth(info, action.size.horizontal);
            const auto height = calcWindowHeight(info, action.size.vertical);
            const auto leftPos = calcWindowLeftPosition(info, action.location.horizontal, width);
            const auto topPos = calcWindowTopPosition(info, action.location.vertical, height);

#ifdef DEBUG
            std::cout << "Setting window position [" << leftPos << ", " << topPos;
            std::cout << ", " << width << ", " << height << "]\n";
#endif

            ShowWindow(info.window.handle, SW_RESTORE);
            SetWindowPos(info.window.handle, HWND_TOP, leftPos, topPos, width, height, SWP_ASYNCWINDOWPOS);
        }

        //=============================================================================================================

    private:
        //=============================================================================================================

        class WindowInfo
        {
        public:
            WindowInfo() noexcept
            {
                handle = GetForegroundWindow(); 
                info.cbSize = sizeof(decltype(info));
                success = GetWindowInfo(handle, &info) != 0;

#ifdef DEBUG
                if (success)
                {
                    char windowTitle[255];
                    auto titleSuccess = GetWindowText(handle, windowTitle, sizeof(windowTitle));
                    if (titleSuccess != 0)
                    {
                        std::cout << windowTitle << ' ';
                    }
                    std::cout << "[" << info.rcWindow.top << ", " << info.rcWindow.bottom;
                    std::cout << ", " << info.rcWindow.left << ", " << info.rcWindow.right << "]\n";
#endif
                }
            }

            HWND        handle{};
            WINDOWINFO  info{};
            bool        success{ false };
        };

        class MonitorInfo
        {
        public:
            MonitorInfo(HWND hwnd)
            {
                handle = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                info.cbSize = sizeof(decltype(info));
                success = GetMonitorInfoA(handle, &info) != 0;
#ifdef DEBUG
                if (success)
                {
                    std::cout << info.szDevice << " [" << info.rcWork.top << ", " << info.rcWork.bottom;
                    std::cout << ", " << info.rcWork.left << ", " << info.rcWork.right << "]\n";
                }
#endif
            }

            HMONITOR        handle{};
            MONITORINFOEX   info{};
            bool            success{ false };
        };

        //=============================================================================================================

        class Info
        {
        public:
            Info() :
                window(),
                monitor(window.handle)
            {}

            WindowInfo  window;
            MonitorInfo monitor;
        };

        //=============================================================================================================

        int calcWindowHeight(const Info& info, int verticalSize) const noexcept
        {
            if (verticalSize < 0)
            {
                return std::abs(info.window.info.rcWindow.bottom - info.window.info.rcWindow.top);
            }

            auto monitorHeight = std::abs(info.monitor.info.rcWork.bottom - info.monitor.info.rcWork.top);
            return static_cast<int>(std::floor(static_cast<double>(monitorHeight * verticalSize) / 100.));
        }

        int calcWindowWidth(const Info& info, int horizontalSize) const noexcept
        {
            if (horizontalSize < 0)
            {
                return std::abs(info.window.info.rcWindow.right - info.window.info.rcWindow.left);
            }

            auto monitorWidth = std::abs(info.monitor.info.rcWork.right - info.monitor.info.rcWork.left);
            return static_cast<int>(std::floor(static_cast<double>(monitorWidth * horizontalSize) / 100.));
        }

        int calcWindowLeftPosition(const Info& info, const config::Horizontal& horizontalPos, int windowWidth) const noexcept
        {
            int leftPos = 0;
            switch (horizontalPos)
            {
                case config::Horizontal::Left:
                {
                    leftPos = info.monitor.info.rcWork.left;
                    break;
                }
                case config::Horizontal::Center:
                {
                    auto windowHalfWidth = windowWidth / 2; // integer division ok
                    auto monitorHalfWidth = std::abs(info.monitor.info.rcWork.right - info.monitor.info.rcMonitor.left) / 2; // integer division ok
                    leftPos = monitorHalfWidth - windowHalfWidth;
                    break;
                }
                case config::Horizontal::Right:
                {
                    leftPos = info.monitor.info.rcWork.right - windowWidth;
                    break;
                }
                case config::Horizontal::None:
                {
                    leftPos = info.window.info.rcWindow.left;
                    break;
                }
            }

            return leftPos;
        }

        int calcWindowTopPosition(const Info& info, const config::Vertical& verticalPos, int windowHeight) const noexcept
        {
            int topPos = 0;
            switch (verticalPos)
            {
                case config::Vertical::Bottom:
                {
                    topPos = info.monitor.info.rcWork.bottom - windowHeight;
                    break;
                }
                case config::Vertical::Center:
                {
                    auto windowHalfHeight = windowHeight / 2; // integer division ok
                    auto monitorHalfHeight = std::abs(info.monitor.info.rcWork.bottom - info.monitor.info.rcMonitor.top) / 2; // integer division ok
                    topPos = monitorHalfHeight - windowHalfHeight;
                    break;
                }
                case config::Vertical::Top:
                {
                    topPos = info.monitor.info.rcWork.top;
                    break;
                }
                case config::Vertical::None:
                {
                    topPos = info.window.info.rcWindow.top;
                    break;
                }
            }

            return topPos;
        }
    };
}
