/*
 * @author BusyBox
 * @date 2024/4/13
 * @version 1.0
 */

#pragma once

#include "rdk/core/transfer/io_stream.h"
#include "stm32f4xx_hal.h"


class SerialPort : public IOStream
{
public:
    SerialPort(UART_HandleTypeDef* huart);
    ~SerialPort();

    void OnHAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size);
    void OnHAL_UART_TxCpltCallback(UART_HandleTypeDef* huart);
    void OnHAL_UART_ErrorCallback(UART_HandleTypeDef* huart);

    void async_write(char* data, int len, ResultHandler result_handler);
    void async_read(char* buff, int size_to_read, ResultHandler result_handler);

    std::size_t write(char* data, int len);
    std::size_t read(char* buff, int size_to_read);

    std::size_t write(char* data, int len, int timeout);
    std::size_t read(char* buff, int size_to_read, int timeout);

private:
    UART_HandleTypeDef* huart;
    ResultHandler write_result_handler;
    ResultHandler read_result_handler;

};