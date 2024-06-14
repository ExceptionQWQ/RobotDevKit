/*
 * @author BusyBox
 * @date 2024/4/26
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/imu/wit_private_protocol.h"

WitPrivateProcotol::WitPrivateProcotol(SerialPort* serialPort)
{
    this->serialPort = serialPort;
}

WitPrivateProcotol::~WitPrivateProcotol()
{
    terminate_read_sensor_thread();
}

/*
 * @brief 开始接收数据，阻塞函数，建议新开任务调用
 */
void WitPrivateProcotol::start()
{
    read_sensor_thread(this);
}

void WitPrivateProcotol::terminate_read_sensor_thread()
{
    b_is_terminate = true;
}

bool WitPrivateProcotol::is_terminate()
{
    return b_is_terminate;
}


/*
 * @brief 获取角度信息
 * @return 角度信息，包含roll，pitch，yaw
 * @attention 当传感器超过一定时间没有返回数据时会产生超时异常
 */
WitPrivateProcotol::AngleInfo WitPrivateProcotol::get_angle_info()
{
    return angle_info;
}

std::size_t WitPrivateProcotol::recv_frame(uint8_t* buff, std::size_t buff_len)
{
    while (true) {
        std::size_t recv_bytes = serialPort->read(buff, 1, 10);
        if (recv_bytes == 0) continue;
        if (buff[0] == 0x55) break;
    }
    std::size_t remain_len = 10;
    std::size_t index = 1;
    while (remain_len) {
        std::size_t read_len = serialPort->read(buff + index, remain_len, 10);
        index += read_len;
        remain_len -= read_len;
    }
    return 11;
}

uint8_t WitPrivateProcotol::checksum(uint8_t* buff)
{
    uint8_t sum = 0;
    for (int i = 0; i < 10; ++i) {
        sum += buff[i];
    }
    return sum;
}

void WitPrivateProcotol::read_sensor_thread(WitPrivateProcotol* wit)
{
    while (true) {
        if (wit->is_terminate()) break;
        uint8_t recv_buff[32];
        memset(recv_buff, 0, 32);
        wit->recv_frame(recv_buff, 32);
        uint8_t sum = wit->checksum(recv_buff);

        if (sum != recv_buff[10]) {
            continue;
        }

        switch ((WitPrivateProcotol::RecvDataType)recv_buff[1])
        {
            case RecvDataType::Angle:
                wit->save_angle_info(recv_buff + 2);
                break;
        }
    }
    return;
}

void WitPrivateProcotol::save_angle_info(uint8_t* buff)
{
    RecvAngleInfo* recv_angle_info = (RecvAngleInfo*)buff;
    angle_info.roll = recv_angle_info->roll / 32768.0 * 180.0;
    angle_info.pitch = recv_angle_info->pitch / 32768.0 * 180.0;
    angle_info.yaw = recv_angle_info->yaw / 32768.0 * 180.0;
    angle_info.version = recv_angle_info->version;
}

void WitPrivateProcotol::unlock()
{
    WriteRegCommand wrc;
    wrc.addr = 0x69;
    wrc.data = 0xB588;
    serialPort->write((uint8_t*)&wrc, sizeof(WriteRegCommand));
}

void WitPrivateProcotol::save()
{
    WriteRegCommand wrc;
    wrc.addr = 0x00;
    wrc.data = 0x0000;
    serialPort->write((uint8_t*)&wrc, sizeof(WriteRegCommand));
}

/*
 * @brief 加速度校准必须将模块正面放置去校准，如果模块反面放置校准会导致加速度异常，从而导致角度异常
 */
void WitPrivateProcotol::acceleration_calibration()
{
    unlock();
    HAL_Delay(200);
    WriteRegCommand wrc;
    wrc.addr = 0x01;
    wrc.data = 0x0001;
    serialPort->write((uint8_t*)&wrc, sizeof(WriteRegCommand));
    HAL_Delay(5000);
    save();
}

/*
 * @brief 角度参考是以传感器当前的实际位置，让xy轴的角度归零，做一个相对归零的操作
 */
void WitPrivateProcotol::angle_reference()
{
    unlock();
    HAL_Delay(200);
    WriteRegCommand wrc;
    wrc.addr = 0x01;
    wrc.data = 0x0008;
    serialPort->write((uint8_t*)&wrc, sizeof(WriteRegCommand));
    HAL_Delay(3000);
    save();
}

/*
 * @brief z轴归零需要在6轴算法的前提下，算法切换可以在上位机配置界面修改，9轴设备下的9轴算法是绝对角度，不能归零
 */
void WitPrivateProcotol::set_z_axis_to_zero()
{
    unlock();
    HAL_Delay(200);
    WriteRegCommand wrc;
    wrc.addr = 0x01;
    wrc.data = 0x0004;
    serialPort->write((uint8_t*)&wrc, sizeof(WriteRegCommand));
    HAL_Delay(3000);
    save();
}