/*
 * @author BusyBox
 * @date 2024/4/26
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include <iostream>
#include "rdk/core/transfer/serial_port.h"
#include "rdk/core/imu/wit_private_protocol.h"

int main()
{
    auto serial = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.3:1.0-port0", 230400);
    WitPrivateProcotol wit = WitPrivateProcotol(serial);

    while (true) {
        std::cout << "yaw: " << wit.get_angle_info().yaw << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}