/*
 * @author BusyBox
 * @date 2024/5/31
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include <iostream>
#include <cstring>
#include <memory>
#include "rdk/core.h"

int main()
{
    auto serial = std::make_shared<SerialPort>("/dev/serial/by-path/platform-3610000.xhci-usb-0:2.4:1.0-port0", 230400);
    auto lidar = std::make_shared<LDLidar>(serial);

    //通过回调函数获取激光点测量数据 这种方式获取的激光点角度的精度高达0.01度
    lidar->set_lidar_point_callback([](float angle, uint16_t distance, uint8_t intensity) {
        printf("%.2f, %d, %d\n", angle, distance, intensity);
    });

    //通过调用lidar get_lidar_point方法获取激光点测量数据 这种方式获取的激光点角度精度只有1度
    while (true) {
        for (int i = 0; i < 360; ++i) {
            auto point = lidar->get_lidar_point(i);
            printf("%d, %d, %d\n", i, point.distance, point.intensity);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}