/*
 * @author BusyBox
 * @date 2024/4/13
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */


#include "rdk/core/transfer/serial_port.h"

SerialPort::SerialPort(UART_HandleTypeDef* huart)
{
    this->huart = huart;

}

SerialPort::~SerialPort()
{

}

void SerialPort::start()
{
    HAL_UARTEx_ReceiveToIdle_DMA(huart, recv_buff1, 512);
}

void SerialPort::OnHAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size)
{
    if (huart->Instance != this->huart->Instance) return ;
    if (buff_select) { //双缓冲接收数据
        buff_select = false;
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);
        HAL_UARTEx_ReceiveToIdle_DMA(huart, recv_buff2, 512);
        for (int i = 0; i < size; ++i) {
            circular_buffer.push_back(recv_buff1[i]);
        }
    } else {
        buff_select = true;
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_RXNE);
        __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE);
        HAL_UARTEx_ReceiveToIdle_DMA(huart, recv_buff1, 512);
        for (int i = 0; i < size; ++i) {
            circular_buffer.push_back(recv_buff2[i]);
        }
    }
}

void SerialPort::OnHAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance != this->huart->Instance) return ;
    if (write_result_handler) write_result_handler(huart->TxXferSize - huart->TxXferCount);
}

void SerialPort::OnHAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance != this->huart->Instance) return ;
    if (read_result_handler) read_result_handler(huart->RxXferSize - huart->RxXferCount);
}

void SerialPort::OnHAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
    if (huart->Instance != this->huart->Instance) return ;
    __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_ORE); //清除ORE错误

    this->OnHAL_UARTEx_RxEventCallback(huart, huart->RxXferSize - huart->RxXferCount);

    //重启DMA接收
    buff_select = true;
    HAL_UARTEx_ReceiveToIdle_DMA(huart, recv_buff1, 512);
}

void SerialPort::async_write(uint8_t* data, std::size_t len, ResultHandler result_handler)
{
    write_result_handler = result_handler;
    HAL_UART_Transmit_IT(huart, data, len);
}

void SerialPort::async_read(uint8_t* buff, std::size_t size_to_read, ResultHandler result_handler)
{
    read_result_handler = result_handler;
    HAL_UART_Receive_DMA(huart, buff, size_to_read);
}

std::size_t SerialPort::write(uint8_t* data, std::size_t len)
{
    return write(data, len, 3000);
}

std::size_t SerialPort::read(uint8_t* buff, std::size_t size_to_read)
{
    return read(buff, size_to_read, 3000);
}

std::size_t SerialPort::write(uint8_t* data, std::size_t len, int timeout)
{
    HAL_UART_Transmit(huart, data, len, timeout);
    return len;
}

std::size_t SerialPort::read(uint8_t* buff, std::size_t size_to_read, int timeout)
{
    uint32_t tick = HAL_GetTick();
    int index = 0;

    while (size_to_read--) {
        if (circular_buffer.available()) {
            buff[index++] = circular_buffer.pop_front();
        }
        if (HAL_GetTick() - tick > (uint32_t)timeout) break;
    }
    return index;
}
