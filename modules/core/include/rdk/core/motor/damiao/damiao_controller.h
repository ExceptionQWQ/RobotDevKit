/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <cstdint>
#include <thread>
#include "rdk/core/transfer/can_bus.h"


class DamiaoController
{
public:
    DamiaoController(std::shared_ptr<CanBus> can_bus);
    ~DamiaoController();


    void enable(uint32_t id);
    void disable(uint32_t id);


private:
    std::shared_ptr<CanBus> can_bus;

};

