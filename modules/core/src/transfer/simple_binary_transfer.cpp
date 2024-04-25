/*
 * @author BusyBox
 * @date 2024/4/25
 * @version 1.0
 */

#include "rdk/core/transfer/simple_binary_transfer.h"

SimpleBinaryTransfer::SimpleBinaryTransfer(std::shared_ptr<IOStream> io_stream)
{
    this->io_stream = io_stream;
    crc8_calculator = std::make_shared<Crc8Calculator>(0x31);
}

SimpleBinaryTransfer::~SimpleBinaryTransfer()
{

}

/*
 * @brief 获取已发送的字节数
 */
std::size_t SimpleBinaryTransfer::get_send_bytes()
{
    return send_bytes;
}

/*
 * @brief 获取接收到的字节数
 */
std::size_t SimpleBinaryTransfer::get_recv_bytes()
{
    return recv_bytes;
}

/*
 * @brief 清除已发送的字节数
 */
void SimpleBinaryTransfer::clear_send_bytes()
{
    send_bytes = 0;
}

/*
 * @brief 清除接收到的字节数
 */
void SimpleBinaryTransfer::clear_recv_bytes()
{
    recv_bytes = 0;
}

/*
 * @brief 获取最大能够传输数据的长度
 */
std::size_t SimpleBinaryTransfer::get_max_transfer_binary_size()
{
    return max_transfer_binary_size;
}

/*
 * @brief 获取crc错误计数
 */
std::size_t SimpleBinaryTransfer::get_crc_error_cnt()
{
    return crc_error_cnt;
}

/*
 * @brief 发送二进制数据
 * @param data 数据
 * @param len 数据长度
 */
void SimpleBinaryTransfer::send_binary(uint8_t* data, std::size_t len)
{
    memset(send_buff, 0, 260);
    std::size_t pkg_len = encode(data, len);
    io_stream->write(send_buff, pkg_len);
    send_bytes += pkg_len;
}

/*
 * @brief 接收二进制数据
 * @param buff 接收缓冲区
 * @param buff_len 缓冲区长度
 * @param timeout 超时时间（毫秒）
 * @return 数据长度
 */
std::size_t SimpleBinaryTransfer::recv_binary(uint8_t* buff, std::size_t buff_len, int timeout)
{
    memset(recv_buff, 0, 260);
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
                io_stream->read(recv_buff + pkg_len, 1, timeout);
                binary_len = recv_buff[pkg_len];
                recv_status = RecvStatus::RecvBinary;
                ++pkg_len;
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
    return decode(buff, buff_len, pkg_len);
}

/*
 * @brief 检查数据长度，不能超过最大能够发送的长度
 */
bool SimpleBinaryTransfer::check_binary_len(std::size_t len)
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
bool SimpleBinaryTransfer::check_recv_status()
{
    if (recv_status == RecvStatus::RecvDone) return true;
    else return false;
}

/*
 * @brief 将数据编码成二进制数据包
 */
std::size_t SimpleBinaryTransfer::encode(uint8_t* data, std::size_t len)
{
    check_binary_len(len);
    send_buff[0] = 0xAA;
    send_buff[1] = 0xAA;
    send_buff[2] = (char)len;
    memcpy(send_buff + 3, data, len);
    std::size_t pkg_len = 3 + len;
    crc8_calculator->process_bytes((uint8_t*)send_buff, pkg_len);
    send_buff[pkg_len++] = crc8_calculator->checksum();
    return pkg_len;
}

/*
 * @brief 将二进制数据包解码成数据
 */
std::size_t SimpleBinaryTransfer::decode(uint8_t* buff, std::size_t buff_len, std::size_t pkg_len)
{
    if (pkg_len < 4 || pkg_len > 260) return 0;

    crc8_calculator->process_bytes((uint8_t*)recv_buff, pkg_len - 1);
    char crc8 = crc8_calculator->checksum();

    if (crc8 != recv_buff[pkg_len - 1]) {
        ++crc_error_cnt;
        return 0;
    }

    if (pkg_len - 4 > buff_len) return 0;

    memcpy(buff, recv_buff + 3, pkg_len - 4);

    recv_buff[pkg_len - 1] = 0;

    return pkg_len - 4;
}
