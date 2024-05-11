/*
 * @author BusyBox
 * @date 2024/4/26
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/imu/wit_private_protocol.h"

WitPrivateProcotol::WitPrivateProcotol(std::shared_ptr<IOStream> io_stream)
{
    this->io_stream = io_stream;
    read_sensor_th = std::thread(read_sensor_thread, this);
    angle_info.time_stamp = get_time_stamp();
}

WitPrivateProcotol::~WitPrivateProcotol()
{
    terminate_read_sensor_thread();
    read_sensor_th.join();
}

void WitPrivateProcotol::terminate_read_sensor_thread()
{
    b_is_terminate = true;
}

bool WitPrivateProcotol::is_terminate()
{
    return b_is_terminate;
}

uint32_t WitPrivateProcotol::get_time_stamp()
{
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

/*
 * @brief 设置超时时间
 * @param timeout 超时时间（秒）
 * @details 当传感器超过一定时间没有返回数据时会产生超时异常
 */
void WitPrivateProcotol::set_timeout(int timeout)
{   
    this->timeout = timeout;
}

/*
 * @brief 获取角度信息
 * @return 角度信息，包含roll，pitch，yaw
 * @attention 当传感器超过一定时间没有返回数据时会产生超时异常
 */
WitPrivateProcotol::AngleInfo WitPrivateProcotol::get_angle_info()
{
    uint32_t time_stamp = get_time_stamp();
    if (time_stamp - angle_info.time_stamp > this->timeout) {
        throw WitPrivateProcotol::TimeOutException{"[WitPrivateProcotol::get_angle_info] TimeOut"};
    }
    return angle_info;
}

std::size_t WitPrivateProcotol::recv_frame(uint8_t* buff, std::size_t buff_len)
{
    while (true) {
        io_stream->read(buff, 1);
        if (buff[0] == 0x55) break;
    }
    std::size_t remain_len = 10;
    std::size_t index = 1;
    while (remain_len) {
        std::size_t read_len = io_stream->read(buff + index, remain_len);
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
        
        // for (int i = 0; i < 11; ++i) printf("%02x ", (uint8_t)recv_buff[i]);
        // printf("\n");

        if (sum != recv_buff[10]) {
            // printf("sum:%02x %02x\n", sum, recv_buff[10]);
            continue;
        }

        switch ((WitPrivateProcotol::RecvDataType)recv_buff[1])
        {
            case RecvDataType::Angle:
                wit->save_angle_info(recv_buff + 2);
                wit->unlock();
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
    angle_info.time_stamp = get_time_stamp();
}

void WitPrivateProcotol::unlock()
{
    WriteRegCommand wrc;
    wrc.addr = 0x69;
    wrc.data = 0xB588;
    io_stream->write((uint8_t*)&wrc, sizeof(WriteRegCommand));
}

void WitPrivateProcotol::save()
{
    WriteRegCommand wrc;
    wrc.addr = 0x00;
    wrc.data = 0x0000;
    io_stream->write((uint8_t*)&wrc, sizeof(WriteRegCommand));
}

/*
 * @brief 加速度校准必须将模块正面放置去校准，如果模块反面放置校准会导致加速度异常，从而导致角度异常
 */
void WitPrivateProcotol::acceleration_calibration()
{
    unlock();  
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
    WriteRegCommand wrc;
    wrc.addr = 0x01;
    wrc.data = 0x0001;
    io_stream->write((uint8_t*)&wrc, sizeof(WriteRegCommand));
    std::this_thread::sleep_for(std::chrono::milliseconds(5000)); 
    save();
}

/*
 * @brief 角度参考是以传感器当前的实际位置，让xy轴的角度归零，做一个相对归零的操作
 */
void WitPrivateProcotol::angle_reference()
{
    unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
    WriteRegCommand wrc;
    wrc.addr = 0x01;
    wrc.data = 0x0008;
    io_stream->write((uint8_t*)&wrc, sizeof(WriteRegCommand));
    std::this_thread::sleep_for(std::chrono::milliseconds(3000)); 
    save();
}

/*
 * @brief z轴归零需要在6轴算法的前提下，算法切换可以在上位机配置界面修改，9轴设备下的9轴算法是绝对角度，不能归零
 */
void WitPrivateProcotol::set_z_axis_to_zero()
{
    unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
    WriteRegCommand wrc;
    wrc.addr = 0x01;
    wrc.data = 0x0004;
    io_stream->write((uint8_t*)&wrc, sizeof(WriteRegCommand));
    std::this_thread::sleep_for(std::chrono::milliseconds(3000)); 
    save();
}