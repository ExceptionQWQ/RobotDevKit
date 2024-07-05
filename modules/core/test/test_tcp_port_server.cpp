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

void Session(std::shared_ptr<TCPPort> client)
{
    std::cout << "有新客户端连接服务器" << std::endl;

    //接收数据
    uint8_t recv_buff[64];
    std::size_t recv_bytes = client->read(recv_buff, 64);
    std::string client_msg = std::string(reinterpret_cast<char*>(recv_buff), recv_bytes);
    std::cout << "recv_bytes: " << recv_bytes << std::endl;
    std::cout << "来自客户端的消息：" << client_msg << std::endl;

    //发送数据
    char send_buff[] = "Hello From Server!";
    std::size_t send_bytes = client->write(reinterpret_cast<uint8_t*>(send_buff), strlen(send_buff) + 1);
    std::cout << "已发送的字节：" << send_bytes << std::endl;

    client->close();
    std::cout << "服务器主动断开连接" << std::endl;
}

int main()
{
    std::shared_ptr<TCPPort> server = std::make_shared<TCPPort>();
    server->set_port(7788); //设置服务器端口，并开启监听

    while (true) {
        auto client = server->accept(); //接受一个客户端连接请求
        std::thread th(Session, client); //启动一个线程处理 服务器与客户端 的交互
        th.join();
    }

    return 0;
}