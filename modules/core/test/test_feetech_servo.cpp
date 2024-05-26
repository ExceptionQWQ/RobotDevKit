/*
 * @author BusyBox
 * @date 2024/5/26
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include <iostream>
#include <cstring>
#include <memory>
#include "rdk/core.h"
#include <boost/endian.hpp>

void ping_all_servo(std::shared_ptr<FeetechProtocol> fee)
{
    std::cout << "ping start" << std::endl;
    for (int i = 0; i < 256; ++i) {
        if (fee->ping(i, 10)) std::cout << "id: " << i << " online" << std::endl;
    }

    std::cout << "ping end" << std::endl;
}

int main()
{
    std::shared_ptr<SerialPort> serial_port = std::make_shared<SerialPort>("/dev/serial/by-path/platform-3610000.xhci-usb-0:2.4:1.0-port0", 115200);
    std::shared_ptr<FeetechProtocol> fee = std::make_shared<FeetechProtocol>(serial_port);
    // ping_all_servo(fee);
    std::cout << fee->ping(1, 10) << std::endl;
    for (int i = 0; i < 6; ++i) {
        uint8_t data[6];
        *reinterpret_cast<uint16_t*>(data + 0) = boost::endian::endian_reverse((uint16_t)100);
        *reinterpret_cast<uint16_t*>(data + 2) = boost::endian::endian_reverse((uint16_t)0);
        *reinterpret_cast<uint16_t*>(data + 4) = boost::endian::endian_reverse((uint16_t)3000);
        std::cout << fee->reg_write(1, 0x2a, data, 6) << std::endl;
        fee->action();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        *reinterpret_cast<uint16_t*>(data + 0) = boost::endian::endian_reverse((uint16_t)4000);
        *reinterpret_cast<uint16_t*>(data + 2) = boost::endian::endian_reverse((uint16_t)0);
        *reinterpret_cast<uint16_t*>(data + 4) = boost::endian::endian_reverse((uint16_t)3000);
        std::cout << fee->reg_write(1, 0x2a, data, 6) << std::endl;
        fee->action();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    return 0;
}