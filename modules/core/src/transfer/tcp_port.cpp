/*
 * @author BusyBox
 * @date 2024/7/5
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/transfer/tcp_port.h"


TCPPort::TCPPort()
{
    ioc = std::make_shared<boost::asio::io_context>();
    timer = std::make_shared<boost::asio::high_resolution_timer>(*ioc);
}

TCPPort::TCPPort(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    ioc = std::make_shared<boost::asio::io_context>();
    timer = std::make_shared<boost::asio::high_resolution_timer>(*ioc);
    this->socket = socket;
}

TCPPort::~TCPPort()
{

}

/*
 * @brief 作为客户端连接服务器
 * @param server_ip 服务器IP
 * @param server_port 服务器端口
 */
bool TCPPort::connect(const std::string& server_ip, int server_port)
{
    boost::asio::ip::tcp::resolver resolver(*ioc);
    auto end_point = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(server_ip), server_port);
    socket = std::make_shared<boost::asio::ip::tcp::socket>(*ioc);
    socket->connect(end_point);
    return socket->is_open();
}

/*
 * @brief 关闭连接
 */
void TCPPort::close()
{
    socket->close();
}

/*
 * @brief 设置服务端端口
 * @param port 端口
 */
void TCPPort::set_port(int port)
{
    this->port = port;
    acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), this->port));
}

/*
 * @brief 接收客户端的连接请求
 * @return 返回能够与客户端通信的TCPPort
 */
std::shared_ptr<TCPPort> TCPPort::accept()
{
    std::shared_ptr<TCPPort> new_port = std::make_shared<TCPPort>(std::make_shared<boost::asio::ip::tcp::socket>(acceptor->accept()));
    return new_port;
}

/*
 * @brief 异步写入数据
 * @param data 要写入的数据
 * @param len 数据长度
 * @param result_handler 操作完成回调函数
 */
void TCPPort::async_write(uint8_t* data, std::size_t len, ResultHandler result_handler)
{
    boost::asio::async_write(*socket, boost::asio::buffer(data, len), boost::asio::transfer_all(), result_handler);
}

/*
 * @brief 异步读取数据，读取至少一个字节
 * @param buff 数据缓冲区
 * @param len 要读取的长度
 * @param result_handler 操作完成回调函数
 */
void TCPPort::async_read(uint8_t* buff, std::size_t size_to_read, ResultHandler result_handler)
{
    boost::asio::async_read(*socket, boost::asio::buffer(buff, size_to_read), boost::asio::transfer_at_least(1), result_handler);
}

/*
 * @brief 写入数据
 * @param data 要写入的数据
 * @param len 数据长度
 */
std::size_t TCPPort::write(uint8_t* data, std::size_t len)
{
    return boost::asio::write(*socket, boost::asio::buffer(data, len), boost::asio::transfer_all());
}

/*
 * @brief 读取数据，读取至少一个字节
 * @param buff 数据缓冲区
 * @param len 要读取的长度
 * @return 读取到的字节数
 */
std::size_t TCPPort::read(uint8_t* buff, std::size_t size_to_read)
{
    return boost::asio::read(*socket, boost::asio::buffer(buff, size_to_read), boost::asio::transfer_at_least(1));
}

/*
 * @brief 写入数据，写入所有数据，直到超时返回
 * @param data 要写入的数据
 * @param len 数据长度
 * @param timeout 超时时间（ms）
 */
std::size_t TCPPort::write(uint8_t* data, std::size_t len, int timeout)
{
    std::optional<boost::system::error_code> timeout_result, write_result;
    std::size_t write_bytes = 0;

    timer->expires_from_now(std::chrono::milliseconds(timeout));
    timer->async_wait(std::bind([](std::optional<boost::system::error_code>* timeout_result, const boost::system::error_code& error){
        *timeout_result = error;
    }, &timeout_result, std::placeholders::_1));
    boost::asio::async_write(*socket, boost::asio::buffer(data, len), boost::asio::transfer_all(), 
        std::bind([](std::optional<boost::system::error_code>* write_result, std::size_t* write_bytes_out, const boost::system::error_code& error, std::size_t write_bytes){
            *write_result = error;
            *write_bytes_out = write_bytes;
    }, &write_result, &write_bytes, std::placeholders::_1, std::placeholders::_2));

    ioc->reset();
    while (ioc->run_one()) {
        if (timeout_result) {
            socket->cancel();
        } else if (write_result) {
            timer->cancel();
        }
    }
    if (timeout_result.value() != boost::asio::error::operation_aborted) {
        throw IOStream::TimeOutException{"[SerialPort:write] timeout"};
    }
    return write_bytes;
}

/*
 * @brief 读取数据，读取所有数据，直到超时返回
 * @param buff 数据缓冲区
 * @param len 要读取的长度
 * @param timeout 超时时间（ms）
 * @return 读取到的字节数
 */
std::size_t TCPPort::read(uint8_t* buff, std::size_t size_to_read, int timeout)
{
    std::size_t remain_size = size_to_read;
    std::size_t read_size = 0;

    int cnt = 0;
    while (remain_size) {
        if (!socket->available()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            ++cnt;
            if (cnt > timeout) return read_size;
            continue;
        }
        std::size_t read_bytes = read(buff + read_size, remain_size);
        read_size += read_bytes;
        remain_size -= read_bytes;
    }
    return read_size;
}
