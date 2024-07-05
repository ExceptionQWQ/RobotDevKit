cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SOURCES "src/*.*")

add_library(rdk_core ${SOURCES})
target_include_directories(rdk_core PRIVATE include)

if(BUILD_TEST)
    add_executable(test_serial_port test/test_serial_port.cpp)
    target_include_directories(test_serial_port PRIVATE include)
    target_link_libraries(test_serial_port rdk_core)

    add_executable(test_simple_message_transfer test/test_simple_message_transfer.cpp)
    target_include_directories(test_simple_message_transfer PRIVATE include)
    target_link_libraries(test_simple_message_transfer rdk_core)

    add_executable(test_simple_message_transfer_with_stm32 test/test_simple_message_transfer_with_stm32.cpp)
    target_include_directories(test_simple_message_transfer_with_stm32 PRIVATE include)
    target_link_libraries(test_simple_message_transfer_with_stm32 rdk_core)

    add_executable(test_simple_binary_transfer test/test_simple_binary_transfer.cpp)
    target_include_directories(test_simple_binary_transfer PRIVATE include)
    target_link_libraries(test_simple_binary_transfer rdk_core)

    add_executable(test_simple_binary_transfer_with_stm32 test/test_simple_binary_transfer_with_stm32.cpp)
    target_include_directories(test_simple_binary_transfer_with_stm32 PRIVATE include)
    target_link_libraries(test_simple_binary_transfer_with_stm32 rdk_core)

    add_executable(test_reliable_binary_transfer test/test_reliable_binary_transfer.cpp)
    target_include_directories(test_reliable_binary_transfer PRIVATE include)
    target_link_libraries(test_reliable_binary_transfer rdk_core)

    add_executable(test_reliable_binary_transfer_with_stm32 test/test_reliable_binary_transfer_with_stm32.cpp)
    target_include_directories(test_reliable_binary_transfer_with_stm32 PRIVATE include)
    target_link_libraries(test_reliable_binary_transfer_with_stm32 rdk_core)

    add_executable(test_jy901b_imu test/test_jy901b_imu.cpp)
    target_include_directories(test_jy901b_imu PRIVATE include)
    target_link_libraries(test_jy901b_imu rdk_core)

    add_executable(test_serial_to_can test/test_serial_to_can.cpp)
    target_include_directories(test_serial_to_can PRIVATE include)
    target_link_libraries(test_serial_to_can rdk_core)

    add_executable(test_c6xx_controller test/test_c6xx_controller.cpp)
    target_include_directories(test_c6xx_controller PRIVATE include)
    target_link_libraries(test_c6xx_controller rdk_core)

    add_executable(test_dji_motor test/test_dji_motor.cpp)
    target_include_directories(test_dji_motor PRIVATE include)
    target_link_libraries(test_dji_motor rdk_core)

    add_executable(test_m3508_motor test/test_m3508_motor.cpp)
    target_include_directories(test_m3508_motor PRIVATE include)
    target_link_libraries(test_m3508_motor rdk_core)

    add_executable(test_damiao_controller test/test_damiao_controller.cpp)
    target_include_directories(test_damiao_controller PRIVATE include)
    target_link_libraries(test_damiao_controller rdk_core)

    add_executable(test_hik_camera test/test_hik_camera.cpp)
    target_include_directories(test_hik_camera PRIVATE include)
    target_link_libraries(test_hik_camera rdk_core ${OpenCV_LIBRARIES})

    add_executable(test_fashion_star_servo test/test_fashion_star_servo.cpp)
    target_include_directories(test_fashion_star_servo PRIVATE include)
    target_link_libraries(test_fashion_star_servo rdk_core)

    add_executable(test_feetech_servo test/test_feetech_servo.cpp)
    target_include_directories(test_feetech_servo PRIVATE include)
    target_link_libraries(test_feetech_servo rdk_core)

    add_executable(test_key_value_pair test/test_key_value_pair.cpp)
    target_include_directories(test_key_value_pair PRIVATE include)
    target_link_libraries(test_key_value_pair rdk_core)

    add_executable(test_ld_lidar test/test_ld_lidar.cpp)
    target_include_directories(test_ld_lidar PRIVATE include)
    target_link_libraries(test_ld_lidar rdk_core ${OpenCV_LIBRARIES})
    
    add_executable(test_point_cloud_view test/test_point_cloud_view.cpp)
    target_include_directories(test_point_cloud_view PRIVATE include)
    target_link_libraries(test_point_cloud_view rdk_core ${OpenCV_LIBRARIES})

    add_executable(test_huaner_servo test/test_huaner_servo.cpp)
    target_include_directories(test_huaner_servo PRIVATE include)
    target_link_libraries(test_huaner_servo rdk_core)

    add_executable(test_tcp_port test/test_tcp_port.cpp)
    target_include_directories(test_tcp_port PRIVATE include)
    target_link_libraries(test_tcp_port rdk_core)
endif()


install(TARGETS rdk_core ARCHIVE DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY include/rdk DESTINATION include/${PROJECT_NAME})