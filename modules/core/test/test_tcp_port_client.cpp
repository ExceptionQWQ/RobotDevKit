/*
 * @author BusyBox
 * @date 2024/7/5
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include <iostream>
#include <cstring>
#include <memory>
#include "rdk/core.h"

int main()
{
    //连接本机服务器
    std::shared_ptr<TCPPort> client = std::make_shared<TCPPort>();
    if (client->connect("127.0.0.1", 7788)) {
        std::cout << "连接成功" << std::endl;
    } else {
        std::cout << "连接失败" << std::endl;
        return -1;
    }

    //发送数据
    char send_buff[] = "Hello From Client";
    std::size_t send_bytes = client->write(reinterpret_cast<uint8_t*>(send_buff), strlen(send_buff) + 1);
    std::cout << "已发送的字节：" << send_bytes << std::endl;
    
    //接收数据
    uint8_t recv_buff[64];
    std::size_t recv_bytes = client->read(recv_buff, 64);
    std::string server_msg = std::string(reinterpret_cast<char*>(recv_buff), recv_bytes);
    std::cout << "recv_bytes: " << recv_bytes << std::endl;
    std::cout << "来自服务端的消息：" << server_msg << std::endl;

    client->close(); //关闭连接

    return 0;
}