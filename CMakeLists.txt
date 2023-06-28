cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(RDK)


add_subdirectory(core)


install(TARGETS pid DESTINATION ${PROJECT_SOURCE_DIR}/lib)
install(FILES core/inc/pid.h DESTINATION ${PROJECT_SOURCE_DIR}/include)

install(TARGETS motion DESTINATION ${PROJECT_SOURCE_DIR}/lib)
install(FILES core/inc/motion.h DESTINATION ${PROJECT_SOURCE_DIR}/include)