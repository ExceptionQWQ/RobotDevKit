/*
 * @author BusyBox
 * @date 2024/4/24
 * @version 1.0
 */

#include "rdk/core/transfer/simple_message_transfer.h"


SimpleMessageTransfer::SimpleMessageTransfer(std::shared_ptr<IOStream> io_stream)
{
    this->io_stream = io_stream;
    crc8_calculator = std::make_shared<Crc8Calculator>(0x31);
}

SimpleMessageTransfer::~SimpleMessageTransfer()
{

}

/*
 * @brief 获取已发送的字节数
 */
std::size_t SimpleMessageTransfer::get_send_bytes()
{
    return send_bytes;
}

/*
 * @brief 获取接收到的字节数
 */
std::size_t SimpleMessageTransfer::get_recv_bytes()
{
    return recv_bytes;
}

/*
 * @brief 清除已发送的字节数
 */
void SimpleMessageTransfer::clear_send_bytes()
{
    send_bytes = 0;
}

/*
 * @brief 清除接收到的字节数
 */
void SimpleMessageTransfer::clear_recv_bytes()
{
    recv_bytes = 0;
}

/*
 * @brief 获取最大能够传输消息的长度
 */
std::size_t SimpleMessageTransfer::get_max_transfer_msg_size()
{
    return max_transfer_msg_size;
}

/*
 * @brief 获取crc错误计数
 */
std::size_t SimpleMessageTransfer::get_crc_error_cnt()
{
    return crc_error_cnt;
}

/*
 * @brief 发送消息
 * @param msg 消息
 */
void SimpleMessageTransfer::send_message(const std::string& msg)
{
    memset(send_buff, 0, 260);
    std::size_t pkg_len = encode(msg);
    io_stream->write(send_buff, pkg_len);
    send_bytes += pkg_len;
}

/*
 * @brief 接收消息
 * @param timeout 超时时间（毫秒）
 * @return 消息
 */
std::string SimpleMessageTransfer::recv_message(int timeout)
{
    memset(recv_buff, 0, 260);
    std::size_t pkg_len = 0;
    std::size_t msg_len = 0;
    uint32_t tick = HAL_GetTick();
    recv_status = RecvStatus::RecvFirstFrameStart;
    while (true) {
        if (check_recv_status()) break;
        if (HAL_GetTick() - tick > (uint32_t)timeout) { //超时
            return std::string();
        }
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
                    recv_status = RecvStatus::RecvMessageLen;
                    ++pkg_len;
                } else {
                    pkg_len = 0;
                }
                break;

            case RecvStatus::RecvMessageLen:
                io_stream->read(recv_buff + pkg_len, 1, timeout);
                msg_len = recv_buff[pkg_len];
                recv_status = RecvStatus::RecvMessage;
                ++pkg_len;
                break;

            case RecvStatus::RecvMessage:
                while (msg_len) {
                    std::size_t recv_len = io_stream->read(recv_buff + pkg_len, msg_len, timeout);
                    pkg_len += recv_len;
                    msg_len -= recv_len;
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
    return decode(pkg_len);
}

/*
 * @brief 检查消息长度，不能超过最大能够发送的长度
 */
std::size_t SimpleMessageTransfer::check_message_len(const std::string& msg)
{
    std::size_t msg_len = strlen(msg.data());
    if (msg_len > max_transfer_msg_size) {
        throw std::runtime_error{"[SimpleMessageTransfer::check_message_len] msg:\"" + msg + "\" msg_len:" + std::to_string(msg_len) + " > max_transfer_msg_size:" + std::to_string(max_transfer_msg_size)};
    }
    return msg_len;
}

/*
 * @brief 检查状态机接收状态
 * @return true 已接收完一个数据包
 */
bool SimpleMessageTransfer::check_recv_status()
{
    if (recv_status == RecvStatus::RecvDone) return true;
    else return false;
}

/*
 * @brief 将消息编码成二进制数据包
 */
std::size_t SimpleMessageTransfer::encode(const std::string& msg)
{
    std::size_t msg_len = check_message_len(msg);
    send_buff[0] = 0xAA;
    send_buff[1] = 0xAA;
    send_buff[2] = (char)msg_len;
    memcpy(send_buff + 3, msg.data(), msg_len);
    std::size_t pkg_len = 3 + msg_len;
    crc8_calculator->process_bytes((uint8_t*)send_buff, pkg_len);
    send_buff[pkg_len++] = crc8_calculator->checksum();
    return pkg_len;
}

/*
 * @brief 将二进制数据包解码成消息
 */
std::string SimpleMessageTransfer::decode(std::size_t pkg_len)
{
    if (pkg_len < 4 || pkg_len > 260) return std::string();

    crc8_calculator->process_bytes((uint8_t*)recv_buff, pkg_len - 1);
    char crc8 = crc8_calculator->checksum();

    if (crc8 != recv_buff[pkg_len - 1]) {
        ++crc_error_cnt;
        return std::string();
    }

    recv_buff[pkg_len - 1] = 0;

    return std::string(recv_buff + 3, pkg_len - 4);
}
