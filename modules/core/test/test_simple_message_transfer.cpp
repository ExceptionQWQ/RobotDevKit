/*
 * @author BusyBox
 * @date 2024/4/22
 * @version 1.0
 */

#include <iostream>
#include <cstring>
#include <boost/crc.hpp>
#include <boost/asio.hpp>
#include "rdk/core.h"


int main()
{
    auto serial1 = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2:1.0-port0", 115200);
    auto serial2 = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.4:1.0-port0", 115200);
    std::cout << "打开成功" << std::endl;

    std::shared_ptr<SimpleMessageTransfer> transfer1 = std::make_shared<SimpleMessageTransfer>(serial1);
    std::shared_ptr<SimpleMessageTransfer> transfer2 = std::make_shared<SimpleMessageTransfer>(serial2);
    
    while (true) {
        std::string msg = "Hello World";
        for (int i = 0; i < 1000; ++i) {
            transfer1->send_message(msg);
            transfer2->recv_message();
            transfer2->send_message(msg);
            transfer1->recv_message();
        }
        std::cout << "crc_error_cnt:" << transfer1->get_crc_error_cnt() << " " << transfer1->get_crc_error_cnt() << std::endl;
    }

    return 0;
}