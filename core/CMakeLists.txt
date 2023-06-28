cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(RDK_Core VERSION 1.0)

include_directories(inc)

add_library(pid STATIC src/pid.cpp)

add_library(motion STATIC src/motion.cpp)

