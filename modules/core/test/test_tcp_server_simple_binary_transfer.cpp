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
    char uin[32];
    char password[32];
};

struct LoginResponse
{
    uint32_t id;
    char response[32];
};

void Session(std::shared_ptr<TCPPort> client)
{
    std::cout << "有新客户端连接服务器" << std::endl;

    std::shared_ptr<SimpleBinaryTransfer> transfer = std::make_shared<SimpleBinaryTransfer>(client);

    //接收数据
    uint8_t recv_buff[128];
    transfer->recv_binary(recv_buff, 128);
    struct LoginRequest* request = reinterpret_cast<struct LoginRequest*>(recv_buff);
    printf("id:0x%x uin:%s password:%s\n", request->id, request->uin, request->password);

    char response_msg[] = "登录成功";
    struct LoginResponse response;
    memcpy(response.response, response_msg, strlen(response_msg) + 1);
    //发送数据
    transfer->send_binary(reinterpret_cast<uint8_t*>(&response), sizeof(response));
   
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