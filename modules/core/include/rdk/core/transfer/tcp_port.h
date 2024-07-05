/*
 * @author BusyBox
 * @date 2024/7/5
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <string>
#include <memory>
#include <optional>
#include <boost/asio.hpp>
#include "rdk/core/transfer/io_stream.h"


class TCPPort : public IOStream
{
public:
    TCPPort();
    TCPPort(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    ~TCPPort();

    /*
     * @brief 作为客户端连接服务器
     * @param server_ip 服务器IP
     * @param server_port 服务器端口
     */
    bool connect(const std::string& server_ip, int server_port);
    /*
     * @brief 关闭连接
     */
    void close();
    /*
     * @brief 设置服务端端口，并在此端口上监听，接收客户端的连接请求
     * @param port 端口
     */
    void set_port(int port);
    /*
     * @brief 接收客户端的连接请求
     * @return 返回能够与客户端通信的TCPPort
     */
    std::shared_ptr<TCPPort> accept();


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
    int port;
    std::shared_ptr<boost::asio::io_context> ioc;
    std::shared_ptr<boost::asio::high_resolution_timer> timer;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;

};