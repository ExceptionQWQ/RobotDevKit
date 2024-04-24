/*
 * @author BusyBox
 * @date 2024/4/24
 * @version 1.0
 */

#pragma once

#include <cstdint>
#include <string>

template<class T, std::size_t buff_size>
class CircularBuffer
{
public:
    CircularBuffer();
    ~CircularBuffer();

    void push_back(const T& t);
    T pop_front();
    bool full();
    bool available();

    struct BufferEmptyException
    {
        std::string error;
    };

private:
    T buff[buff_size];
    volatile std::size_t sz = 0;
    volatile std::size_t head = 0;
    volatile std::size_t tail = 0;

};


