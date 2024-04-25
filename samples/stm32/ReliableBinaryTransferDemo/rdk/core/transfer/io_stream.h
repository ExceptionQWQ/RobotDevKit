/*
 * @author BusyBox
 * @date 2024/4/23
 * @version 1.0
 */

#pragma once

#include <cstdint>
#include <string>
#include <functional>

using ResultHandler = std::function<void(std::size_t)>;

class IOStream
{
public:
    IOStream();
    ~IOStream();

    virtual void async_write(char* data, int len, ResultHandler result_handler) = 0;
    virtual void async_read(char* buff, int size_to_read, ResultHandler result_handler) = 0;
    virtual std::size_t write(char* data, int len) = 0;
    virtual std::size_t read(char* buff, int size_to_read) = 0;
    virtual std::size_t write(char* data, int len, int timeout) = 0;
    virtual std::size_t read(char* buff, int size_to_read, int timeout) = 0;

    class TimeOutException
    {
        std::string error;
    };

private:

};