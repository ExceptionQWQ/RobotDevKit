/*
 * @author BusyBox
 * @date 2024/4/24
 * @version 1.0
 */

#pragma once

#include <cstdint>

class Crc8Calculator
{
public:
    Crc8Calculator(uint8_t poly);
    ~Crc8Calculator();

    void process_bytes(uint8_t* data, std::size_t len);
    uint8_t checksum();

private:
    uint8_t poly;
    uint8_t crc;

    uint8_t crc_table[256];

    uint8_t crc_high_first(uint8_t* data, std::size_t len);
    void create_crc_table();
};