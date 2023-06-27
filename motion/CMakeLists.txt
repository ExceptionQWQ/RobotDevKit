cmake_minimum_required(VERSION 3.1)

project(motion VERSION 1.0)

include_directories(inc)

add_library(motion STATIC src/motion.cpp inc/motion.h)
