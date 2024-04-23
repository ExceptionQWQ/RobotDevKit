/*
 * @author BusyBox
 * @date 2024/4/13
 * @version 1.0
 */


#include "rdk/core/transfer/serial_port.h"

SerialPort::SerialPort(UART_HandleTypeDef* huart)
{
    this->huart = huart;
//    __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
}

SerialPort::~SerialPort()
{

}

void SerialPort::OnHAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size)
{
    if (huart->Instance != this->huart->Instance) return ;
    read_result_handler(size);
}

void SerialPort::OnHAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance != this->huart->Instance) return ;
    write_result_handler(huart->TxXferSize);
}

void SerialPort::OnHAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance != this->huart->Instance) return ;
    __HAL_UART_CLEAR_OREFLAG(huart);
}

void SerialPort::async_write(char* data, int len, ResultHandler result_handler)
{
    write_result_handler = result_handler;
    HAL_UART_Transmit_DMA(huart, (uint8_t*)data, len);
}

void SerialPort::async_read(char* buff, int size_to_read, ResultHandler result_handler)
{
    read_result_handler = result_handler;
    HAL_UARTEx_ReceiveToIdle_DMA(huart, (uint8_t*)buff, size_to_read);
}

std::size_t SerialPort::write(char* data, int len)
{
    write(data, len, 3000);
    return len;
}

std::size_t SerialPort::read(char* buff, int size_to_read)
{
    read(buff, size_to_read, 3000);
    return size_to_read;
}

std::size_t SerialPort::write(char* data, int len, int timeout)
{
    volatile std::size_t write_bytes = 0;
    async_write(data, len, std::bind([](volatile std::size_t* write_bytes_out, std::size_t sz){
        *write_bytes_out = sz;
    }, &write_bytes, std::placeholders::_1));
    while (!write_bytes) {}
    return write_bytes;
}

std::size_t SerialPort::read(char* buff, int size_to_read, int timeout)
{
//    volatile std::size_t read_bytes = 0;
//    async_read(buff, size_to_read, std::bind([](volatile std::size_t* read_bytes_out, std::size_t sz){
//        *read_bytes_out = sz;
//    }, &read_bytes, std::placeholders::_1));
//    while (!read_bytes) {}
//    return read_bytes;
    HAL_UART_Receive(huart, (uint8_t*)buff, size_to_read, timeout);
    return size_to_read;
}


