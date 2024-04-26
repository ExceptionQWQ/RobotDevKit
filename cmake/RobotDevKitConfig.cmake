include(FindPackageHandleStandardArgs)
set(RobotDevKit_FOUND TRUE)
message("RobotDevKit_FOUND " ${RobotDevKit_FOUND})

get_filename_component(CMAKE_CURRENT_LIST_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
message("CMAKE_CURRENT_LIST_DIR " ${CMAKE_CURRENT_LIST_DIR})
get_filename_component(RobotDevKit_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)
message("RobotDevKit_INSTALL_PREFIX " ${RobotDevKit_INSTALL_PREFIX})

find_library(RobotDevKit_LIBRARY rdk_core PATHS "${RobotDevKit_INSTALL_PREFIX}/lib/RobotDevKit")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(RobotDevKit_INCLUDE_DIRS ${RobotDevKit_INSTALL_PREFIX}/include/RobotDevKit)
set(RobotDevKit_LIBRARIES ${RobotDevKit_LIBRARY})