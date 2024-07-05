/*
 * @author BusyBox
 * @date 2024/4/25
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/transfer/binary_transfer.h"

BinaryTransfer::BinaryTransfer(std::shared_ptr<IOStream> io_stream)
{
    this->io_stream = io_stream;
    crc8_calculator = std::make_shared<Crc8Calculator>(0x31);
}

BinaryTransfer::~BinaryTransfer()
{

}

/*
 * @brief 获取已发送的字节数
 */
std::size_t BinaryTransfer::get_send_bytes()
{
    return send_bytes;
}

/*
 * @brief 获取接收到的字节数
 */
std::size_t BinaryTransfer::get_recv_bytes()
{
    return recv_bytes;
}

/*
 * @brief 清除已发送的字节数
 */
void BinaryTransfer::clear_send_bytes()
{
    send_bytes = 0;
}

/*
 * @brief 清除接收到的字节数
 */
void BinaryTransfer::clear_recv_bytes()
{
    recv_bytes = 0;
}

/*
 * @brief 获取最大能够传输数据的长度
 */
std::size_t BinaryTransfer::get_max_transfer_binary_size()
{
    return max_transfer_binary_size;
}

/*
 * @brief 获取crc错误计数
 */
std::size_t BinaryTransfer::get_crc_error_cnt()
{
    return crc_error_cnt;
}

/*
 * @brief 发送二进制数据
 * @param data 数据
 * @param len 数据长度
 */
void BinaryTransfer::send_binary(uint8_t* data, std::size_t len)
{
    uint8_t* buff = new uint8_t[len + 32];
    std::size_t pkg_len = encode(buff, data, len);
    io_stream->write(buff, pkg_len);
    send_bytes += pkg_len;
    delete buff;
}

/*
 * @brief 接收二进制数据
 * @param buff 接收缓冲区
 * @param buff_len 缓冲区长度
 * @param timeout 超时时间（毫秒）
 * @return 数据长度
 */
std::size_t BinaryTransfer::recv_binary(uint8_t* buff, std::size_t buff_len, int timeout)
{
    uint8_t* recv_buff = new uint8_t[buff_len + 32];

    std::size_t pkg_len = 0;
    std::size_t binary_len = 0;
    recv_status = RecvStatus::RecvFirstFrameStart;
    while (true) {
        if (check_recv_status()) break;
        switch (recv_status) {
            case RecvStatus::RecvFirstFrameStart:
                io_stream->read(recv_buff + pkg_len, 1, timeout);
                if (recv_buff[pkg_len] == 0xAA) {
                    recv_status = RecvStatus::RecvSecondFrameStart;
                    ++pkg_len;
                } else {
                    pkg_len = 0;
                }
            break;

            case RecvStatus::RecvSecondFrameStart:
                io_stream->read(recv_buff + pkg_len, 1, timeout);
                if (recv_buff[pkg_len] == 0xAA) {
                    recv_status = RecvStatus::RecvBinaryLen;
                    ++pkg_len;
                } else {
                    pkg_len = 0;
                }
            break;

            case RecvStatus::RecvBinaryLen:
                io_stream->read(recv_buff + pkg_len, 4, timeout);
                binary_len = *reinterpret_cast<uint32_t*>(recv_buff + pkg_len);
                recv_status = RecvStatus::RecvBinary;
                pkg_len += 4;
            break;

            case RecvStatus::RecvBinary:
                while (binary_len) {
                    std::size_t recv_len = io_stream->read(recv_buff + pkg_len, binary_len, timeout);
                    pkg_len += recv_len;
                    binary_len -= recv_len;
                }
                recv_status = RecvStatus::RecvCrc;
            break;

            case RecvStatus::RecvCrc:
                io_stream->read(recv_buff + pkg_len, 1, timeout);
                recv_status = RecvStatus::RecvDone;
                ++pkg_len;
            break;
        }
    }

    recv_bytes += pkg_len;
    std::size_t data_len = decode(buff, buff_len, recv_buff, pkg_len);
    delete recv_buff;
    return data_len;
}

/*
 * @brief 检查数据长度，不能超过最大能够发送的长度
 */
bool BinaryTransfer::check_binary_len(std::size_t len)
{
    if (len > max_transfer_binary_size) {
        throw std::runtime_error{"[SimpleBinaryTransfer::check_binary_len] binary_len:" + std::to_string(len) + " > max_transfer_binary_size:" + std::to_string(max_transfer_binary_size)};
    }
    return true;
}

/*
 * @brief 检查状态机接收状态
 * @return true 已接收完一个数据包
 */
bool BinaryTransfer::check_recv_status()
{
    if (recv_status == RecvStatus::RecvDone) return true;
    else return false;
}

/*
 * @brief 将数据编码成二进制数据包
 */
std::size_t BinaryTransfer::encode(uint8_t* buff, uint8_t* data, std::size_t len)
{
    check_binary_len(len);
    buff[0] = 0xAA;
    buff[1] = 0xAA;
    buff[2] = (uint8_t)len & 0xff;
    buff[3] = (uint8_t)(len >> 8) & 0xff;
    buff[4] = (uint8_t)(len >> 16) & 0xff;
    buff[5] = (uint8_t)(len >> 24) & 0xff;
    memcpy(buff + 6, data, len);
    std::size_t pkg_len = 6 + len;
    crc8_calculator->process_bytes(buff, pkg_len);
    buff[pkg_len++] = crc8_calculator->checksum();
    return pkg_len;
}

/*
 * @brief 将二进制数据包解码成数据
 */
std::size_t BinaryTransfer::decode(uint8_t* buff, std::size_t buff_len, uint8_t* pkg, std::size_t pkg_len)
{
    if (pkg_len < 7 || pkg_len > max_transfer_binary_size) return 0;

    crc8_calculator->process_bytes(pkg, pkg_len - 1);
    uint8_t crc8 = crc8_calculator->checksum();

    if (crc8 != pkg[pkg_len - 1]) {
        ++crc_error_cnt;
        return 0;
    }

    if (pkg_len - 7 > buff_len) return 0;

    memcpy(buff, pkg + 6, pkg_len - 7);

    return pkg_len - 7;
}
