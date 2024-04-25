/*
 * @author BusyBox
 * @date 2024/4/18
 * @version 1.0
 */

#include "rdk/core/transfer/serial_port.h"

SerialPort::SerialPort(const std::string& serial_dev_path, int baud_rate)
{
    open(serial_dev_path, baud_rate);
    timer = std::make_shared<boost::asio::high_resolution_timer>(ioc);
}

SerialPort::~SerialPort()
{

}

void SerialPort::open(const std::string& serial_dev_path, int baud_rate)
{
    if (!serial) serial = std::make_shared<boost::asio::serial_port>(ioc);
    serial->open(serial_dev_path);
    
    if (!serial->is_open()) {
        throw std::runtime_error{"[SerialPort::open()]"};
    }
    serial->set_option(boost::asio::serial_port::baud_rate(baud_rate));
    set_default_option();
}

void SerialPort::close()
{
    serial->close();
}

void SerialPort::async_write(uint8_t* data, std::size_t len, ResultHandler result_handler)
{
    boost::asio::async_write(*serial, boost::asio::buffer(data, len), boost::asio::transfer_all(), result_handler);
}

void SerialPort::async_read(uint8_t* buff, std::size_t size_to_read, ResultHandler result_handler)
{
    boost::asio::async_read(*serial, boost::asio::buffer(buff, size_to_read), boost::asio::transfer_at_least(1), result_handler);
}

std::size_t SerialPort::write(uint8_t* data, std::size_t len)
{
    return boost::asio::write(*serial, boost::asio::buffer(data, len), boost::asio::transfer_at_least(1));
}

std::size_t SerialPort::read(uint8_t* buff, std::size_t size_to_read)
{
    return boost::asio::read(*serial, boost::asio::buffer(buff, size_to_read), boost::asio::transfer_at_least(1));
}

std::size_t SerialPort::write(uint8_t* data, std::size_t len, int timeout)
{
    std::optional<boost::system::error_code> timeout_result, write_result;
    std::size_t write_bytes = 0;

    timer->expires_from_now(std::chrono::milliseconds(timeout));
    timer->async_wait(std::bind([](std::optional<boost::system::error_code>* timeout_result, const boost::system::error_code& error){
        *timeout_result = error;
    }, &timeout_result, std::placeholders::_1));
    boost::asio::async_write(*serial, boost::asio::buffer(data, len), boost::asio::transfer_all(), 
        std::bind([](std::optional<boost::system::error_code>* write_result, std::size_t* write_bytes_out, const boost::system::error_code& error, std::size_t write_bytes){
            *write_result = error;
            *write_bytes_out = write_bytes;
    }, &write_result, &write_bytes, std::placeholders::_1, std::placeholders::_2));

    ioc.reset();
    while (ioc.run_one()) {
        if (timeout_result) {
            serial->cancel();
        } else if (write_result) {
            timer->cancel();
        }
    }
    if (timeout_result.value() != boost::asio::error::operation_aborted) {
        throw IOStream::TimeOutException{"[SerialPort:write] timeout"};
    }
    return write_bytes;
}

std::size_t SerialPort::read(uint8_t* buff, std::size_t size_to_read, int timeout)
{
    std::optional<boost::system::error_code> timeout_result, read_result;
    std::size_t read_bytes = 0;

    timer->expires_from_now(std::chrono::milliseconds(timeout));
    timer->async_wait(std::bind([](std::optional<boost::system::error_code>* timeout_result, const boost::system::error_code& error){
        *timeout_result = error;
    }, &timeout_result, std::placeholders::_1));
    boost::asio::async_read(*serial, boost::asio::buffer(buff, size_to_read), boost::asio::transfer_at_least(1),
        std::bind([](std::optional<boost::system::error_code>* read_result, std::size_t* read_bytes_out, const boost::system::error_code& error, std::size_t read_bytes){
            *read_result = error;
            *read_bytes_out = read_bytes;
        }, &read_result, &read_bytes, std::placeholders::_1, std::placeholders::_2));

    ioc.reset();
    while (ioc.run_one()) {
        if (timeout_result) {
            serial->cancel();
        } else if (read_result) {
            timer->cancel();
        }
    }
    if (timeout_result.value() != boost::asio::error::operation_aborted) {
        throw IOStream::TimeOutException{"[SerialPort:read] timeout"};
    }
    return read_bytes;
}

void SerialPort::set_default_option()
{
    serial->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
    serial->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
    serial->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
    serial->set_option(boost::asio::serial_port::character_size(8));
}