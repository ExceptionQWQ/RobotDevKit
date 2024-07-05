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

    std::shared_ptr<SimpleMessageTransfer> transfer = std::make_shared<SimpleMessageTransfer>(client);

    //发送数据
    transfer->send_message("Hello From Client");
    
    //接收数据
    std::string message = transfer->recv_message();
    std::cout << "来自服务器的消息: " << message << std::endl;

    return 0;
}