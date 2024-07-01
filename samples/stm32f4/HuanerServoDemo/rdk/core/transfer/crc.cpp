/*
 * @author BusyBox
 * @date 2024/4/24
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/transfer/crc.h"


Crc8Calculator::Crc8Calculator(uint8_t poly)
{
    this->poly = poly;
    create_crc_table();
}

Crc8Calculator::~Crc8Calculator()
{

}

void Crc8Calculator::process_bytes(uint8_t* data, std::size_t len)
{
    crc = 0x00;
    while (len--) {
        crc = crc_table[crc ^ *data++];
    }
}

uint8_t Crc8Calculator::checksum()
{
    return crc;
}

uint8_t Crc8Calculator::crc_high_first(uint8_t* data, std::size_t len)
{
    uint8_t crc = 0x00;
    while (len--) {
        crc ^= *data++;
        for (uint8_t i = 8; i > 0; --i) {
            if (crc & 0x80) crc = (crc << 1) ^ poly;
            else crc = (crc << 1);
        }
    }
    return crc;
}

void Crc8Calculator::create_crc_table()
{
    for (int i = 0; i <= 0xFF; ++i) {
        crc_table[i] = crc_high_first((uint8_t*)&i, 1);
    }
}