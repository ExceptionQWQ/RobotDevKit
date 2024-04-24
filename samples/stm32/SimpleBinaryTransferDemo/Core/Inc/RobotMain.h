/*
 * @author BusyBox
 * @date 2024/4/24
 * @version 1.0
 */

#pragma once

#include "usart.h"

#ifdef __cplusplus

#include <functional>
#include <memory>
#include "rdk/core/transfer/serial_port.h"
#include "rdk/core/transfer/simple_binary_transfer.h"

extern std::shared_ptr<SerialPort> serial_port;
extern std::shared_ptr<SimpleBinaryTransfer> transfer;


#endif



#ifdef __cplusplus
extern "C" {
#endif

    void RobotInit();
    void RobotTest();
    void OnHAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size);
    void OnHAL_UART_TxCpltCallback(UART_HandleTypeDef* huart);
    void OnHAL_UART_RxCpltCallback(UART_HandleTypeDef* huart);
    void OnHAL_UART_ErrorCallback(UART_HandleTypeDef* huart);

#ifdef __cplusplus
};
#endif