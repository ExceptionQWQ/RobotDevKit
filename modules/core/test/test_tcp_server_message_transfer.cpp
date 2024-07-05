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

    std::shared_ptr<SimpleMessageTransfer> transfer = std::make_shared<SimpleMessageTransfer>(client);

    //接收数据
    std::string message = transfer->recv_message();
    std::cout << "来自客户端的消息: " << message << std::endl;
    
    //发送数据
    transfer->send_message("Hello From Server");
    
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