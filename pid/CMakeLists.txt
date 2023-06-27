cmake_minimum_required(VERSION 3.1)

project(pid)

include_directories(inc)

add_library(pid STATIC src/pid.cpp inc/pid.h)
