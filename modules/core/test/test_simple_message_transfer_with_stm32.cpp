/*
 * @author BusyBox
 * @date 2024/4/23
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
    std::cout << "打开成功" << std::endl;

    std::shared_ptr<SimpleMessageTransfer> transfer1 = std::make_shared<SimpleMessageTransfer>(serial1);
    
    while (true) {
        try {
            char msg[] = "Hello WorldHello WorldHello WorldHello WorldHello World";
            transfer1->send_message(msg);
            std::string recv = transfer1->recv_message(10);
            std::cout << recv << std::endl;
            // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } catch (IOStream::TimeOutException e) {

        }
        
    }

    
    return 0;
}