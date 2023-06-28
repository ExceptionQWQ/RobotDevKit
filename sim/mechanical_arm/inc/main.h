#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>


#define PI 3.141592653589793

struct ArmInfo
{
    double len;
    double radian;
};

using ArmList = std::vector<ArmInfo>;


ArmList SolveArm2D(double len1, double len2, double xPos, double yPos);


#endif