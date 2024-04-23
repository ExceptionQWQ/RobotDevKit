/*
 * @author BusyBox
 * @date 2024/4/24
 * @version 1.0
 */

#include "RobotMain.h"


std::shared_ptr<SerialPort> serial_port;
std::shared_ptr<SimpleMessageTransfer> transfer;

void RobotInit()
{
    serial_port = std::make_shared<SerialPort>(&huart1);
    transfer = std::make_shared<SimpleMessageTransfer>(serial_port);
}


void RobotTest()
{
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);
    while (true) {
        std::string msg = transfer->recv_message(10);
        transfer->send_message(msg);
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

void OnHAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
    serial_port->OnHAL_UART_ErrorCallback(huart);
}