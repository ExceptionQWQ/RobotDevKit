/*
 * @author BusyBox
 * @date 2024/5/31
 * @version 1.0
 * @details 乐动激光雷达通信协议
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/lidar/ldrobot/ld_lidar.h"

LDLidar::LDLidar(std::shared_ptr<IOStream> io_stream)
{
    this->io_stream = io_stream;
    read_sensor_th = std::thread(read_sensor_thread, this);
}

LDLidar::~LDLidar()
{
    terminate_read_sensor_thread();
    read_sensor_th.join();
}

/*
 * @brief 设置雷达测量数据点输出回调函数
 */
void LDLidar::set_lidar_point_callback(LidarPointCallback lidar_point_callback)
{
    this->lidar_point_callback = lidar_point_callback;
}

/*
 * @brief 获取雷达测量点数据
 * @param angle 角度 0-360
 * @return LidarPoint 
 */
LDLidar::LidarPoint LDLidar::get_lidar_point(int angle)
{
    angle = std::clamp(angle, 0, 360);
    return points[angle];
}

uint8_t LDLidar::calc_crc8(uint8_t* data, uint8_t len)
{
    uint8_t crc = 0; 
    for (uint16_t i = 0; i < len; i++) { 
        crc = crc_table[(crc ^ *data++) & 0xff]; 
    } 
    return crc;
}

void LDLidar::read_sensor_thread(LDLidar* lidar)
{
    while (true) {
        if (lidar->is_terminate()) break;
        LidarFrame frame;
        try {
            int ret = lidar->recv_frame(reinterpret_cast<uint8_t*>(&frame), 100);
            if (ret != 0) {
                printf("[LDLidar::read_sensor_thread] recv_frame timeout\n");
                continue;
            }
        } catch (const IOStream::TimeOutException& e) {
            printf("[LDLidar::read_sensor_thread] recv_frame timeout\n");
        }
        
        uint8_t crc8 = lidar->calc_crc8(reinterpret_cast<uint8_t*>(&frame), sizeof(LidarFrame) - 1);
        
        if (crc8 != frame.crc8) {
            printf("[LDLidar::read_sensor_thread] crc error, crc8:%x frame.crc8:%x\n", crc8, frame.crc8);
            continue;
        }

        float step = 0;
        if (frame.end_angle - frame.start_angle > 0) step = (frame.end_angle - frame.start_angle) / 11.0;
        else step = (36000 + frame.end_angle - frame.start_angle) / 11.0;

        for (int i = 0; i < 12; ++i) {
            float angle = (frame.start_angle + step * i) / 100.0;
            angle = std::fmod(angle + 360, 360);

            uint16_t distance = frame.point[i].distance;
            uint8_t intensity = frame.point[i].intensity;

            if (lidar->lidar_point_callback) lidar->lidar_point_callback(angle, distance, intensity);
            int angle2 = angle;
            angle2 = std::clamp(angle2, 0, 360);
            lidar->points[angle2] = frame.point[i];
        }
    }
}

void LDLidar::terminate_read_sensor_thread()
{
    b_is_terminate = true;
}

bool LDLidar::is_terminate()
{
    return b_is_terminate;
}

/*
 * @brief 接收一帧数据
 * @param buff 接收缓冲区
 * @param timeout 超时时间
 * @return 0 接收成功 -1 超时
 */
int LDLidar::recv_frame(uint8_t* buff, int timeout)
{
    RecvStatusMachine status = RecvStatusMachine::RecvFirstHeader;
    std::size_t recv_bytes = 0;
    std::size_t remain_len = 0;
    std::size_t index = 0;
    while (true) {
        if (status == RecvStatusMachine::RecvDone) break;
        switch (status) {
            case RecvStatusMachine::RecvFirstHeader:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                if (buff[index] == 0x54) {
                    ++index;
                    status = RecvStatusMachine::RecvSecondHeader;
                }
            break;
            case RecvStatusMachine::RecvSecondHeader:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                if (buff[index] == 0x2c) {
                    ++index;
                    status = RecvStatusMachine::RecvFrame;
                } else {
                    index = 0;
                    status = RecvStatusMachine::RecvFirstHeader;
                }
            break;
            case RecvStatusMachine::RecvFrame:
                remain_len = sizeof(LidarFrame) - 2;
                while (remain_len) {
                    recv_bytes = io_stream->read(buff + index, remain_len, timeout);
                    if (recv_bytes == 0) return -1; //超时
                    remain_len -= recv_bytes;
                    index += recv_bytes;
                }
                status = RecvStatusMachine::RecvDone;
            break;
        }
    }
    return 0;
}