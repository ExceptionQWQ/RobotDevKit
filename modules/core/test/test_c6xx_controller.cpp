/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
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

    while (true) {
        std::cout << c6xx_controller->read_angle(1) << std::endl;

        c6xx_controller->write_current(1, 15000);
        c6xx_controller->tick();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    return 0;
}