//
// Created by Mac on 2024/4/26.
//

#include "RobotMain.h"

CircularBuffer circular_buffer;
SerialPort serial_port(&huart1, &circular_buffer);
Crc8Calculator crc8_calculator(0x31);
ReliableBinaryTransfer transfer(&serial_port, &crc8_calculator);


void RobotInit()
{
    serial_port.start();
}

void RobotTest()
{
    while (true) {
        uint8_t recv_buff[64];
        std::size_t recv_len = transfer.recv_binary(recv_buff, 64, 200);
        if (recv_len == 0) continue;
        transfer.send_binary(recv_buff, recv_len);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
}

void OnHAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size)
{
    serial_port.OnHAL_UARTEx_RxEventCallback(huart, size);
}

void OnHAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    serial_port.OnHAL_UART_TxCpltCallback(huart);
}

void OnHAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    serial_port.OnHAL_UART_RxCpltCallback(huart);
}

void OnHAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
    serial_port.OnHAL_UART_ErrorCallback(huart);
}