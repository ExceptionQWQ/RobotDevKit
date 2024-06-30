//
// Created by Mac on 2024/6/30.
//

#include "RobotMain.h"

std::shared_ptr<SerialPort> serialPort;
std::shared_ptr<SerialPort> remoteSerial;
std::shared_ptr<DR16> dr16;


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size)
{
    serialPort->OnHAL_UARTEx_RxEventCallback(huart, size);
    remoteSerial->OnHAL_UARTEx_RxEventCallback(huart, size);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    serialPort->OnHAL_UART_TxCpltCallback(huart);
    remoteSerial->OnHAL_UART_TxCpltCallback(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    serialPort->OnHAL_UART_RxCpltCallback(huart);
    remoteSerial->OnHAL_UART_RxCpltCallback(huart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
    serialPort->OnHAL_UART_ErrorCallback(huart);
    remoteSerial->OnHAL_UART_ErrorCallback(huart);
}

void RobotInit()
{
    serialPort = std::make_shared<SerialPort>(&huart8);
    serialPort->start();

    remoteSerial = std::make_shared<SerialPort>(&huart1);
    remoteSerial->start();

    dr16 = std::make_shared<DR16>(remoteSerial);
}

void RobotMain()
{
    while (true) {
        HAL_GPIO_TogglePin(LEDG_GPIO_Port, LEDG_Pin);
        char buff[128];

        snprintf(buff, 128, "0:%d 1:%d 2:%d 3:%d\r\n", dr16->get_channel_0(), dr16->get_channel_1(),
                 dr16->get_channel_2(), dr16->get_channel_3());
        serialPort->write(reinterpret_cast<uint8_t*>(buff), strlen(buff));

        snprintf(buff, 128, "s1:%d s2:%d\r\n", dr16->get_s1(), dr16->get_s2());
        serialPort->write(reinterpret_cast<uint8_t*>(buff), strlen(buff));

        HAL_Delay(100);
    }
}

void RecvDR16Thread()
{
    dr16->start();
}