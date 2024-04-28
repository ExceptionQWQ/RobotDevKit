/*
 * @author BusyBox
 * @date 2024/4/27
 * @version 1.0
 */

#pragma once

#include <cstdint>
#include "stm32f4xx_hal.h"

class C6xxController
{
public:
    C6xxController(CAN_HandleTypeDef* hcan);
    ~C6xxController();

    /*
     * @brief 用户需要在外部编写以下2个中断函数，并调用
     */
    void OnHAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
    void OnHAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);

    void write_current(int id, int16_t current);
    void flush();
    void tick();

    uint16_t read_angle(int id);
    int16_t read_rpm(int id);
    int16_t read_current(int id);
    uint8_t read_temp(int id);

    void enable();
    void disable();

    struct C6xxControllerFeedback
    {
        uint16_t angle; //转子机械角度0~8191，对应0~360度
        int16_t rpm; //转速
        int16_t current; //实际转矩电流
        uint8_t temp; //电机温度
    };


private:
    CAN_HandleTypeDef* hcan;
    bool enabled = true;
    C6xxControllerFeedback feedback[9] = {0};
    int16_t current_buff[9] = {0};

    void save_feedback(uint8_t* data, int motor_id);
    void read_feedback_msg(CAN_HandleTypeDef* hcan, uint32_t RxFifo);
    void write_current1(int16_t c1, int16_t c2, int16_t c3, int16_t c4);
    void write_current2(int16_t c5, int16_t c6, int16_t c7, int16_t c8);


};