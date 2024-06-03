/*
 * @author BusyBox
 * @date 2024/5/3
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/visualization/point_cloud_view.h"

PointCloudView2D::PointCloudView2D(cv::Size window_size)
{
    this->window_size = window_size;
}

PointCloudView2D::~PointCloudView2D()
{

}

/*
 * @brief 显示点云
 * @param points 点云
 * @param title 窗口标题
 * @param delay 延迟多久窗口消失，单位毫秒，0为不消失
 */
void PointCloudView2D::show_point_cloud(const PointCloud2D& points, const std::string& title, int delay)
{
    cv::Mat image = get_point_cloud_image(points);
    cv::imshow(title, image);
    cv::waitKey(delay);
}

/*
 * @brief 获取点云图像
 * @param points 点云
 * @return cv::Mat类型的图像
 */
cv::Mat PointCloudView2D::get_point_cloud_image(const PointCloud2D& points)
{
    cv::Point2f center;
    float radius;
    cv::minEnclosingCircle(points, center, radius);

    double max_image_width = 2 * radius;
    double min_window_width = std::min(window_size.width, window_size.height);
    double scale = min_window_width / max_image_width;
    double cx = center.x, cy = center.y;

    cv::Mat image = cv::Mat(window_size, CV_8UC1, cv::Scalar(0));
    for (const auto point : points) {
        int tx = window_size.width / 2 + (point.x - cx) * scale;
        int ty = window_size.height / 2 - (point.y - cy) * scale;
        tx = std::clamp(tx, 0, window_size.width - 1);
        ty = std::clamp(ty, 0, window_size.height - 1);
        cv::circle(image, cv::Point(tx, ty), 3, cv::Scalar(255), cv::FILLED);
    }
    return image;
}