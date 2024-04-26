/*
 * @author BusyBox
 * @date 2024/4/25
 * @version 1.0
 */

#pragma once

#include <memory>
#include <string>
#include <cstring>
#include <optional>
#include <stdexcept>
#include "rdk/core/transfer/crc.h"
#include "rdk/core/transfer/io_stream.h"
#include "stm32f4xx_hal.h"

class SimpleBinaryTransfer
{
public:
    SimpleBinaryTransfer(std::shared_ptr<IOStream> io_stream);
    ~SimpleBinaryTransfer();

    std::size_t get_send_bytes();
    std::size_t get_recv_bytes();
    void clear_send_bytes();
    void clear_recv_bytes();
    std::size_t get_max_transfer_binary_size();
    std::size_t get_crc_error_cnt();

    void send_binary(uint8_t* data, std::size_t len);
    std::size_t recv_binary(uint8_t* buff, std::size_t buff_len, int timeout = 3000);

private:
    std::shared_ptr<IOStream> io_stream;
    std::shared_ptr<Crc8Calculator> crc8_calculator;

    std::size_t send_bytes = 0;
    std::size_t recv_bytes = 0;
    std::size_t crc_error_cnt = 0;
    std::size_t max_transfer_binary_size = 256;

    uint8_t send_buff[260];
    uint8_t recv_buff[260];

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

    bool check_binary_len(std::size_t len);
    bool check_recv_status();
    std::size_t encode(uint8_t* data, std::size_t len);
    std::size_t decode(uint8_t* buff, std::size_t buff_len, std::size_t pkg_len);

};
