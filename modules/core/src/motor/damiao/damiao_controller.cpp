/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/motor/damiao/damiao_controller.h"

DamiaoController::DamiaoController(std::shared_ptr<CanBus> can_bus, Mode mode)
{
    this->can_bus = can_bus;
    this->mode = mode;
}

DamiaoController::~DamiaoController()
{
    
}

/*
 * @brief 电机使能
 * @param id 电机id
 */
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
    can_bus->write_frame(get_transmit_id(id), data);
}

/*
 * @brief 电机失能
 * @param id 电机id
 */
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
    can_bus->write_frame(get_transmit_id(id), data);
}

/*
 * @brief 获取帧id
 * @param id 电机id
 */
uint32_t DamiaoController::get_transmit_id(uint32_t id)
{
    if (mode == Mode::MIT) return id;
    else if (mode == Mode::PosSpeed) return 0x100 + id;
    else if (mode == Mode::Speed) return 0x200 + id;
    else return 0;
}

/*
 * @brief 保存零点位置
 * @param id 电机id
 */
void DamiaoController::save_pos_zero(uint32_t id)
{
    CanBus::FrameData data;
    data.d0 = 0xFF;
    data.d1 = 0xFF;
    data.d2 = 0xFF;
    data.d3 = 0xFF;
    data.d4 = 0xFF;
    data.d5 = 0xFF;
    data.d6 = 0xFF;
    data.d7 = 0xFE;
    can_bus->write_frame(get_transmit_id(id), data);
}

/*
 * @brief 清除电机产生的所有错误
 * @param id 电机id
 */
void DamiaoController::clear_error(uint32_t id)
{
    CanBus::FrameData data;
    data.d0 = 0xFF;
    data.d1 = 0xFF;
    data.d2 = 0xFF;
    data.d3 = 0xFF;
    data.d4 = 0xFF;
    data.d5 = 0xFF;
    data.d6 = 0xFF;
    data.d7 = 0xFB;
    can_bus->write_frame(get_transmit_id(id), data);
}

/*
 * @brief 位置速度模式下写位置速度
 * @param id 电机id
 * @param pos 位置（弧度）
 * @param speed 速度（弧度）
 */
void DamiaoController::write_pos_speed(uint32_t id, float pos, float speed)
{
    uint8_t* _pos = (uint8_t*)&pos;
    uint8_t* _speed = (uint8_t*)&speed;

    CanBus::FrameData data;
    data.d0 = _pos[0];
    data.d1 = _pos[1];
    data.d2 = _pos[2];
    data.d3 = _pos[3];
    data.d4 = _speed[0];
    data.d5 = _speed[1];
    data.d6 = _speed[2];;
    data.d7 = _speed[3];
    can_bus->write_frame(get_transmit_id(id), data);
}

/*
 * @brief 速度模式下写速度
 * @param id 电机id
 * @param speed 速度（弧度）
 */
void DamiaoController::write_speed(uint32_t id, float speed)
{
    uint8_t* _speed = (uint8_t*)&speed;

    CanBus::FrameData data;
    data.d0 = _speed[0];
    data.d1 = _speed[1];
    data.d2 = _speed[2];
    data.d3 = _speed[3];
    can_bus->write_frame(get_transmit_id(id), data);
}
