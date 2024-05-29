/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include <iostream>
#include <cstring>
#include <memory>
#include "rdk/core.h"

int main()
{
    KeyValuePair pair1;
    pair1.put("name", "robot1");
    pair1.put("x_speed", "100.1");
    pair1.put("y_speed", "-10.3");
    pair1.put("z_speed", "30.0");
    auto pair1_list = pair1.get_pair_list();
    std::cout << pair1_list << std::endl;

    KeyValuePair pair2(pair1_list);
    pair2.put("roll", "3.14");
    pair2.put("pitch", "1.56");
    pair2.put("yaw", "1.08");
    std::cout << pair2.get_pair_list() << std::endl;

    std::cout << "name=" << pair2.get("name") << std::endl;
    std::cout << "yaw=" << pair2.get("yaw") << std::endl; 


    return 0;
}