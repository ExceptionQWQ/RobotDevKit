/*
 * @author BusyBox
 * @date 2024/4/25
 * @version 1.0
 */

#include <iostream>
#include <cstring>
#include "rdk/core.h"


struct TestMsg
{
    int id;
    char info[32];
};

int main()
{
    auto serial = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2:1.0-port0", 921600);
    std::cout << "打开成功" << std::endl;

    std::shared_ptr<SimpleBinaryTransfer> transfer = std::make_shared<SimpleBinaryTransfer>(serial);
    
    int id = 0;
    while (true) {
        try {
            std::string info = "Hello STM32 " + std::to_string(id);
            TestMsg test_msg;
            test_msg.id = id;
            memcpy(test_msg.info, info.data(), info.size() + 1);
            ++id;

            transfer->send_binary((char*)&test_msg, sizeof(TestMsg));

            char recv_buff[1024];
            std::size_t recv_len = transfer->recv_binary(recv_buff, 1024, 50);
            if (recv_len == 0) {
                std::cout << "接收失败" << std::endl;
                continue ;
            }
            TestMsg* test_msg2 = (TestMsg*)recv_buff;
            std::cout << "id: " << test_msg2->id << " info: " << test_msg2->info << std::endl;

            // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } catch (IOStream::TimeOutException e) {

        }
    }
    return 0;
}