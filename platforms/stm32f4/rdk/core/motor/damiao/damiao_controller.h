/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
 */

#pragma once

#include <cstdint>
#include "stm32f4xx_hal.h"

class DamiaoController
{
public:
    enum class Mode:uint8_t
    {
        MIT = 0x00, //MIT模式
        PosSpeed = 0x01, //位置速度模式
        Speed = 0x02, //速度模式
    };


    DamiaoController(CAN_HandleTypeDef* hcan, Mode mode);
    ~DamiaoController();

    /*
     * @brief 用户需要在外部编写以下2个中断函数，并调用
     */
    void OnHAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
    void OnHAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);

    void enable(uint32_t id);
    void disable(uint32_t id);
    void save_pos_zero(uint32_t id);
    void clear_error(uint32_t id);

    void write_pos_speed(uint32_t id, float pos, float speed);
    void write_speed(uint32_t id, float speed);

private:
    CAN_HandleTypeDef* hcan;
    Mode mode;

    uint32_t get_transmit_id(uint32_t id);
    void read_feedback_msg(CAN_HandleTypeDef* hcan, uint32_t RxFifo);


};