/*
 * @author BusyBox
 * @date 2024/4/24
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <cstdint>


class CircularBuffer
{
public:
    CircularBuffer();
    ~CircularBuffer();

    void push_back(uint8_t t);
    uint8_t pop_front();
    bool full();
    bool available();


private:
    std::size_t buff_size = 1024;
    uint8_t buff[1024];
    volatile std::size_t sz = 0;
    volatile std::size_t head = 0;
    volatile std::size_t tail = 0;

};


