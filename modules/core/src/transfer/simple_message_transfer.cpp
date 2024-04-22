/*
 * @author BusyBox
 * @date 2024/4/22
 * @version 1.0
 */

#include "rdk/core/transfer/simple_message_transfer.h"

SimpleMessageTransfer::SimpleMessageTransfer(std::shared_ptr<IOStream> io_stream)
{
    this->io_stream = io_stream;
    crc8_calculator = std::make_shared<boost::crc_optimal<8, 0x31, 0, 0, false, false>>();
}

SimpleMessageTransfer::~SimpleMessageTransfer()
{

}

std::size_t SimpleMessageTransfer::get_send_bytes()
{
    return send_bytes;
}

std::size_t SimpleMessageTransfer::get_recv_bytes()
{
    return recv_bytes;
}

void SimpleMessageTransfer::clear_send_bytes()
{
    send_bytes = 0;
}

void SimpleMessageTransfer::clear_recv_bytes()
{
    recv_bytes = 0;
}

std::size_t SimpleMessageTransfer::get_max_transfer_msg_size()
{
    return max_transfer_msg_size;
}

std::size_t SimpleMessageTransfer::get_crc_error_cnt()
{
    return crc_error_cnt;
}

void SimpleMessageTransfer::send_message(const std::string& msg)
{
    std::size_t pkg_len = encode(msg);
    io_stream->write(send_buff, pkg_len);
}

std::string SimpleMessageTransfer::recv_message(int timeout)
{
    std::size_t pkg_len = 0;
    std::size_t msg_len = 0;
    recv_status = RecvStatus::RecvFirstFrameStart;
    while (true) {
        if (check_recv_status()) break;
        switch (recv_status) {
            case RecvStatus::RecvFirstFrameStart:
                io_stream->read(recv_buff + pkg_len, 1);
                if (recv_buff[pkg_len] == 0xAA) {
                    recv_status = RecvStatus::RecvSecondFrameStart;
                    ++pkg_len;
                } else {
                    pkg_len = 0;
                }
            break;

            case RecvStatus::RecvSecondFrameStart:
                io_stream->read(recv_buff + pkg_len, 1);
                if (recv_buff[pkg_len] == 0xAA) {
                    recv_status = RecvStatus::RecvMessageLen;
                    ++pkg_len;
                } else {
                    pkg_len = 0;
                }
            break;

            case RecvStatus::RecvMessageLen:
                io_stream->read(recv_buff + pkg_len, 1);
                msg_len = recv_buff[pkg_len];
                recv_status = RecvStatus::RecvMessage;
                ++pkg_len;
            break;

            case RecvStatus::RecvMessage:
                while (msg_len) {
                    std::size_t recv_len = io_stream->read(recv_buff + pkg_len, msg_len);
                    pkg_len += recv_len;
                    msg_len -= recv_len;
                }
                recv_status = RecvStatus::RecvCrc;
            break;

            case RecvStatus::RecvCrc:
                io_stream->read(recv_buff + pkg_len, 1);
                recv_status = RecvStatus::RecvDone;
                ++pkg_len;
            break;
        }
    }
    return decode(pkg_len);
}

std::size_t SimpleMessageTransfer::check_message_len(const std::string& msg)
{
    std::size_t msg_len = strlen(msg.data());
    if (msg_len > max_transfer_msg_size) {
        throw std::runtime_error{"[SimpleMessageTransfer::check_message_len] msg:\"" + msg + "\" msg_len:" + std::to_string(msg_len) + " > max_transfer_msg_size:" + std::to_string(max_transfer_msg_size)};
    }
    return msg_len;
}

bool SimpleMessageTransfer::check_recv_status()
{
    if (recv_status == RecvStatus::RecvDone) return true;
    else return false;
}

std::size_t SimpleMessageTransfer::encode(const std::string& msg)
{
    std::size_t msg_len = check_message_len(msg);
    send_buff[0] = 0xAA;
    send_buff[1] = 0xAA;
    send_buff[2] = (char)msg_len;
    memcpy(send_buff + 3, msg.data(), msg_len);
    std::size_t pkg_len = 3 + msg_len;
    crc8_calculator->process_bytes(send_buff, pkg_len);
    send_buff[pkg_len++] = crc8_calculator->checksum();
    return pkg_len;
}

std::string SimpleMessageTransfer::decode(std::size_t pkg_len)
{
    crc8_calculator->process_bytes(recv_buff, pkg_len - 1);
    char crc8 = crc8_calculator->checksum();

    if (crc8 != recv_buff[pkg_len - 1]) {
        ++crc_error_cnt;
        return std::string();
    }

    return std::string(recv_buff + 2, pkg_len - 4);
}
