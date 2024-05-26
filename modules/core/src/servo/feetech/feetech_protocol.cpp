/*
 * @author BusyBox
 * @date 2024/5/26
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/servo/feetech/feetech_protocol.h"


FeetechProtocol::FeetechProtocol(std::shared_ptr<IOStream> io_stream)
{
    this->io_stream = io_stream;
}

FeetechProtocol::~FeetechProtocol()
{

}

/*
 * @brief 校验和, 对每个字节数据求和，最后结果取反
 * @param data 数据指针
 * @aram len 数据长度
 * @return sum
 */
uint8_t FeetechProtocol::checksum(uint8_t* data, std::size_t len)
{
    uint8_t sum = 0;
    for (std::size_t i = 0; i != len; ++i) sum += data[i];
    return ~(sum % 256);
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
std::size_t FeetechProtocol::encode(uint8_t* buff, uint8_t id, ControlCMD cmd, uint8_t* data, std::size_t len)
{
    if (len > 240) return 0;
    buff[0] = 0xff;
    buff[1] = 0xff;
    buff[2] = id;
    buff[3] = len + 2;
    buff[4] = static_cast<uint8_t>(cmd);
    memcpy(buff + 5, data, len);
    uint8_t sum = checksum(buff + 2, 3 + len);
    buff[5 + len] = sum;
    return 5 + len + 1;
}

/*
 * @brief 将数据包解码成数据
 * @param buff 数据包缓冲区
 * @param buff_len 数据包长度
 * @param id 输出舵机id
 * @param error 输出当前状态
 * @param data 输出数据包指针
 * @param data_len 输出数据长度
 * @return 是否解码成功,成功返回0
 */
int FeetechProtocol::decode(uint8_t* buff, std::size_t buff_len, uint8_t* id, uint8_t* error, uint8_t** data, std::size_t* data_len)
{
    if (buff[0] != 0xff || buff[1] != 0xff) return -1;
    *id = buff[2];
    *data_len = buff[3] - 2;
    *error = buff[4];
    if (buff_len < 5 + *data_len + 1) return -2;
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
int FeetechProtocol::recv_package(uint8_t* buff, std::size_t buff_len, std::size_t* package_len, int timeout)
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
                if (buff[index] == 0xff) {
                    ++index;
                    status = RecvStatusMachine::RecvSecondHeader;
                }
            break;
            case RecvStatusMachine::RecvSecondHeader:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                if (buff[index] == 0xff) {
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
                data_len = buff[index] - 2;
                ++index;
                status = RecvStatusMachine::RecvError;
            break;
            case RecvStatusMachine::RecvError:
                recv_bytes = io_stream->read(buff + index, 1, timeout);
                if (recv_bytes == 0) return -1; //超时
                ++index;
                status = RecvStatusMachine::RecvData;
            break;
            case RecvStatusMachine::RecvData:
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
 * @brief 检查舵机是否在线
 * @param id 舵机id
 * @param ms 超时时间
 */
bool FeetechProtocol::ping(uint8_t id, int ms)
{
    uint8_t buff[16];
    std::size_t len = encode(buff, id, ControlCMD::Ping, nullptr, 0);
    io_stream->write(buff, len);
    std::size_t package_len = 0;
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 16, &package_len, ms) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t data_len;
        ControlCMD cmd;
        uint8_t* data;
        uint8_t recv_id;
        uint8_t recv_error;
        if (decode(buff, 16, &recv_id, &recv_error, &data, &data_len) != 0) continue;
        if (recv_id == id && recv_error == 0 && data_len == 0) return true;
    }
    return false;
}

/*
 * @brief 舵机数据读取
 * @param id 舵机id
 * @param addr 舵机内存控制表地址
 * @param data_len 要读取的数据长度
 * @param data 接收数据缓冲区
 * @return 是否读取成功
 */
bool FeetechProtocol::read_data(uint8_t id, uint8_t addr, uint8_t* data, uint8_t data_len)
{
    uint8_t buff[256];
    uint8_t request[2];
    request[0] = addr;
    request[1] = data_len;
    std::size_t len = encode(buff, id, ControlCMD::ReadData, request, 2);
    io_stream->write(buff, len);
    std::size_t package_len = 0;
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 256, &package_len, timeout) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t recv_data_len;
        uint8_t recv_id;
        uint8_t recv_error;
        uint8_t* recv_data;
        if (decode(buff, 256, &recv_id, &recv_error, &recv_data, &recv_data_len) != 0) continue;
        if (recv_id == id && recv_error == 0) {
            memcpy(data, recv_data, recv_data_len);
            return true;
        }
    }
    return false;
}

/*
 * @brief 舵机数据写入
 * @param id 舵机id
 * @param addr 舵机内存控制表地址
 * @param data 数据
 * @param data_len 写入数据长度
 * @return 是否写入成功
 */
bool FeetechProtocol::write_data(uint8_t id, uint8_t addr, uint8_t* data, uint8_t data_len)
{
    uint8_t buff[256];
    uint8_t request[256];
    request[0] = addr;
    memcpy(request + 1, data, data_len);
    std::size_t len = encode(buff, id, ControlCMD::WriteData, request, data_len + 1);
    io_stream->write(buff, len);
    std::size_t package_len = 0;
    if (id == 0xfe) return true; //因为采用广播ID 发送指令，所以不会有数据返回。
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 256, &package_len, timeout) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t recv_data_len;
        uint8_t recv_id;
        uint8_t recv_error;
        uint8_t* recv_data;
        if (decode(buff, 256, &recv_id, &recv_error, &recv_data, &recv_data_len) != 0) continue;
        if (recv_id == id && recv_error == 0 && recv_data_len == 0) return true;
    }
    return false;
}

/*
 * @brief 异步写指令
 * @param id 舵机id
 * @param addr 舵机内存控制表地址
 * @param data 数据
 * @param data_len 写入数据长度
 * @return 是否写入成功
 */
bool FeetechProtocol::reg_write(uint8_t id, uint8_t addr, uint8_t* data, uint8_t data_len)
{
    uint8_t buff[256];
    uint8_t request[256];
    request[0] = addr;
    memcpy(request + 1, data, data_len);
    std::size_t len = encode(buff, id, ControlCMD::RegWrite, request, data_len + 1);
    io_stream->write(buff, len);
    std::size_t package_len = 0;
    if (id == 0xfe) return true; //因为采用广播ID 发送指令，所以不会有数据返回。
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 256, &package_len, timeout) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t recv_data_len;
        uint8_t recv_id;
        uint8_t recv_error;
        uint8_t* recv_data;
        if (decode(buff, 256, &recv_id, &recv_error, &recv_data, &recv_data_len) != 0) continue;
        if (recv_id == id && recv_error == 0 && recv_data_len == 0) return true;
    }
    return false;
}

/*
 * @brief 执行异步写指令
 * @details 在控制多个舵机时，使用ACTION指令可以使第一个和最后一个舵机同时执行各自的动作，中间无延时。
 */
bool FeetechProtocol::action()
{
    uint8_t buff[16];
    std::size_t len = encode(buff, 0xfe, ControlCMD::Action, nullptr, 0);
    io_stream->write(buff, len);
    return true;
}

/*
 * @brief 复位指令
 * @param id 舵机id
 * @details 把内存控制表里特定的数据进行复位（特定舵机型号采用）
 */
bool FeetechProtocol::reset(uint8_t id)
{
    uint8_t buff[16];
    std::size_t len = encode(buff, id, ControlCMD::Reset, nullptr, 0);
    io_stream->write(buff, len);
    std::size_t package_len = 0;
    for (int i = 0; i < 3; ++i) {
        try {
            if (recv_package(buff, 16, &package_len, timeout) != 0) continue;
        } catch (const IOStream::TimeOutException& e) {
            continue;
        }
        std::size_t data_len;
        ControlCMD cmd;
        uint8_t* data;
        uint8_t recv_id;
        uint8_t recv_error;
        if (decode(buff, 16, &recv_id, &recv_error, &data, &data_len) != 0) continue;
        if (recv_id == 1 && recv_error == 0 && data_len == 0) return true;
    }
    return false;
}

/*
 * @brief 舵机扭矩开关
 * @param id 舵机id
 * @param value true 打开扭矩输出 false 关闭扭矩输出
 */
bool FeetechProtocol::torque_switch(uint8_t id, bool value)
{
    return write_data(id, static_cast<uint8_t>(MemoryTable::TorqueSwitch), reinterpret_cast<uint8_t*>(&value), 1);
}

/*
 * @brief 读取舵机位置
 * @param id 舵机id
 * @param position 位置
 */
bool FeetechProtocol::read_position(uint8_t id, int16_t* position, bool endian_reverse)
{
    int16_t pos = 0;
    bool ret = read_data(id, static_cast<uint8_t>(MemoryTable::CurrentPostion), reinterpret_cast<uint8_t*>(&pos), 2);
    if (endian_reverse) pos = boost::endian::endian_reverse(pos);
    *position = pos;
    return ret;
}

/*
 * @brief 写入舵机位置
 * @param id 舵机id
 * @param position 位置
 */
bool FeetechProtocol::write_position(uint8_t id, int16_t position, bool endian_reverse)
{
    if (endian_reverse) position = boost::endian::endian_reverse(position);
    return write_data(id, static_cast<uint8_t>(MemoryTable::TargetPosition), reinterpret_cast<uint8_t*>(&position), 2);
}

/*
 * @brief 写入位置速度
 * @param id 舵机id
 * @param speed 步/s
 */
bool FeetechProtocol::write_position_speed(uint8_t id, int16_t position, uint16_t speed, bool endian_reverse)
{
    if (endian_reverse) position = boost::endian::endian_reverse(position);
    if (endian_reverse) speed = boost::endian::endian_reverse(speed);
    uint8_t data[6];
    *reinterpret_cast<int16_t*>(data + 0) = position;
    *reinterpret_cast<uint16_t*>(data + 2) = 0;
    *reinterpret_cast<uint16_t*>(data + 4) = speed;
    return write_data(id, static_cast<uint8_t>(MemoryTable::TargetPosition), data, 6);
}