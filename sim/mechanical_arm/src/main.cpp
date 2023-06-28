#include "main.h"


ArmList SolveArm2D(double len1, double len2, double xPos, double yPos)
{
    if (fabs(xPos) < 0.001) xPos = 0.001;
    double len = std::sqrt(std::pow(xPos, 2) + std::pow(yPos, 2));
    if (len > len1 + len2) return ArmList{}; //无解
    double r4 = std::acos((std::pow(len1, 2) + std::pow(len2, 2) - std::pow(len, 2)) / (2 * len1 * len2));
    double r5 = std::atan(yPos / xPos);
    if (xPos < 0 && yPos > 0) {
        r5 += PI;
    }
    std::cout << r4 << std::endl;
    double r2 = r4 - PI;
    double r3 = std::asin(len2 * std::sin(r4) / len);
    double r1 = r3 + r5;
    return ArmList{{len1, r1}, {len2, r2}};
}

ArmList SolveArm3DH(double len1, double len2, double len3, double xPos, double yPos)
{
    if (fabs(xPos) < 0.001) xPos = 0.001;
    double len = std::sqrt(std::pow(xPos, 2) + std::pow(yPos, 2));
    if (len > len1 + len2) return ArmList{}; //无解
    double r4 = std::acos((std::pow(len1, 2) + std::pow(len2, 2) - std::pow(len, 2)) / (2 * len1 * len2));
    double r5 = std::atan(yPos / xPos);
    if (xPos < 0 && yPos > 0) {
        r5 += PI;
    }
    std::cout << r4 << std::endl;
    double r2 = r4 - PI;
    double r3 = std::asin(len2 * std::sin(r4) / len);
    double r1 = r3 + r5;
    return ArmList{{len1, r1}, {len2, r2}, {len3, -(r1 + r2)}};
}


cv::Mat DrawArmList(const ArmList& armList, cv::Size viewSz)
{
    cv::Mat view = cv::Mat::zeros(viewSz, CV_8UC3);
    cv::Point start = cv::Point(viewSz.width / 2, viewSz.height / 2);
    std::vector<cv::Point> points = {start};
    double radian = 0;
    for (size_t i = 0; i != armList.size(); ++i) {
        radian += armList[i].radian;
        radian = std::fmod(radian + 2 * PI, 2 * PI);
        cv::Point end = cv::Point(start.x + armList[i].len * std::cos(radian), start.y + armList[i].len * std::sin(radian));
        cv::line(view, start, end, cv::Scalar(0, 255, 0), 3);
        points.push_back(end);
        start = end;
    }
    for (size_t i = 0; i != points.size(); ++i) {
        cv::circle(view, points[i], 3, cv::Scalar(0, 0, 255), -1);
    }

    cv::flip(view, view, 0);
    return view;
}


int main()
{
    double len1 = 100, len2 = 100;
    double xPos = 0, yPos = 0;
    for (int i = 0; i < 130; ++i) {
        xPos += 1;
        ArmList armList = SolveArm3DH(len1, len2, 50, xPos, yPos);
        cv::Mat view = DrawArmList(armList, cv::Size(512, 512));
        cv::imshow("view", view);
        cv::waitKey(10);
    }
    for (int i = 0; i < 130; ++i) {
        yPos += 1;
        ArmList armList = SolveArm3DH(len1, len2, 50, xPos, yPos);
        cv::Mat view = DrawArmList(armList, cv::Size(512, 512));
        cv::imshow("view", view);
        cv::waitKey(10);
    }
    for (int i = 0; i < 260; ++i) {
        xPos -= 1;
        ArmList armList = SolveArm3DH(len1, len2, 50, xPos, yPos);
        cv::Mat view = DrawArmList(armList, cv::Size(512, 512));
        cv::imshow("view", view);
        cv::waitKey(10);
    }
    for (int i = 0; i < 260; ++i) {
        xPos += 1;
        ArmList armList = SolveArm3DH(len1, len2, 50, xPos, yPos);
        cv::Mat view = DrawArmList(armList, cv::Size(512, 512));
        cv::imshow("view", view);
        cv::waitKey(10);
    }
    for (int i = 0; i < 260; ++i) {
        yPos -= 1;
        ArmList armList = SolveArm3DH(len1, len2, 50, xPos, yPos);
        
        cv::Mat view = DrawArmList(armList, cv::Size(512, 512));
        cv::imshow("view", view);
        cv::waitKey(10);
    }
    
    cv::waitKey(0);
    return 0;
}