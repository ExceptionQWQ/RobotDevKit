/*
 * @author BusyBox
 * @date 2024/4/18
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <string>
#include <memory>
#include <optional>
#include <boost/asio.hpp>
#include "rdk/core/transfer/io_stream.h"


class SerialPort : public IOStream
{
public:
    /*
     * @brief 串口创建时自动打开
     * @param serial_dev_path 串口地址
     * @param baud_rate 串口波特率
     * @attention 串口地址建议使用/dev/serial/by-path，这个为固定地址
     */
    SerialPort(const std::string& serial_dev_path, int baud_rate);
    ~SerialPort();

    /*
     * @brief 打开串口
     * @param serial_dev_path 串口地址
     * @param baud_rate 串口波特率
     */
    void open(const std::string& serial_dev_path, int baud_rate);
    /*
     * @brief 关闭串口
     */
    void close();

    /*
     * @brief 异步写入数据
     * @param data 要写入的数据
     * @param len 数据长度
     * @param result_handler 操作完成回调函数
     */
    void async_write(uint8_t* data, std::size_t len, ResultHandler result_handler) override;
    /*
     * @brief 异步读取数据，读取至少一个字节
     * @param buff 数据缓冲区
     * @param len 要读取的长度
     * @param result_handler 操作完成回调函数
     */
    void async_read(uint8_t* buff, std::size_t size_to_read, ResultHandler result_handler) override;

    /*
     * @brief 写入数据
     * @param data 要写入的数据
     * @param len 数据长度
     */
    std::size_t write(uint8_t* data, std::size_t len) override;
    /*
     * @brief 读取数据，读取至少一个字节
     * @param buff 数据缓冲区
     * @param len 要读取的长度
     * @return 读取到的字节数
     */
    std::size_t read(uint8_t* buff, std::size_t size_to_read) override;

    /*
     * @brief 写入数据，写入所有数据，直到超时返回
     * @param data 要写入的数据
     * @param len 数据长度
     * @param timeout 超时时间（ms）
     */
    std::size_t write(uint8_t* data, std::size_t len, int timeout) override;
    /*
     * @brief 读取数据，读取所有数据，直到超时返回
     * @param buff 数据缓冲区
     * @param len 要读取的长度
     * @param timeout 超时时间（ms）
     * @return 读取到的字节数
     */
    std::size_t read(uint8_t* buff, std::size_t size_to_read, int timeout) override;

private:
    boost::asio::io_context ioc;
    std::shared_ptr<boost::asio::serial_port> serial;
    std::shared_ptr<boost::asio::high_resolution_timer> timer;

    void set_default_option();

};