/*
 * @author BusyBox
 * @date 2024/4/22
 * @version 1.0
 */

#pragma once

#include <cstdint>
#include <string>
#include <functional>
#include <boost/system/error_code.hpp>

using ResultHandler = std::function<void(const boost::system::error_code& error, std::size_t)>;

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

    struct TimeOutException
    {
        std::string error;
    };

private:

};