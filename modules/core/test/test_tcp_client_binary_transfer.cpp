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

struct LoginRequest
{
    uint32_t id;
    char data[1024 * 1024 * 10];
    char uin[32];
    char password[32];
};

struct LoginResponse
{
    uint32_t id;
    char response[32];
};

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

    std::shared_ptr<BinaryTransfer> transfer = std::make_shared<BinaryTransfer>(client);

    char uin[] = "BusyBox";
    char password[] = "12345678";
    LoginRequest* request = new LoginRequest;
    request->id = 0x12345678;
    memcpy(request->uin, uin, strlen(uin) + 1);
    memcpy(request->password, password, strlen(password) + 1);
    
    //发送数据
    transfer->send_binary(reinterpret_cast<uint8_t*>(request), sizeof(LoginRequest));
    delete request;
    
    //接收数据
    uint8_t recv_buff[128];
    transfer->recv_binary(recv_buff, 128);
    struct LoginResponse* response = reinterpret_cast<struct LoginResponse*>(recv_buff);

    std::cout << "来自服务器的消息：" << response->response << std::endl;

    return 0;
}