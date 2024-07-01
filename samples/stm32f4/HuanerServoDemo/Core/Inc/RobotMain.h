//
// Created by Mac on 2024/7/1.
//

#pragma once

#include "usart.h"

#ifdef __cplusplus

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <memory>
#include "rdk/core/transfer/serial_port.h"
#include "rdk/core/servo/huaner/huaner_protocol.h"

extern std::shared_ptr<SerialPort> serialPort;
extern std::shared_ptr<HuanerProtocol> servo;

#endif


#ifdef __cplusplus
extern "C" {
#endif

    void RobotInit();
    void RobotMain();

#ifdef __cplusplus
};
#endif