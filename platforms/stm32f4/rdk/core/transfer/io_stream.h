/*
 * @author BusyBox
 * @date 2024/4/23
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
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

    virtual void async_write(uint8_t* data, std::size_t len, ResultHandler result_handler) = 0;
    virtual void async_read(uint8_t* buff, std::size_t size_to_read, ResultHandler result_handler) = 0;
    virtual std::size_t write(uint8_t* data, std::size_t len) = 0;
    virtual std::size_t read(uint8_t* buff, std::size_t size_to_read) = 0;
    virtual std::size_t write(uint8_t* data, std::size_t len, int timeout) = 0;
    virtual std::size_t read(uint8_t* buff, std::size_t size_to_read, int timeout) = 0;

    class TimeOutException
    {
        std::string error;
    };

private:

};