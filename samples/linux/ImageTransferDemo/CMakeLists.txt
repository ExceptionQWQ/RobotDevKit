cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

project(ImageTransferDemo VERSION 1.0)

find_package(RobotDevKit REQUIRED)
include_directories(${RobotDevKit_INCLUDE_DIRS})

add_executable(server src/server.cpp)
target_link_libraries(server ${RobotDevKit_LIBRARIES})

add_executable(client src/client.cpp)
target_link_libraries(client ${RobotDevKit_LIBRARIES})
