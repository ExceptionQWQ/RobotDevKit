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

    while (true) {
        uint32_t id;
        CanBus::FrameData data;
        id = 0x200;
        data.d0 = 1;
        data.d1 = 0;
        can->write_frame(id, data);
        std::cout << id << " " << (int)data.d0 << std::endl;

    }
    
    return 0;
}