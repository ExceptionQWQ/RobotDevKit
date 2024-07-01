/*
 * @author BusyBox
 * @date 2024/7/1
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include <iostream>
#include <cstring>
#include <memory>
#include "rdk/core.h"
#include <boost/endian.hpp>

void id_scan(std::shared_ptr<HuanerProtocol> servo)
{
    for (int id = 0; id < 255; ++id) {
        uint8_t id2;
        if (servo->id_read(id, &id2)) {
            printf("id:%d on\n", id2);
        }
    }
}

int main()
{
    std::shared_ptr<SerialPort> serial_port = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.4:1.0-port0", 115200);
    std::shared_ptr<HuanerProtocol> servo = std::make_shared<HuanerProtocol>(serial_port);
    
    // servo->move_time_write(3, 500, 1000);

    while (true) {
        int16_t pos;
        if (servo->pos_read(3, &pos)) {
            printf("pos:%d\n", pos);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}