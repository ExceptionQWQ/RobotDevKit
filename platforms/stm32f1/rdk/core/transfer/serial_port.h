/*
 * @author BusyBox
 * @date 2024/4/13
 * @version 1.0
 */

#pragma once

#include "rdk/core/transfer/circular_buffer.h"
#include <cstdint>
#include "stm32f1xx_hal.h"


using ResultHandler = void(*)(std::size_t);

/*
 * @brief STM32串口库，请开启RX DMA
 */
class SerialPort
{
public:
    SerialPort(UART_HandleTypeDef* huart, CircularBuffer* circular_buffer);
    ~SerialPort();


    /*
     * @brief 用户需要在外部编写以下4个中断函数，并调用
     */
    void OnHAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size);
    void OnHAL_UART_TxCpltCallback(UART_HandleTypeDef* huart);
    void OnHAL_UART_RxCpltCallback(UART_HandleTypeDef* huart);
    void OnHAL_UART_ErrorCallback(UART_HandleTypeDef* huart);

    void start();

    void async_write(uint8_t* data, std::size_t len, ResultHandler result_handler);
    void async_read(uint8_t* buff, std::size_t size_to_read, ResultHandler result_handler);

    std::size_t write(uint8_t* data, std::size_t len);
    std::size_t read(uint8_t* buff, std::size_t size_to_read);

    std::size_t write(uint8_t* data, std::size_t len, int timeout);
    std::size_t read(uint8_t* buff, std::size_t size_to_read, int timeout);


private:
    UART_HandleTypeDef* huart;
    ResultHandler write_result_handler;
    ResultHandler read_result_handler;

    bool buff_select = true;
    uint8_t recv_buff1[512];
    uint8_t recv_buff2[512];

    CircularBuffer* circular_buffer;

};