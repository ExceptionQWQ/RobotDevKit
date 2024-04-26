/*
 * @author BusyBox
 * @date 2024/4/25
 * @version 1.0
 */

#include "rdk/core/transfer/reliable_binary_transfer.h"


ReliableBinaryTransfer::ReliableBinaryTransfer(std::shared_ptr<IOStream> io_stream):SimpleBinaryTransfer(io_stream)
{
    timer = std::make_shared<boost::asio::high_resolution_timer>(ioc);
}

ReliableBinaryTransfer::~ReliableBinaryTransfer()
{

}

/*
 * @brief 获取已发送的字节数
 */
std::size_t ReliableBinaryTransfer::get_send_bytes()
{
    return SimpleBinaryTransfer::get_send_bytes();
}

/*
 * @brief 获取接收到的字节数
 */
std::size_t ReliableBinaryTransfer::get_recv_bytes()
{
    return SimpleBinaryTransfer::get_recv_bytes();
}

/*
 * @brief 清除已发送的字节数
 */
void ReliableBinaryTransfer::clear_send_bytes()
{
    SimpleBinaryTransfer::clear_send_bytes();
}

/*
 * @brief 清除接收到的字节数
 */
void ReliableBinaryTransfer::clear_recv_bytes()
{
    SimpleBinaryTransfer::clear_recv_bytes();
}

/*
 * @brief 获取最大能够传输数据的长度
 */
std::size_t ReliableBinaryTransfer::get_max_transfer_binary_size()
{
    return SimpleBinaryTransfer::get_max_transfer_binary_size();
}

/*
 * @brief 获取crc错误计数
 */
std::size_t ReliableBinaryTransfer::get_crc_error_cnt()
{
    return SimpleBinaryTransfer::get_crc_error_cnt();
}

/*
 * @brief 发送二进制数据
 * @param data 数据
 * @param len 数据长度
 * @return 实际发送的字节数，为0表示发送失败
 */
std::size_t ReliableBinaryTransfer::send_binary(uint8_t* data, std::size_t len)
{
    std::size_t send_bytes;
    int try_cnt = 0;
    while (try_cnt < max_try_cnt) {
        try {
            send_bytes = send_binary_once(data, len);
        } catch (const IOStream::TimeOutException& e) {
        
        }
        if (send_bytes) {
            break;
        } else {
            ++try_cnt;
            // printf("try_cnt:%d\n", try_cnt);
        }
    }
    return send_bytes;
}

/*
 * @brief 接收二进制数据
 * @param buff 接收缓冲区
 * @param buff_len 缓冲区长度
 * @param timeout 超时时间（毫秒）
 * @return 实际接收到的字节数
 */
std::size_t ReliableBinaryTransfer::recv_binary(uint8_t* buff, std::size_t buff_len, int timeout)
{
    int recv_duplicated_flag = 0;
    std::size_t recv_bytes = 0;
    do {
        try {
            recv_bytes = recv_binary_once(buff, buff_len, &recv_duplicated_flag, timeout);
        } catch (const IOStream::TimeOutException& e) {
            
        }
    } while (recv_duplicated_flag);
    return recv_bytes;
}

/*
 * @brief 设置最大发送重试次数
 * @param max_try_cnt 最大重试次数
 */
void ReliableBinaryTransfer::set_max_try_cnt(int max_try_cnt)
{
    this->max_try_cnt = max_try_cnt;
}

/*
 * @brief 设置接收ACK超时时间
 * @param ack_timeout ACK超时时间（毫秒）
 */
void ReliableBinaryTransfer::set_ack_timeout(int ack_timeout)
{
    this->ack_timeout = ack_timeout;
}

/*
 * @brief 尝试发送一次二进制数据
 * @param data 数据
 * @param len 数据长度
 * @return 实际发送的字节数，为0表示发送失败
 */
std::size_t ReliableBinaryTransfer::send_binary_once(uint8_t* data, std::size_t len)
{
    memset(control_block_send_buff, 0, 300);
    ControlBlock* cb = (ControlBlock*)control_block_send_buff;
    cb->seq = seq++;
    cb->cm = ControlCommand::Data;
    cb->data_len = (uint8_t)len;
    memcpy(cb->data, data, len);
    SimpleBinaryTransfer::send_binary(control_block_send_buff, sizeof(ControlBlock) + len);
    for (int try_cnt = 0; try_cnt < max_try_cnt; ++try_cnt) {
        std::size_t recv_len = 0;
        try {
            recv_len = SimpleBinaryTransfer::recv_binary(control_block_recv_buff, 300, ack_timeout);
        } catch (const IOStream::TimeOutException& e) {
        
        }
        if (recv_len == 0) continue;
        ControlBlock* cb2 = (ControlBlock*)control_block_recv_buff;
        // if (cb2->cm == ControlCommand::ACK) {
        //     printf("seq:%d %d\n", cb->seq, cb2->seq);
        // } else {
        //     printf("seq2:%d %d\n", cb->seq, cb2->seq);
        // }
        if (cb->seq == cb2->seq && cb2->cm == ControlCommand::ACK) {
            SimpleBinaryTransfer::send_binary(control_block_recv_buff, recv_len);
            return len;
        }
    }
    return 0;
}

/*
 * @brief 接收二进制数据
 * @param buff 接收缓冲区
 * @param buff_len 缓冲区长度
 * @param timeout 超时时间（毫秒）
 * @param recv_duplicated_flag 接收到重复包标志 
 * @return 实际接收到的字节数
 */
std::size_t ReliableBinaryTransfer::recv_binary_once(uint8_t* buff, std::size_t buff_len, int* recv_duplicated_flag, int timeout)
{
    memset(control_block_recv_buff, 0, 300);
    ControlBlock* cb = (ControlBlock*)control_block_recv_buff;
    std::size_t recv_len = SimpleBinaryTransfer::recv_binary(control_block_recv_buff, 300, timeout);
    if (recv_len == 0) return 0;
    if (buff_len < cb->data_len) return 0;
    if (cb->seq == recv_seq) {
        *recv_duplicated_flag = 1;
        return 0; //丢弃重复数据包
    }
    recv_seq = cb->seq;
    *recv_duplicated_flag = 0;
    if (cb->cm == ControlCommand::ACK) { //丢弃ACK数据包
        return 0;
    }
    ControlBlock* cb2 = (ControlBlock*)control_block_send_buff;
    cb2->seq = cb->seq;
    cb2->cm = ControlCommand::ACK;
    SimpleBinaryTransfer::send_binary(control_block_send_buff, sizeof(ControlBlock));

    for (int try_cnt = 0; try_cnt < max_try_cnt; ++try_cnt) {
        std::size_t recv_len = 0;
        try {
            recv_len = SimpleBinaryTransfer::recv_binary(control_block_recv_ack_buff, 32, ack_timeout);
        } catch (const IOStream::TimeOutException& e) {
        
        }
        if (recv_len == 0) continue;
        ControlBlock* cb3 = (ControlBlock*)control_block_recv_ack_buff;
        if (cb2->seq == cb3->seq && cb3->cm == ControlCommand::ACK) {
            memcpy(buff, cb->data, cb->data_len);
            return cb->data_len;
        }
    }
    return 0;
}