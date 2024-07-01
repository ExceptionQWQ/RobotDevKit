//
// Created by Mac on 2024/7/1.
//

#include "RobotMain.h"

std::shared_ptr<SerialPort> serialPort;
std::shared_ptr<HuanerProtocol> servo;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size)
{
    serialPort->OnHAL_UARTEx_RxEventCallback(huart, size);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    serialPort->OnHAL_UART_TxCpltCallback(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    serialPort->OnHAL_UART_RxCpltCallback(huart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
    serialPort->OnHAL_UART_ErrorCallback(huart);
}

void RobotInit()
{
    serialPort = std::make_shared<SerialPort>(&huart8);
    serialPort->start();

    servo = std::make_shared<HuanerProtocol>(serialPort);
}

void RobotMain()
{
    while (true) {
        servo->move_time_write(3, 600, 100);
        HAL_Delay(500);
        servo->move_time_write(3, 400, 100);
        HAL_Delay(500);
    }
}