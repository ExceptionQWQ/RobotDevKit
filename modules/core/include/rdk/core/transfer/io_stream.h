/*
 * @author BusyBox
 * @date 2024/4/22
 * @version 1.0
 */

#pragma once

#include <cstdint>

class IOStream
{
public:
    IOStream();
    ~IOStream();

    virtual std::size_t write(char* data, int len) = 0;
    virtual std::size_t read(char* buff, int size_to_read) = 0;

private:

};