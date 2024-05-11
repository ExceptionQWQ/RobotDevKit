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
    auto serial = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.1:1.0-port0", 921600);
    auto can = std::make_shared<SerialToCan>(serial, CanBus::IDType::STD);
    auto c6xx_controller = std::make_shared<C6xxController>(can);
    auto m3508_motor = std::make_shared<M3508Motor>(c6xx_controller, 1, DjiMotor::Mode::SPEED_POS);

    m3508_motor->set_pos_pid(0.06, 0, 0.06);

    double angle = 0;
    int cnt = 0;
    while (true) {
        ++cnt;
    
        if (cnt % 800 == 0) {
            angle += 90;
        }
        m3508_motor->set_target_pos(angle);
        
        m3508_motor->tick();
        c6xx_controller->tick();
        
        std::cout << m3508_motor->get_pos() << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}