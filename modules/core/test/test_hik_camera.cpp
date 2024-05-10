/*
 * @author BusyBox
 * @date 2024/4/30
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat frame;
    cv::VideoCapture vc(0);
    if (!vc.isOpened()) {
        std::cout << "cannot open hik camera" << std::endl;
        return -1;
    }
    while (true) {
        vc >> frame;
        cv::imshow("frame", frame);
        cv::waitKey(1);
    }

    return 0;
}