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
    enum class Mode:uint8_t
    {
        MIT = 0x00, //MIT模式
        PosSpeed = 0x01, //位置速度模式
        Speed = 0x02, //速度模式
    };

    DamiaoController(std::shared_ptr<CanBus> can_bus, Mode mode);
    ~DamiaoController();

    /*
     * @brief 电机使能
     * @param id 电机id
     */
    void enable(uint32_t id);
    /*
     * @brief 电机失能
     * @param id 电机id
     */
    void disable(uint32_t id);

    /*
     * @brief 保存零点位置
     * @param id 电机id
     */
    void save_pos_zero(uint32_t id);
    /*
     * @brief 清除电机产生的所有错误
     * @param id 电机id
     */
    void clear_error(uint32_t id);

    /*
     * @brief 位置速度模式下写位置速度
     * @param id 电机id
     * @param pos 位置（弧度）
     * @param speed 速度（弧度）
     */
    void write_pos_speed(uint32_t id, float pos, float speed);
    /*
     * @brief 速度模式下写速度
     * @param id 电机id
     * @param speed 速度（弧度）
     */
    void write_speed(uint32_t id, float speed);

private:
    std::shared_ptr<CanBus> can_bus;
    Mode mode;

    /*
     * @brief 获取帧id
     * @param id 电机id
     */
    uint32_t get_transmit_id(uint32_t id);
};

