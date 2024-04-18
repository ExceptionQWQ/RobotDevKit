cmake_minimum_required(VERSION 3.10)

add_library(rdk_core src/serial_port/serial_port.cpp)
target_include_directories(rdk_core PRIVATE include)


add_executable(test_serial_port test/test_serial_port.cpp)
target_include_directories(test_serial_port PRIVATE include)
target_link_libraries(test_serial_port rdk_core)