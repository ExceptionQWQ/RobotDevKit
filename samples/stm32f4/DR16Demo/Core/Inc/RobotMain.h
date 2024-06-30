//
// Created by Mac on 2024/6/30.
//


#pragma once

#include "usart.h"
#include "gpio.h"

#ifdef __cplusplus

#include <cstdio>
#include <cstring>
#include <memory>
#include "rdk/core/transfer/serial_port.h"
#include "rdk/core/remote/dr16.h"

extern std::shared_ptr<SerialPort> serialPort;
extern std::shared_ptr<SerialPort> remoteSerial;
extern std::shared_ptr<DR16> dr16;

#endif


#ifdef __cplusplus
extern "C" {
#endif

    void RobotInit();
    void RobotMain();

    void RecvDR16Thread();

#ifdef __cplusplus
};
#endif