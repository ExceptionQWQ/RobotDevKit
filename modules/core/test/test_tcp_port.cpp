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
    //连接www.baidu.com
    std::shared_ptr<TCPPort> client = std::make_shared<TCPPort>();
    if (client->connect("153.3.238.102", 80)) {
        std::cout << "连接成功" << std::endl;
    } else {
        std::cout << "连接失败" << std::endl;
        return -1;
    }

    //发送数据
    uint8_t send_buff[64];
    std::size_t send_bytes = client->write(send_buff, 64);
    std::cout << "send_bytes: " << send_bytes << std::endl;
    
    //接收数据
    uint8_t recv_buff[64];
    try {
        std::size_t recv_bytes = client->read(recv_buff, 2);
        std::cout << "recv_bytes: " << recv_bytes << std::endl;
    } catch (IOStream::TimeOutException e) {
        
    }
    

    return 0;
}