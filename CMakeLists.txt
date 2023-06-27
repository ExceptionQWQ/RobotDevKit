cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(RDK)


add_subdirectory(pid)
install(TARGETS pid DESTINATION ${PROJECT_SOURCE_DIR}/lib)
install(FILES pid/inc/pid.h DESTINATION ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(motion)
install(TARGETS motion DESTINATION ${PROJECT_SOURCE_DIR}/lib)
install(FILES motion/inc/motion.h DESTINATION ${PROJECT_SOURCE_DIR}/include)