/*
 * @author BusyBox
 * @date 2024/4/22
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <memory>
#include <string>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <boost/asio.hpp>
#include "rdk/core/transfer/crc.h"
#include "rdk/core/transfer/io_stream.h"

class SimpleMessageTransfer
{
public:
    SimpleMessageTransfer(std::shared_ptr<IOStream> io_stream);
    ~SimpleMessageTransfer();

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
     * @brief 获取最大能够传输消息的长度
     */
    std::size_t get_max_transfer_msg_size();
    /*
     * @brief 获取crc错误计数
     */
    std::size_t get_crc_error_cnt();

    /*
     * @brief 发送消息
     * @param msg 消息
     */
    void send_message(const std::string& msg);
    /*
     * @brief 接收消息
     * @param timeout 超时时间（毫秒）
     * @return 消息
     */
    std::string recv_message(int timeout = 3000);

private:
    std::shared_ptr<IOStream> io_stream;
    std::shared_ptr<Crc8Calculator> crc8_calculator;
    
    std::size_t send_bytes = 0;
    std::size_t recv_bytes = 0;
    std::size_t crc_error_cnt = 0;
    std::size_t max_transfer_msg_size = 256;

    uint8_t send_buff[260];
    uint8_t recv_buff[260];

    enum class RecvStatus
    {
        RecvFirstFrameStart = 0,
        RecvSecondFrameStart = 1,
        RecvMessageLen = 2,
        RecvMessage = 3,
        RecvCrc = 4,
        RecvDone = 5,
    };

    RecvStatus recv_status = RecvStatus::RecvFirstFrameStart;

    /*
     * @brief 检查消息长度，不能超过最大能够发送的长度
     */
    std::size_t check_message_len(const std::string& msg);
    /*
     * @brief 检查状态机接收状态
     * @return true 已接收完一个数据包
     */
    bool check_recv_status();
    /*
     * @brief 将消息编码成二进制数据包
     */
    std::size_t encode(const std::string& msg);
    /*
     * @brief 将二进制数据包解码成消息
     */
    std::string decode(std::size_t pkg_len);
    

};
