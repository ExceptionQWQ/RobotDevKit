/*
 * @author BusyBox
 * @date 2024/4/18
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include <iostream>
#include <cstring>
#include <memory>
#include "rdk/core.h"

int main()
{
    auto serial1 = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2:1.0-port0", 115200);
    auto serial2 = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.4:1.0-port0", 115200);
    std::cout << "打开成功" << std::endl;
    while(true) {
        char data[] = "Hello World\n";
        serial1->write((uint8_t*)data, strlen(data), 1000);
        uint8_t buff[1024];
        size_t len = serial2->read(buff, 1024);
        serial2->write((uint8_t*)data, strlen(data));
        len = serial1->read(buff, 1024, 1000);
        std::cout << buff;
    }
    return 0;
}