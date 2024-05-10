/*
 * @author BusyBox
 * @date 2024/4/28
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/transfer/serial_to_can.h"

SerialToCan::SerialToCan(std::shared_ptr<SerialPort> serial_port, IDType id_type):CanBus(id_type)
{
    this->serial_port = serial_port;
    if (id_type == IDType::STD) {
        frame_size = 8 + 2;
    } else if (id_type == IDType::EXT) {
        frame_size = 8 + 4;
    }
    timer = std::make_shared<boost::asio::high_resolution_timer>(ioc);
}

SerialToCan::~SerialToCan()
{

}

void SerialToCan::encode(uint8_t* buff, uint32_t id, FrameData data)
{
    if (id_type == IDType::STD) {
        buff[0] = (uint8_t)(id >> 8);
        buff[1] = (uint8_t)(id);
        buff[2] = data.d0;
        buff[3] = data.d1;
        buff[4] = data.d2;
        buff[5] = data.d3;
        buff[6] = data.d4;
        buff[7] = data.d5;
        buff[8] = data.d6;
        buff[9] = data.d7;
    } else if (id_type == IDType::EXT) {
        buff[0] = (uint8_t)(id >> 24);
        buff[1] = (uint8_t)(id >> 16);
        buff[2] = (uint8_t)(id >> 8);
        buff[3] = (uint8_t)(id);
        buff[4] = data.d0;
        buff[5] = data.d1;
        buff[6] = data.d2;
        buff[7] = data.d3;
        buff[8] = data.d4;
        buff[9] = data.d5;
        buff[10] = data.d6;
        buff[11] = data.d7;
    }
}

void SerialToCan::decode(uint8_t* buff, uint32_t* id, FrameData* data)
{
    if (id_type == IDType::STD) {
        *id = (uint32_t)(((uint32_t)buff[0] << 8) | ((uint32_t)buff[1]));
        data->d0 = buff[2];
        data->d1 = buff[3];
        data->d2 = buff[4];
        data->d3 = buff[5];
        data->d4 = buff[6];
        data->d5 = buff[7];
        data->d6 = buff[8];
        data->d7 = buff[9];
    } else if (id_type == IDType::EXT) {
        *id = (uint32_t)(((uint32_t)buff[0] << 24) | ((uint32_t)buff[1] << 16) | ((uint32_t)buff[2] << 8) | ((uint32_t)buff[3]));
        data->d0 = buff[4];
        data->d1 = buff[5];
        data->d2 = buff[6];
        data->d3 = buff[7];
        data->d4 = buff[8];
        data->d5 = buff[9];
        data->d6 = buff[10];
        data->d7 = buff[11];
    }   
}

void SerialToCan::async_write_frame(uint32_t id, FrameData data, ResultHandler result_handler)
{
    encode(send_buff, id, data);
    serial_port->async_write(send_buff, frame_size, std::bind([](uint32_t id, FrameData data, ResultHandler result_handler, const boost::system::error_code& error, std::size_t sz){
        if (result_handler) result_handler(error, id, data);
    }, id, data, result_handler, std::placeholders::_1, std::placeholders::_2));
}

void SerialToCan::async_read_frame(uint32_t* id, FrameData* data, ResultHandler result_handler)
{
    // TODO
}

void SerialToCan::write_frame(uint32_t id, FrameData data)
{
    encode(send_buff, id, data);
    serial_port->write(send_buff, frame_size);
}


void SerialToCan::read_frame(uint32_t* id, FrameData* data)
{
    std::size_t recv_bytes = 0, remain_bytes = frame_size;
    bool flag = true;
    while (remain_bytes) {
        std::size_t ret = serial_port->read(recv_buff + recv_bytes, remain_bytes);
        recv_bytes += ret;
        remain_bytes -= ret;

        //利用id范围进行帧头定位
        if (flag) {
            uint32_t id = 0;
            if (id_type == IDType::STD) {
                id = (uint32_t)(((uint32_t)recv_buff[0] << 8) | ((uint32_t)recv_buff[1]));
            } else if (id_type == IDType::EXT) {
                id = (uint32_t)(((uint32_t)recv_buff[0] << 24) | ((uint32_t)recv_buff[1] << 16) | ((uint32_t)recv_buff[2] << 8) | ((uint32_t)recv_buff[3]));
            }
            if (id >= 0x200 && id <= 0x210) {
                flag = false;
            } else {
                recv_bytes = 0;
                remain_bytes = frame_size;
            }
        }
    }
    // for (int i = 0; i < frame_size; ++i) {
    //     printf("%02x ", recv_buff[i]);
    // }
    // printf("\n");
    decode(recv_buff, id, data);
}

void SerialToCan::write_frame(uint32_t id, FrameData data, int timeout)
{
    encode(send_buff, id, data);
    
    std::optional<boost::system::error_code> timeout_result, write_result;
    std::size_t write_bytes = 0;

    timer->expires_from_now(std::chrono::milliseconds(timeout));
    timer->async_wait(std::bind([](std::optional<boost::system::error_code>* timeout_result, const boost::system::error_code& error){
        *timeout_result = error;
    }, &timeout_result, std::placeholders::_1));

    serial_port->async_write(send_buff, frame_size, std::bind([](std::optional<boost::system::error_code>* write_result, std::size_t* write_bytes_out, const boost::system::error_code& error, std::size_t write_bytes){
            *write_result = error;
            *write_bytes_out = write_bytes;
    }, &write_result, &write_bytes, std::placeholders::_1, std::placeholders::_2));


    ioc.reset();
    while (ioc.run_one()) {
        if (timeout_result) {
            break;
        } else if (write_result) {
            timer->cancel();
        }
    }
    if (timeout_result.value() != boost::asio::error::operation_aborted) {
        throw IOStream::TimeOutException{"[SerialPort:write] timeout"};
    }
}

void SerialToCan::read_frame(uint32_t* id, FrameData* data, int timeout)
{
    //TODO
}