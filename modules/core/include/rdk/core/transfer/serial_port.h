/*
 * @author BusyBox
 * @date 2024/4/18
 * @version 1.0
 */

#pragma once

#include <string>
#include <memory>
#include <optional>
#include <boost/asio.hpp>
#include "rdk/core/transfer/io_stream.h"


class SerialPort : public IOStream
{
public:
    SerialPort(const std::string& serial_dev_path, int baud_rate);
    ~SerialPort();

    void open(const std::string& serial_dev_path, int baud_rate);
    void close();

    void async_write(uint8_t* data, std::size_t len, ResultHandler result_handler) override;
    void async_read(uint8_t* buff, std::size_t size_to_read, ResultHandler result_handler) override;

    std::size_t write(uint8_t* data, std::size_t len) override;
    std::size_t read(uint8_t* buff, std::size_t size_to_read) override;

    std::size_t write(uint8_t* data, std::size_t len, int timeout) override;
    std::size_t read(uint8_t* buff, std::size_t size_to_read, int timeout) override;

private:
    boost::asio::io_context ioc;
    std::shared_ptr<boost::asio::serial_port> serial;
    std::shared_ptr<boost::asio::high_resolution_timer> timer;

    void set_default_option();

};