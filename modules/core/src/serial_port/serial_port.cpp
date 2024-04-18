/*
 * @author BusyBox
 * @date 2024/4/18
 * @version 1.0
 */

#include "rdk/core/serial_port/serial_port.hpp"

SerialPort::SerialPort(const std::string& serial_dev_path, int baud_rate)
{
    open(serial_dev_path, baud_rate);
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

std::size_t SerialPort::write(char* data, int len)
{
    return boost::asio::write(*serial, boost::asio::buffer(data, len), boost::asio::transfer_all());
}

void SerialPort::set_default_option()
{
    serial->set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
    serial->set_option(boost::asio::serial_port::flow_control(boost::asio::serial_port::flow_control::none));
    serial->set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
    serial->set_option(boost::asio::serial_port::character_size(8));
}