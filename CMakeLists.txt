cmake_minimum_required(VERSION 3.10)

option(BUILD_ARM64 "build arm64" OFF)
option(BUILD_ARM32 "build arm32" OFF)
option(BUILD_TEST "build test program" ON)

if (BUILD_ARM64)
    set(CMAKE_TOOLCHAIN_FILE "cmake/aarch64-gnu.toolchain.cmake")
elseif (BUILD_ARM32)
    set(CMAKE_TOOLCHAIN_FILE "cmake/arm-gnueabi.toolchain.cmake")
else()

endif()

set(CMAKE_CXX_STANDARD 20)

project(RobotDevKit VERSION 1.0)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(modules/core)

set(CONFIG_FILE_PATH lib/cmake/${PROJECT_NAME})
install(FILES cmake/${PROJECT_NAME}Config.cmake DESTINATION ${CONFIG_FILE_PATH})
