/*
 * @author BusyBox
 * @date 2024/5/10
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/vofa/vofa_tcp_server.h"

VofaTCPServer::VofaTCPServer(int port)
{
    this->port = port;
    ioc = std::make_shared<boost::asio::io_context>();
    acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(*ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::any(), this->port));
    listen_thread = std::thread([](VofaTCPServer* server){
        server->start_listen();
    }, this);
    send_thread = std::thread([](VofaTCPServer* server){
        while (true) {
            server->println_lock.lock();
            if (server->msg_queue.size() == 0) {
                server->println_lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue; 
            }

            std::string msg = server->msg_queue.front();
            server->msg_queue.pop();
            try {
                if (server->client_socket) boost::asio::write(*server->client_socket, boost::asio::buffer(msg.data(), msg.size() + 1));
            } catch (const boost::system::system_error& error) {
        
            }

            server->println_lock.unlock();
        }
    }, this);
}

VofaTCPServer::~VofaTCPServer()
{

}

void VofaTCPServer::start_listen()
{
    while (true) {
        client_socket = std::make_shared<boost::asio::ip::tcp::socket>(acceptor->accept());
    }
}

void VofaTCPServer::println(std::string msg)
{
    println_lock.lock();
    msg += "\n";
    if (msg_queue.size() < max_queue_size) msg_queue.push(msg);
    println_lock.unlock();
}
