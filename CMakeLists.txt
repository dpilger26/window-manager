cmake_minimum_required(VERSION 3.14)

project("window-manager"
    VERSION 1.0
    DESCRIPTION "A simple window manager with hotkeys for Windows 10"
    HOMEPAGE_URL "https://github.com/dpilger26/window-manager"
    LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

add_subdirectory(src)
