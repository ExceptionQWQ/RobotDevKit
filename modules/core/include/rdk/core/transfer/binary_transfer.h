/*
 * @author BusyBox
 * @date 2024/4/25
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

class BinaryTransfer
{
public:
    BinaryTransfer(std::shared_ptr<IOStream> io_stream);
    ~BinaryTransfer();

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
     */
    void send_binary(uint8_t* data, std::size_t len);
    /*
     * @brief 接收二进制数据
     * @param buff 接收缓冲区
     * @param buff_len 缓冲区长度
     * @param timeout 超时时间（毫秒）
     * @return 数据长度
     */
    std::size_t recv_binary(uint8_t* buff, std::size_t buff_len, int timeout = 3000);

private:
    std::shared_ptr<IOStream> io_stream;
    std::shared_ptr<Crc8Calculator> crc8_calculator;
    
    std::size_t send_bytes = 0;
    std::size_t recv_bytes = 0;
    std::size_t crc_error_cnt = 0;
    std::size_t max_transfer_binary_size = 1024 * 1024 * 256;

    enum class RecvStatus
    {
        RecvFirstFrameStart = 0,
        RecvSecondFrameStart = 1,
        RecvBinaryLen = 2,
        RecvBinary = 3,
        RecvCrc = 4,
        RecvDone = 5,
    };

    RecvStatus recv_status = RecvStatus::RecvFirstFrameStart;

    /*
     * @brief 检查数据长度，不能超过最大能够发送的长度
     */
    bool check_binary_len(std::size_t len);
    /*
     * @brief 检查状态机接收状态
     * @return true 已接收完一个数据包
     */
    bool check_recv_status();
    /*
     * @brief 将数据编码成二进制数据包
     */
    std::size_t encode(uint8_t* buff, uint8_t* data, std::size_t len);
    /*
     * @brief 将二进制数据包解码成数据
     */
    std::size_t decode(uint8_t* buff, std::size_t buff_len, uint8_t* pkg, std::size_t pkg_len);
};
