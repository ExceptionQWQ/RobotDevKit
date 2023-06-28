cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(RDK)


add_subdirectory(core)


install(TARGETS RDK_Core DESTINATION ${PROJECT_SOURCE_DIR}/lib)
install(FILES core/inc/rdk_core.h DESTINATION ${PROJECT_SOURCE_DIR}/include)
install(FILES core/inc/pid.h DESTINATION ${PROJECT_SOURCE_DIR}/include)
install(FILES core/inc/motor.h DESTINATION ${PROJECT_SOURCE_DIR}/include)
install(FILES core/inc/motion.h DESTINATION ${PROJECT_SOURCE_DIR}/include)