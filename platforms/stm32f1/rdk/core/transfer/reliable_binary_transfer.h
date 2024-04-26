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
#include "rdk/core/transfer/serial_port.h"
#include "rdk/core/transfer/simple_binary_transfer.h"
#include "stm32f1xx_hal.h"

class ReliableBinaryTransfer : private SimpleBinaryTransfer
{
public:
    ReliableBinaryTransfer(SerialPort* serial_port, Crc8Calculator* crc8_calculator);
    ~ReliableBinaryTransfer();

    std::size_t get_send_bytes();
    std::size_t get_recv_bytes();
    void clear_send_bytes();
    void clear_recv_bytes();
    std::size_t get_max_transfer_binary_size();
    std::size_t get_crc_error_cnt();

    std::size_t send_binary(uint8_t* data, std::size_t len);
    std::size_t recv_binary(uint8_t* buff, std::size_t buff_len, int timeout = 3000);

    void set_max_try_cnt(int max_try_cnt);
    void set_ack_timeout(int ack_timeout);

private:
    volatile uint8_t seq = 0;
    volatile uint8_t recv_seq = 255;
    int max_try_cnt = 3;
    int ack_timeout = 20;

    uint8_t control_block_send_buff[300];
    uint8_t control_block_recv_buff[300];
    uint8_t control_block_recv_ack_buff[32];

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

    std::size_t send_binary_once(uint8_t* data, std::size_t len);
    std::size_t recv_binary_once(uint8_t* buff, std::size_t buff_len, volatile int* recv_duplicated_flag, int timeout = 3000);

};
