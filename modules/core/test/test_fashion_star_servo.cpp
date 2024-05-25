/*
 * @author BusyBox
 * @date 2024/5/25
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include <iostream>
#include <cstring>
#include <memory>
#include "rdk/core.h"



void ping_all_servo(std::shared_ptr<FashionStarProtocol> star)
{
    std::cout << "start scan" << std::endl;
    for (int i = 0; i < 256; ++i) {
        if (star->ping(i, 10)) {
            std::cout << "id: " << i << " online" << std::endl;
        }
    }
    std::cout << "start end" << std::endl;
}

void read_servo_info(std::shared_ptr<FashionStarProtocol> star, uint8_t id)
{
    uint16_t voltage = 0;
    star->read_voltage(id, &voltage);
    std::cout << "voltage: " << (int)voltage << std::endl;

    uint16_t current = 0;
    star->read_current(id, &current);
    std::cout << "current: " << (int)current << std::endl;

    uint16_t power = 0;
    star->read_power(id, &power);
    std::cout << "power: " << (int)power << std::endl;

    uint16_t temp = 0;
    star->read_temp(id, &temp);
    std::cout << "temp: " << (int)temp << std::endl;

    uint8_t status = 0;
    star->read_servo_status(id, &status);
    std::cout << "status: " << (int)status << std::endl;

    uint16_t type = 0;
    star->read_servo_type(id, &type);
    std::cout << "type: " << (int)type << std::endl;

    uint16_t version = 0;
    star->read_firmware_version(id, &version);
    std::cout << "version: " << (int)version << std::endl;

    uint32_t number = 0;
    star->read_serial_number(id, &number);
    std::cout << "number: " << (int)number << std::endl;
}

int main()
{
    std::shared_ptr<SerialPort> serial_port = std::make_shared<SerialPort>("/dev/serial/by-path/platform-3610000.xhci-usb-0:2.4:1.0-port0", 115200);
    std::shared_ptr<FashionStarProtocol> star = std::make_shared<FashionStarProtocol>(serial_port);
    // read_servo_info(star, 0);
    // ping_all_servo(star);
    for (int i = 0; i < 10; ++i) {
        star->move_on_angle_mode_ex_by_interval(0, -900, 1000, 300, 300, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        star->move_on_angle_mode_ex_by_interval(0, 900, 1000, 300, 300, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        int16_t angle;
        star->read_angle(0, &angle);
        std::cout << "angle: " << angle << std::endl;
    }
    
    
    return 0;
}