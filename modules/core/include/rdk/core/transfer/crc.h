/*
 * @author BusyBox
 * @date 2024/4/24
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <cstdint>
#include <cstddef>

class Crc8Calculator
{
public:
    /*
     * @brief 以poly为多项式二进制构造crc8计算器
     * @details 例如 0*x7 + 0*x6 + 1*x5 + 0*x4 + 0*x3 + 1*x2 + 0*x1 + 1*x0 的poly = 0x25 = 00100101
     */
    Crc8Calculator(uint8_t poly);
    ~Crc8Calculator();

    /*
     * @brief 计算给定数据的crc
     * @param data 数据
     * @param len 数据长度
     */
    void process_bytes(uint8_t* data, std::size_t len);
    /*
     * @brief 获取crc计算结果
     * @return crc8
     */
    uint8_t checksum();

private:
    uint8_t poly;
    uint8_t crc;

    uint8_t crc_table[256];

    uint8_t crc_high_first(uint8_t* data, std::size_t len);
    void create_crc_table();
};