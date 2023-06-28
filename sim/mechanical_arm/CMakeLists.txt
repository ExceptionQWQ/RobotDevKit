cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(mechanical_arm_sim)

find_package(OpenCV REQUIRED)

include_directories(inc)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(mechanical_arm_sim src/main.cpp)
target_link_libraries(mechanical_arm_sim 
    ${OpenCV_LIBS}    
)