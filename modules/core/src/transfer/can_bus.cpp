/*
 * @author BusyBox
 * @date 2024/4/28
 * @version 1.0
 */

#include "rdk/core/transfer/can_bus.h"

CanBus::CanBus(IDType id_type)
{
    this->id_type = id_type;
}

CanBus::~CanBus()
{
    
}
