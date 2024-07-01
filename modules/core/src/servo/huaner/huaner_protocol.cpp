/*
 * @author BusyBox
 * @date 2024/7/1
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/servo/huaner/huaner_protocol.h"

HuanerProtocol::HuanerProtocol(std::shared_ptr<IOStream> io_stream)
{
    this->io_stream = io_stream;
}

HuanerProtocol::~HuanerProtocol()
{

}

/*
 * @brief 校验和, 对每个字节数据求和
 * @param data 数据指针
 * @aram len 数据长度
 * @return sum
 */
uint8_t HuanerProtocol::checksum(uint8_t* data, std::size_t len)
{
    uint8_t sum = 0;
    for (std::size_t i = 0; i != len; ++i) sum += data[i];
    return ~sum;
}

/*
 * @brief 将数据编码成数据包
 * @param buff 数据包缓冲区，要求大小能够存储数据的长度+5
 * @param id 舵机id
 * @param cmd 控制指令
 * @param data 数据指针
 * @param len 数据长度
 * @return 数据包长度
 */
std::size_t HuanerProtocol::encode(uint8_t* buff, uint8_t id, ControlCMD cmd, uint8_t* data, std::size_t len)
{
    if (len > 240) return 0;
    buff[0] = 0x55;
    buff[1] = 0x55;
    buff[2] = id;
    buff[3] = len + 3;
    buff[4] = static_cast<uint8_t>(cmd);
    memcpy(buff + 5, data, len);
    uint8_t sum = checksum(buff + 2, 3 + len);
    buff[5 + len] = sum;
    return 5 + len + 1;
}

/*
 * @brief 将数据包解码成数据
 * @param buff 数据包缓冲区
 * @param buff_size 数据包长度
 * @param id 输出舵机id
 * @param cmd 控制指令
 * @param data 输出数据包指针
 * @param data_len 输出数据长度
 * @return 是否解码成功,成功返回0
 */
int HuanerProtocol::decode(uint8_t* buff, std::size_t buff_len, uint8_t* id, ControlCMD* cmd, uint8_t** data, std::size_t* data_len)
{
    if (buff[0] != 0x55|| buff[1] != 0x55) return -1;
    if (id) *id = buff[2];
    if (data_len) *data_len = buff[3] - 3;
    if (cmd) *cmd = static_cast<ControlCMD>(buff[4]);
    if (buff_len < 5+ *data_len + 1) return -2;
    *data = buff + 5;
    uint8_t sum = checksum(buff + 2, 3 + *data_len);
    if (sum != buff[5 + *data_len]) return -3;
    return 0;
}

/*
 * @brief 接收响应数据包
 * @param buff 接收数据缓冲区
 * @param buff_len 缓冲区大小
 * @param package_len 接收到的数据包大小
 * @param timeout 超时时间
 * @return 是否成功接收，成功返回0，不成功返回-1
 */
int HuanerProtocol::recv_package(uint8_t* buff, std::size_t buff_len, std::size_t* package_len, int timeout)
{
    RecvStatusMachine status = RecvStatusMachine::RecvFirstHeader;
    std::size_t recv_bytes = 0;
    std::size_t index = 0;
    std::size_t data_len = 0;
    std::size_t remain_len = 0;
    while (true) {
        if (status == RecvStatusMachine::RecvDone) break;
        switch (status) {
            case RecvStatusMachine::RecvFirstHeader:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                if (buff[index] == 0x55) {
                    ++index;
                    status = RecvStatusMachine::RecvSecondHeader;
                }
            break;
            case RecvStatusMachine::RecvSecondHeader:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                if (buff[index] == 0x55) {
                    ++index;
                    status = RecvStatusMachine::RecvID;
                } else {
                    index = 0;
                    status = RecvStatusMachine::RecvFirstHeader;
                }
            break;
            case RecvStatusMachine::RecvID:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                ++index;
                status = RecvStatusMachine::RecvDataLen;
            break;
            case RecvStatusMachine::RecvDataLen:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                data_len = buff[index] - 3;
                ++index;
                status = RecvStatusMachine::RecvCMD;
            break;
            case RecvStatusMachine::RecvCMD:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                ++index;
                status = RecvStatusMachine::RecvParams;
            break;
            case RecvStatusMachine::RecvParams:
                remain_len = data_len;
                while (remain_len) {
                    recv_bytes = io_stream->read(buff + index, remain_len, timeout);
                    if (recv_bytes == 0) return -1; //超时
                    remain_len -= recv_bytes;
                    index += recv_bytes;
                }
                status = RecvStatusMachine::RecvCheckSum;
            break;
            case RecvStatusMachine::RecvCheckSum:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                ++index;
                status = RecvStatusMachine::RecvDone;
            break;  
        }
    }
    *package_len = index;
    return 0;
}

/*
 * @brief 舵机将在参数时间内从当前角度匀速转动到参数角度
 * @param id 舵机id
 * @param angle 范围 0~1000，对应舵机角度的 0~240°
 * @param time 时间的范围 0~30000 毫秒
 */
void HuanerProtocol::move_time_write(uint8_t id, uint16_t angle, uint16_t time)
{
    uint8_t data[4];
    *reinterpret_cast<uint16_t*>(data + 0) = angle;
    *reinterpret_cast<uint16_t*>(data + 2) = time;
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_MOVE_TIME_WRITE, data, 4);
    io_stream->write(buff, package_size);
}

/*
 * @brief 舵机将在参数时间内从当前角度匀速转动到参数角度
 * @attention 不同的是该指令到达舵机后，舵机不会立即转动，需要等待指令名SERVO_MOVE_START送达舵机后，舵机才会转动，将在参数时间内从当前角度匀速转动到参数角度。
 * @param id 舵机id
 * @param angle 范围 0~1000，对应舵机角度的 0~240°
 * @param time 时间的范围 0~30000 毫秒
 */
void HuanerProtocol::move_time_wait_write(uint8_t id, uint16_t angle, uint16_t time)
{
    uint8_t data[4];
    *reinterpret_cast<uint16_t*>(data + 0) = angle;
    *reinterpret_cast<uint16_t*>(data + 2) = time;
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_MOVE_TIME_WAIT_WRITE, data, 4);
    io_stream->write(buff, package_size);
}

/*
 * @brief 配合指令SERVO_MOVE_TIME_WAIT_WRITE 使用，在第3点中有说明。
 * @param id 舵机id
 */
void HuanerProtocol::move_start(uint8_t id)
{   
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_MOVE_START, nullptr, 0);
    io_stream->write(buff, package_size);
}

/*
 * @brief 该指令到达舵机，如果舵机正在转动，就会立即停止转动。停在当前角度位置。
 * @param id 舵机id
 */
void HuanerProtocol::move_stop(uint8_t id)
{
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_MOVE_STOP, nullptr, 0);
    io_stream->write(buff, package_size);
}

/*
 * @brief 写入新的舵机ID
 * @param id 舵机id
 * @param id2 舵机的ID值，范围 0~253，默认为1。该指令会重新给舵机写入ID值，并且掉电保存。
 */
void HuanerProtocol::id_write(uint8_t id, uint8_t id2)
{
    uint8_t data[1];
    data[0] = id2;
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_ID_WRITE, data, 1);
    io_stream->write(buff, package_size);
}

/*
 * @brief 读取舵机ID
 * @param id 舵机id
 * @param id2 舵机的ID值，范围 0~253，默认为1。
 * @return 1 读取成功 0 读取失败
 */
bool HuanerProtocol::id_read(uint8_t id, uint8_t* id2)
{
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_ID_READ, nullptr, 0);
    io_stream->write(buff, package_size);

    std::size_t package_len = 0;
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 64, &package_len, timeout) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t data_len;
        ControlCMD cmd;
        uint8_t* data;
        if (decode(buff, 64, id2, &cmd, &data, &data_len) != 0) continue;
        if (data_len == 1) return true;
    }
    return false;
}

/*
 * @brief 调整舵机内部的偏差值
 * @param id 舵机id
 * @param offset 舵机内部的偏差值，范围-125~125，对应角度为-30°~30°，该指令到达舵机，舵机将立即转动进行偏差调整。
 * @attention 注意 1:通过此指令调整好的偏差值不支持掉电保存，想要保存请参加第 10 点。注意 2:因为该参数为 signed char 型数据，而发送的指令包都为 unsigned char 型数据，所以发送之前要做强制转换成 unsigned char 型数据放到指令包中 进行发送
 */
void HuanerProtocol::angle_offset_adjust(uint8_t id, uint8_t offset)
{
    uint8_t data[1];
    data[0] = offset;
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_ANGLE_OFFSET_ADJUST, data, 1);
    io_stream->write(buff, package_size);
}

/*
 * @brief 写入舵机内部的偏差值
 * @param id 舵机id
 * @param offset 舵机内部的偏差值，范围-125~125，对应角度为-30°~30°，该指令到达舵机，舵机将立即转动进行偏差调整。
 * @attention 注意 1:通过此指令调整好的偏差值支持掉电保存，想要保存请参加第 10 点。注意 2:因为该参数为 signed char 型数据，而发送的指令包都为 unsigned char 型数据，所以发送之前要做强制转换成 unsigned char 型数据放到指令包中 进行发送
 */
void HuanerProtocol::angle_offset_write(uint8_t id, uint8_t offset)
{
    uint8_t data[1];
    data[0] = offset;
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_ANGLE_OFFSET_WRITE, data, 1);
    io_stream->write(buff, package_size);
}

/*
 * @brief 舵机角度限制
 * @param id 舵机id
 * @param min 最小角度 范围 0~1000
 * @param max 最大角度 范围 0~1000
 */
void HuanerProtocol::angle_limit_write(uint8_t id, uint16_t min, uint16_t max)
{
    uint8_t data[4];
    *reinterpret_cast<uint16_t*>(data + 0) = min;
    *reinterpret_cast<uint16_t*>(data + 2) = max;
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_ANGLE_LIMIT_WRITE, data, 4);
    io_stream->write(buff, package_size);
}

/*
 * @brief 舵机电压限制
 * @param id 舵机id
 * @param min 最小电压 范围 4500~12000 毫伏
 * @param max 最大电压 范围 4500~12000 毫伏
 * @attention 且最小输入电压值要 始终小于最大输入电压值。该命令发送给舵机，舵机的输入电压将被限制在最 小与最大之间。超出范围舵机的 LED 灯将会闪烁报警，(如果设置了 LED 报 警)为了保护舵机，其内的电机将会处于卸载断电状态，此时舵机将不会输出 力矩，并且输入电压限制值支持掉电保存。
 */
void HuanerProtocol::vin_limit_write(uint8_t id, uint16_t min, uint16_t max)
{
    uint8_t data[4];
    *reinterpret_cast<uint16_t*>(data + 0) = min;
    *reinterpret_cast<uint16_t*>(data + 2) = max;
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_VIN_LIMIT_WRITE, data, 4);
    io_stream->write(buff, package_size);
}

/*
 * @brief 舵机内部最高温度限制
 * @param id 舵机id
 * @param temp 舵机内部最高温度限制，范围 50~100°C，默认值 85°C
 */
void HuanerProtocol::temp_max_limit_write(uint8_t id, uint8_t temp)
{
    uint8_t data[1];
    data[0] = temp;
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_TEMP_MAX_LIMIT_WRITE, data, 1);
    io_stream->write(buff, package_size);
}

/*
 * @brief 舵机/电机控制模式
 * @param id 舵机id
 * @param mode 舵机模式，范围 0 或 1，0 代表位置控制模式，1 代表电机控制模式， 默认值 0，
 * @param speed 范围-1000~1000，只在电机控制模式时有效，控 制电机的转速，该值为负值代表反转，正值代表正转。写入的模式和速度不支 持掉电保存。
 */
void HuanerProtocol::servo_or_motor_mode_write(uint8_t id, uint8_t mode, uint16_t speed)
{
    uint8_t data[4];
    *reinterpret_cast<uint8_t*>(data + 0) = mode;
    *reinterpret_cast<uint16_t*>(data + 2) = speed;
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_OR_MOTOR_MODE_WRITE, data, 4);
    io_stream->write(buff, package_size);
}

/*
 * @brief 舵机内部电机是否卸载掉电
 * @param id 舵机id
 * @param value 舵机内部电机是否卸载掉电，范围 0 或 1，0 代表卸载掉电，此时舵机 无力矩输出。1 代表装载电机，此时舵机有力矩输出，默认值 0。
 */
void HuanerProtocol::load_or_unload_write(uint8_t id, uint8_t value)
{
    uint8_t data[1];
    data[0] = value;
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_LOAD_OR_UNLOAD_WRITE, data, 1);
    io_stream->write(buff, package_size);
}

/*
 * @brief 读取舵机位置
 * @param id 舵机id
 * @param pos 返回的角度位置值
 * @return 1 读取成功 0 读取失败
 */
bool HuanerProtocol::pos_read(uint8_t id, int16_t* pos)
{
    uint8_t buff[64];
    std::size_t package_size = encode(buff, id, ControlCMD::SERVO_POS_READ, nullptr, 0);
    io_stream->write(buff, package_size);

    std::size_t package_len = 0;
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 64, &package_len, timeout) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t data_len;
        ControlCMD cmd;
        uint8_t* data;
        if (decode(buff, 64, &id, &cmd, &data, &data_len) != 0) continue;
        *pos = *reinterpret_cast<int16_t*>(data);
        if (data_len == 2) return true;
    }
    return false;
}