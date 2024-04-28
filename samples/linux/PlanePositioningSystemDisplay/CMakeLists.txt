cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE "cmake/aarch64-gnu.toolchain.cmake")
set(CMAKE_CXX_STANDARD 20)

project(PlanePositioningSystemDisplay VERSION 1.0)

find_package(RobotDevKit REQUIRED)
include_directories(${RobotDevKit_INCLUDE_DIRS})

add_executable(ops src/main.cpp)
target_link_libraries(ops ${RobotDevKit_LIBRARIES})

