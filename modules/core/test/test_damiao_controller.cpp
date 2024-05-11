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
    auto damiao_controller = std::make_shared<DamiaoController>(can, DamiaoController::Mode::PosSpeed);

    damiao_controller->enable(1);
    while (true) {
        damiao_controller->write_pos_speed(1, 2 * 3.14, 9.14);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        damiao_controller->write_pos_speed(1, 0, 9.14);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}