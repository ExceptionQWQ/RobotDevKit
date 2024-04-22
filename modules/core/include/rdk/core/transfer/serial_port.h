/*
 * @author BusyBox
 * @date 2024/4/18
 * @version 1.0
 */

#pragma once

#include <string>
#include <memory>
#include <boost/asio.hpp>
#include "rdk/core/transfer/io_stream.h"


class SerialPort : public IOStream
{
public:
    SerialPort(const std::string& serial_dev_path, int baud_rate);
    ~SerialPort();

    void open(const std::string& serial_dev_path, int baud_rate);
    void close();

    std::size_t write(char* data, int len) override;
    std::size_t read(char* buff, int size_to_read) override;


private:
    boost::asio::io_context ioc;
    std::shared_ptr<boost::asio::serial_port> serial;

    void set_default_option();

};