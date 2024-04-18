/*
 * @author BusyBox
 * @date 2024/4/18
 * @version 1.0
 */

#include <iostream>
#include <cstring>
#include <memory>
#include "rdk/core.hpp"

int main()
{
    auto serial = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.4:1.0-port0", 115200);
    std::cout << "打开成功" << std::endl;
    while(true) {
        char data[] = "Hello World\n";
        serial->write(data, strlen(data));
    }
    return 0;
}