/*
 * @author BusyBox
 * @date 2024/5/31
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include <iostream>
#include <cstring>
#include <cmath>
#include <numbers>
#include <memory>
#include "rdk/core.h"


int main()
{
    auto serial = std::make_shared<SerialPort>("/dev/tty.usbserial-0001", 230400);
    auto lidar = std::make_shared<LDLidar>(serial);

    //通过回调函数获取激光点测量数据 这种方式获取的激光点角度的精度高达0.01度
    // lidar->set_lidar_point_callback([](float angle, uint16_t distance, uint8_t intensity) {
    //     printf("%.2f, %d, %d\n", angle, distance, intensity);
    // });

    //通过调用lidar get_lidar_point方法获取激光点测量数据 这种方式获取的激光点角度精度只有1度
    PointCloudView2D pcv(cv::Size(1024, 1024));
    while (true) {
        PointCloud2D points;
        for (int i = 0; i < 360; ++i) {
            auto point = lidar->get_lidar_point(i);
            points.push_back(cv::Point2f(point.distance * std::cos(i / 180.0 * std::numbers::pi), point.distance * std::sin(i / 180.0 * std::numbers::pi)));
        }
        pcv.show_point_cloud(points, "point_cloud", 10);
    }
    return 0;
}