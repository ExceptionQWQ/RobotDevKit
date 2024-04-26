cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE "cmake/aarch64-gnu.toolchain.cmake")
set(CMAKE_CXX_STANDARD 20)

project(RobotDevKit VERSION 1.0)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(modules/core)




set(CONFIG_FILE_PATH lib/cmake/${PROJECT_NAME})
install(FILES cmake/${PROJECT_NAME}Config.cmake DESTINATION ${CONFIG_FILE_PATH})
