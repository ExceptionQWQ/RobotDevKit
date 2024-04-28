/*
 * @author BusyBox
 * @date 2024/4/28
 * @version 1.0
 */

#include <iostream>
#include <cstring>
#include <thread>
#include <utility>
#include "rdk/core.h"

enum class MasterCmdType : uint8_t
{
    Ping = 0x00, //通信测试
    GetYaw = 0x01,  //获取Yaw轴角度
    GetWheelDis = 0x02, //获取轮子里程
};

struct MasterCmd
{
    MasterCmdType cmd_type;
};

struct MasterCmdGetYaw
{
    MasterCmdType cmd_type;
    double yaw;
};

struct MasterCmdGetWheelDis
{
    MasterCmdType cmd_type;
    int32_t dis1;
    int32_t dis2;
};


std::shared_ptr<SerialPort> ops_serial;
std::shared_ptr<ReliableBinaryTransfer> ops_transfer;

void init_ops()
{
    ops_serial = std::make_shared<SerialPort>("/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.1:1.0-port0", 921600);
    std::cout << "init_slave打开成功" << std::endl;
    ops_transfer = std::make_shared<ReliableBinaryTransfer>(ops_serial);
}

//检查与ops的通信是否正常，函数一直阻塞至正常通信
void check_ops_connection()
{
    while (true) {
        MasterCmd cmd;
        cmd.cmd_type = MasterCmdType::Ping;
        auto ret = ops_transfer->send_binary((uint8_t*)&cmd, sizeof(MasterCmd));
        if (ret == 0) {
            std::cout << "check_slave_connection ping指令发送失败" << std::endl;
        }

        uint8_t recv_buff[32];
        auto recv_bytes = ops_transfer->recv_binary(recv_buff, 32, 100);
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

double get_yaw()
{
    MasterCmdGetYaw cmd;
    cmd.cmd_type = MasterCmdType::GetYaw;
    auto ret = ops_transfer->send_binary((uint8_t*)&cmd, sizeof(MasterCmdGetYaw));
    if (ret == 0) {
        std::cout << "get_yaw指令发送失败" << std::endl;
    }
    uint8_t recv_buff[32];
    auto recv_bytes = ops_transfer->recv_binary(recv_buff, 32, 100);
    if (recv_bytes == 0) {
        std::cout << "get_yaw指令接收失败" << std::endl;
        return 0;
    }
    MasterCmdGetYaw* ret_cmd = reinterpret_cast<MasterCmdGetYaw*>(recv_buff);
    if (ret_cmd->cmd_type != MasterCmdType::GetYaw) {
        std::cout << "get_yaw指令解析失败" << std::endl;
        return 0;
    }
    return ret_cmd->yaw;
}

std::pair<int32_t, int32_t> get_wheel_dis()
{
    MasterCmdGetWheelDis cmd;
    cmd.cmd_type = MasterCmdType::GetWheelDis;
    auto ret = ops_transfer->send_binary((uint8_t*)&cmd, sizeof(MasterCmdGetWheelDis));
    if (ret == 0) {
        std::cout << "get_wheel_dis指令发送失败" << std::endl;
    }
    uint8_t recv_buff[32];
    auto recv_bytes = ops_transfer->recv_binary(recv_buff, 32, 100);
    if (recv_bytes == 0) {
        std::cout << "get_wheel_dis指令接收失败" << std::endl;
        return std::make_pair(0, 0);
    }
    MasterCmdGetWheelDis* ret_cmd = reinterpret_cast<MasterCmdGetWheelDis*>(recv_buff);
    if (ret_cmd->cmd_type != MasterCmdType::GetWheelDis) {
        std::cout << "get_wheel_dis指令解析失败" << std::endl;
        return std::make_pair(0, 0);
    }
    return std::make_pair(ret_cmd->dis1, ret_cmd->dis2);
}

int main()
{
    init_ops();
    check_ops_connection();

    while (true) {
        double yaw = get_yaw();
        auto wheel_dis = get_wheel_dis();
        std::cout << "yaw: " << yaw << " dis1: " << wheel_dis.first << " dis2: " << wheel_dis.second << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}