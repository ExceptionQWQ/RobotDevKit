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
    auto serial = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.1:1.0-port0", 115200);
    auto can = std::make_shared<SerialToCan>(serial, CanBus::IDType::STD);
    auto c6xx_controller = std::make_shared<C6xxController>(can);
    auto dji_motor = std::make_shared<DjiMotor>(c6xx_controller, 1, DjiMotor::Mode::SPEED_POS);

    dji_motor->set_speed_pid(50, 3, 20);
    dji_motor->set_target_rpm(3000); //转速设置过大会丢步
    dji_motor->set_pos_pid(0.12, 0, 0.12);

    double angle = 0;
    int cnt = 0;
    while (true) {
        ++cnt;
    
        if (cnt % 200 == 0) {
            angle += 6000;
        }
        dji_motor->set_target_pos(angle);
        
        dji_motor->tick();
        c6xx_controller->tick();

        std::cout << dji_motor->get_pos() << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    return 0;
}