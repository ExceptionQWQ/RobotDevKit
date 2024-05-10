/*
 * @author BusyBox
 * @date 2024/5/10
 * @version 1.0
 */

#pragma once


#include <cstdint>
#include <thread>
#include <mutex>
#include <queue>
#include <boost/asio.hpp>

class VofaTCPServer
{
public:
    VofaTCPServer(int port);
    ~VofaTCPServer();

    std::queue<std::string> msg_queue;
    std::mutex println_lock;
    std::shared_ptr<boost::asio::ip::tcp::socket> client_socket;

    void start_listen();

    /*
     * @brief 将消息发送到vofa客户端，末尾自动加换行符
     * @param msg 格式 plot0:x,x,x
     */
    void println(std::string msg);
    
    
private:
    int port;
    std::shared_ptr<boost::asio::io_context> ioc;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor;
    std::thread listen_thread;
    std::thread send_thread;
    std::size_t max_queue_size = 1024;
};
