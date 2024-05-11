/*
 * @author BusyBox
 * @date 2024/4/25
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include "rdk/core/transfer/simple_binary_transfer.h"
#include "boost/asio.hpp"

class ReliableBinaryTransfer : private SimpleBinaryTransfer
{
public:
    ReliableBinaryTransfer(std::shared_ptr<IOStream> io_stream);
    ~ReliableBinaryTransfer();

    /*
     * @brief 获取已发送的字节数
     */
    std::size_t get_send_bytes();
    /*
     * @brief 获取接收到的字节数
     */
    std::size_t get_recv_bytes();
    /*
     * @brief 清除已发送的字节数
     */
    void clear_send_bytes();
    /*
     * @brief 清除接收到的字节数
     */
    void clear_recv_bytes();
    /*
     * @brief 获取最大能够传输数据的长度
     */
    std::size_t get_max_transfer_binary_size();
    /*
     * @brief 获取crc错误计数
     */
    std::size_t get_crc_error_cnt();

    /*
     * @brief 发送二进制数据
     * @param data 数据
     * @param len 数据长度
     * @return 实际发送的字节数，为0表示发送失败
     */
    std::size_t send_binary(uint8_t* data, std::size_t len);
    /*
     * @brief 接收二进制数据
     * @param buff 接收缓冲区
     * @param buff_len 缓冲区长度
     * @param timeout 超时时间（毫秒）
     * @return 实际接收到的字节数
     */
    std::size_t recv_binary(uint8_t* buff, std::size_t buff_len, int timeout = 3000);

    /*
     * @brief 设置最大发送重试次数
     * @param max_try_cnt 最大重试次数
     */
    void set_max_try_cnt(int max_try_cnt);
    /*
     * @brief 设置接收ACK超时时间
     * @param ack_timeout ACK超时时间（毫秒）
     */
    void set_ack_timeout(int ack_timeout);

private:
    uint8_t seq = 0;
    uint8_t recv_seq = 255;
    int max_try_cnt = 3;
    int ack_timeout = 20;

    uint8_t control_block_send_buff[300];
    uint8_t control_block_recv_buff[300];
    uint8_t control_block_recv_ack_buff[32];

    boost::asio::io_context ioc;
    std::shared_ptr<boost::asio::high_resolution_timer> timer;

    enum class ControlCommand : uint8_t
    {
        Data = 0,
        ACK = 1,
    };

    #pragma pack(push, 1)
    struct ControlBlock
    {
        uint8_t seq;
        ControlCommand cm;
        uint8_t data_len;
        uint8_t data[0];
    };
    #pragma pack(pop)

    /*
     * @brief 尝试发送一次二进制数据
     * @param data 数据
     * @param len 数据长度
     * @return 实际发送的字节数，为0表示发送失败
     */
    std::size_t send_binary_once(uint8_t* data, std::size_t len);
    /*
     * @brief 接收二进制数据
     * @param buff 接收缓冲区
     * @param buff_len 缓冲区长度
     * @param timeout 超时时间（毫秒）
     * @param recv_duplicated_flag 接收到重复包标志 
     * @return 实际接收到的字节数
     */
    std::size_t recv_binary_once(uint8_t* buff, std::size_t buff_len, int* recv_duplicated_flag, int timeout = 3000);
};
