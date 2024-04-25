/*
 * @author BusyBox
 * @date 2024/4/25
 * @version 1.0
 */

#include <iostream>
#include <cstring>
#include <thread>
#include "rdk/core.h"


struct TestMsg
{
    int id;
    char info[32];
};

void test_thread()
{
    auto serial1 = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2:1.0-port0", 921600);
    std::cout << "打开成功1" << std::endl;
    std::shared_ptr<ReliableBinaryTransfer> transfer1 = std::make_shared<ReliableBinaryTransfer>(serial1);

    int id = 0;
    while (true) {
        std::string info = "Hello STM32 " + std::to_string(id);
        TestMsg test_msg;
        test_msg.id = id;
        memcpy(test_msg.info, info.data(), info.size() + 1);
        ++id;

        auto ret = transfer1->send_binary((char*)&test_msg, sizeof(TestMsg));
        if (ret == 0) {
            std::cout << "发送失败" << std::endl;
        }

        char recv_buff[1024];
        std::size_t recv_len = transfer1->recv_binary(recv_buff, 1024, 300);
        if (recv_len == 0) {
            std::cout << "接收失败" << std::endl;
            continue ;
        }
        TestMsg* test_msg2 = (TestMsg*)recv_buff;
        std::cout << "id: " << test_msg.id << " id: " << test_msg2->id << " info: " << test_msg2->info << std::endl;

        // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main()
{
    std::thread th1 = std::thread(test_thread);
    th1.join();
    return 0;
}