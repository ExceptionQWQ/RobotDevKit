/*
 * @author BusyBox
 * @date 2024/5/11
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/transfer/can_bus.h"

CanBus::CanBus(IDType id_type)
{
    this->id_type = id_type;
}

CanBus::~CanBus()
{

}
