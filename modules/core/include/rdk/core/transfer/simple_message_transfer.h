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

    std::size_t get_send_bytes();
    std::size_t get_recv_bytes();
    void clear_send_bytes();
    void clear_recv_bytes();
    std::size_t get_max_transfer_msg_size();
    std::size_t get_crc_error_cnt();

    void send_message(const std::string& msg);
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

    std::size_t check_message_len(const std::string& msg);
    bool check_recv_status();
    std::size_t encode(const std::string& msg);
    std::string decode(std::size_t pkg_len);
    

};
