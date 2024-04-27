/*
 * @author BusyBox
 * @date 2024/4/27
 * @version 1.0
 */

#include <iostream>
#include <cstring>
#include <thread>
#include <stdio.h>
#include "rdk/core.h"

enum class MasterCmdType : uint8_t
{
    Ping = 0x00, //通信测试
    Motion = 0x01,  //控制底盘移动
};

struct MasterCmd
{
    MasterCmdType cmd_type;
};

struct MasterCmdMotion
{
    MasterCmdType cmd_type;
    int16_t x_speed;
    int16_t y_speed;
    int16_t z_speed;
};

std::shared_ptr<SerialPort> slave_serial;
std::shared_ptr<ReliableBinaryTransfer> slave_transfer;

void init_slave()
{
    slave_serial = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.4:1.0-port0", 921600);
    std::cout << "init_slave打开成功" << std::endl;
    slave_transfer = std::make_shared<ReliableBinaryTransfer>(slave_serial);
}

//检查与下位机的通信是否正常，函数一直阻塞至正常通信
void check_slave_connection()
{
    while (true) {
        MasterCmd cmd;
        cmd.cmd_type = MasterCmdType::Ping;
        auto ret = slave_transfer->send_binary((uint8_t*)&cmd, sizeof(MasterCmd));
        if (ret == 0) {
            std::cout << "check_slave_connection ping指令发送失败" << std::endl;
        }

        uint8_t recv_buff[32];
        auto recv_bytes = slave_transfer->recv_binary(recv_buff, 32, 100);
        if (recv_bytes) {
            std::cout << "check_slave_connection ping指令接收失败" << std::endl;
        }

        MasterCmd* ret_cmd = reinterpret_cast<MasterCmd*>(recv_buff);
        if (ret_cmd->cmd_type == MasterCmdType::Ping) {
            std::cout << "通信正常" << std::endl;
            break;
        }
    }
}

void slave_motion(int16_t x_speed, int16_t y_speed, int16_t z_speed)
{
    MasterCmdMotion cmd;
    cmd.cmd_type = MasterCmdType::Motion;
    cmd.x_speed = x_speed;
    cmd.y_speed = y_speed;
    cmd.z_speed = z_speed;
    auto ret = slave_transfer->send_binary((uint8_t*)&cmd, sizeof(MasterCmdMotion));
    if (ret == 0) {
        std::cout << "slave_motion指令发送失败" << std::endl;
    }
}

int main()
{
    init_slave();
    check_slave_connection();

    while (true) {
        char input = getchar();
        switch (input) {
            case 'w':
                slave_motion(1000, 0, 0);
            break;
            case 's':
                slave_motion(-1000, 0, 0);
            break;
            case 'a':
                slave_motion(0, 1000, 0);
            break;
            case 'd':
                slave_motion(0, -1000, 0);
            break;
            case 'q':
                slave_motion(0, 0, 100);
            break;
            case 'e':
                slave_motion(0, 0, -100);
            break;
        }
    }
    return 0;
}