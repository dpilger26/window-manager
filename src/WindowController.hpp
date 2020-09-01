#pragma once

#include "Config.hpp"

#include <Windows.h>

#include <algorithm>
#include <cmath>
#ifdef DEBUG
#include <iostream>
#endif

//=====================================================================================================================

namespace wm
{
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

                if (success)
                {
                    borderTop = std::abs(info.rcWindow.top - info.rcClient.top);
                    borderBottom = std::abs(info.rcWindow.bottom - info.rcClient.bottom);
                    borderLeft = std::abs(info.rcWindow.left - info.rcClient.left);
                    borderRight = std::abs(info.rcWindow.right - info.rcClient.right);
#ifdef DEBUG
                    char windowTitle[255];
                    auto titleSuccess = GetWindowText(handle, windowTitle, sizeof(windowTitle));
                    if (titleSuccess != 0)
                    {
                        std::cout << "window title: " << windowTitle << '\n';
                    }
                    std::cout << "\twindow: [" << info.rcWindow.top << ", " << info.rcWindow.bottom;
                    std::cout << ", " << info.rcWindow.left << ", " << info.rcWindow.right << "]\n";
                    std::cout << "\tclient: [" << info.rcClient.top << ", " << info.rcClient.bottom;
                    std::cout << ", " << info.rcClient.left << ", " << info.rcClient.right << "]\n";
                    std::cout << "\tborder: [" << borderTop << ", " << borderBottom;
                    std::cout << ", " << borderLeft << ", " << borderRight << "]\n";
#endif
                }
            }

            HWND        handle{};
            WINDOWINFO  info{};
            int         borderTop{ 0 };
            int         borderBottom{ 0 };
            int         borderLeft{ 0 };
            int         borderRight{ 0 };
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

            const auto monitorHeight = std::abs(info.monitor.info.rcWork.bottom - info.monitor.info.rcWork.top);
            auto windowHeight = static_cast<int>(std::floor(static_cast<double>(monitorHeight * verticalSize) / 100.));
            windowHeight += info.window.borderTop;
            windowHeight += info.window.borderBottom;

            return windowHeight;
        }

        int calcWindowWidth(const Info& info, int horizontalSize) const noexcept
        {
            if (horizontalSize < 0)
            {
                return std::abs(info.window.info.rcWindow.right - info.window.info.rcWindow.left);
            }

            const auto monitorWidth = std::abs(info.monitor.info.rcWork.right - info.monitor.info.rcWork.left);
            auto windowWidth = static_cast<int>(std::floor(static_cast<double>(monitorWidth * horizontalSize) / 100.));
            windowWidth += info.window.borderLeft;
            windowWidth += info.window.borderRight;

            return windowWidth;
        }

        int calcWindowLeftPosition(const Info& info, const config::Horizontal& horizontalPos, int windowWidth) const noexcept
        {
            int leftPos = 0;
            switch (horizontalPos)
            {
                case config::Horizontal::Left:
                {
                    leftPos = info.monitor.info.rcWork.left - info.window.borderLeft;
                    break;
                }
                case config::Horizontal::Center:
                {
                    auto windowHalfWidth = windowWidth / 2; // integer division ok
                    auto monitorHalfWidth = std::abs(info.monitor.info.rcWork.right - info.monitor.info.rcMonitor.left) / 2; // integer division ok
                    leftPos = monitorHalfWidth - windowHalfWidth - info.window.borderLeft;
                    break;
                }
                case config::Horizontal::Right:
                {
                    leftPos = info.monitor.info.rcWork.right - windowWidth + info.window.borderRight;
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
                    topPos = info.monitor.info.rcWork.bottom - windowHeight + info.window.borderBottom;
                    break;
                }
                case config::Vertical::Center:
                {
                    auto windowHalfHeight = windowHeight / 2; // integer division ok
                    auto monitorHalfHeight = std::abs(info.monitor.info.rcWork.bottom - info.monitor.info.rcMonitor.top) / 2; // integer division ok
                    topPos = std::max(monitorHalfHeight - windowHalfHeight - info.window.borderTop, info.monitor.info.rcMonitor.top); // max keeps from snapping
                    break;
                }
                case config::Vertical::Top:
                {
                    topPos = info.monitor.info.rcWork.top - info.window.borderTop;
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
