/*
 * @author BusyBox
 * @date 2024/4/18
 * @version 1.0
 */

#include <boost/asio.hpp>
#include <string>
#include <memory>

class SerialPort
{
public:
    SerialPort(const std::string& serial_dev_path, int baud_rate);
    ~SerialPort();

    void open(const std::string& serial_dev_path, int baud_rate);
    void close();

    std::size_t write(char* data, int len);


private:
    boost::asio::io_context ioc;
    std::shared_ptr<boost::asio::serial_port> serial;

    void set_default_option();

};