/*
 * @author BusyBox
 * @date 2024/4/24
 * @version 1.0
 */

#include "RobotMain.h"

std::shared_ptr<SerialPort> serial_port;
std::shared_ptr<ReliableBinaryTransfer> transfer;

void RobotInit()
{
    serial_port = std::make_shared<SerialPort>(&huart1);
    transfer = std::make_shared<ReliableBinaryTransfer>(serial_port);
}

void RobotTest()
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);
    while (true) {
        char recv_buff[64];
        std::size_t recv_len = transfer->recv_binary(recv_buff, 64, 3000);
        if (recv_len == 0) continue;
        transfer->send_binary(recv_buff, recv_len);
        HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_10);
    }
}

void OnHAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size)
{
    serial_port->OnHAL_UARTEx_RxEventCallback(huart, size);
}

void OnHAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    serial_port->OnHAL_UART_TxCpltCallback(huart);
}

void OnHAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    serial_port->OnHAL_UART_RxCpltCallback(huart);
}

void OnHAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
    serial_port->OnHAL_UART_ErrorCallback(huart);
}