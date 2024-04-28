/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
 */

#include "rdk/core/motor/damiao/damiao_controller.h"

DamiaoController::DamiaoController(std::shared_ptr<CanBus> can_bus)
{
    this->can_bus = can_bus;
}

DamiaoController::~DamiaoController()
{
    
}

void DamiaoController::enable(uint32_t id)
{
    CanBus::FrameData data;
    data.d0 = 0xFF;
    data.d1 = 0xFF;
    data.d2 = 0xFF;
    data.d3 = 0xFF;
    data.d4 = 0xFF;
    data.d5 = 0xFF;
    data.d6 = 0xFF;
    data.d7 = 0xFC;
    can_bus->write_frame(id, data);
}

void DamiaoController::disable(uint32_t id)
{
    CanBus::FrameData data;
    data.d0 = 0xFF;
    data.d1 = 0xFF;
    data.d2 = 0xFF;
    data.d3 = 0xFF;
    data.d4 = 0xFF;
    data.d5 = 0xFF;
    data.d6 = 0xFF;
    data.d7 = 0xFD;
    can_bus->write_frame(id, data);
}