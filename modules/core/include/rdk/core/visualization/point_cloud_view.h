/*
 * @author BusyBox
 * @date 2024/5/3
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>
#include <opencv2/opencv.hpp>

using PointCloud2D = std::vector<cv::Point2f>;

class PointCloudView2D
{
public:
    PointCloudView2D(cv::Size window_size);
    ~PointCloudView2D();

    /*
     * @brief 显示点云
     * @param points 点云
     * @param title 窗口标题
     * @param delay 延迟多久窗口消失，单位毫秒，0为不消失
     */
    void show_point_cloud(const PointCloud2D& points, const std::string& title, int delay = 0);

    /*
     * @brief 获取点云图像
     * @param points 点云
     * @return cv::Mat类型的图像
     */
    cv::Mat get_point_cloud_image(const PointCloud2D& points);

private:
    cv::Size window_size;

};