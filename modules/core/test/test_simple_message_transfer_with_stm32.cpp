/*
 * @author BusyBox
 * @date 2024/4/23
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include <iostream>
#include <cstring>
#include "rdk/core.h"

int main()
{
    auto serial1 = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2:1.0-port0", 921600);
    std::cout << "打开成功" << std::endl;

    std::shared_ptr<SimpleMessageTransfer> transfer1 = std::make_shared<SimpleMessageTransfer>(serial1);
    
    int cnt = 0;
    while (true) {
        try {
            std::string msg = "Are you ok? Are you ok? Are you ok? Are you ok? Are you ok? Are you ok? Are you ok? Are you ok? Are you ok? Are you ok? Are you ok? " + std::to_string(cnt++);
            transfer1->send_message(msg);
            std::string recv = transfer1->recv_message(20);
            std::cout << recv << std::endl;
            // std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } catch (IOStream::TimeOutException e) {

        }
        
    }

    
    return 0;
}