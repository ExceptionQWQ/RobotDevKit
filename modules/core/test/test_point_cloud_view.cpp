/*
 * @author BusyBox
 * @date 2024/5/3
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include <iostream>
#include "rdk/core.h"


int main()
{
    PointCloud2D points;
    for (int i = 0; i < 100; ++i) {
        points.push_back(cv::Point2f(i, 0));
        points.push_back(cv::Point2f(i, i));
        points.push_back(cv::Point2f(0, i));
    }
    PointCloudView2D pcv(cv::Size(1024, 1024));
    pcv.show_point_cloud(points, "pcv");

    return 0;
}