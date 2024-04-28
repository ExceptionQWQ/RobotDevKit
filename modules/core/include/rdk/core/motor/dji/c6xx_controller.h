/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
 */

#pragma once

#include <cstdint>
#include <thread>
#include "rdk/core/transfer/can_bus.h"

class C6xxController
{
public:
    C6xxController(std::shared_ptr<CanBus> can_bus);
    ~C6xxController();

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

    struct TimeOutException
    {
        std::string error;
    };


private:
    std::shared_ptr<CanBus> can_bus;
    bool enabled = true;
    C6xxControllerFeedback feedback[9] = {0};
    int16_t current_buff[9] = {0};
    bool b_is_terminate = false;
    std::thread read_feedback_msg_th;
    int timeout = 3;
    uint32_t time_stamp = 0;

    static void read_feedback_msg_thread(C6xxController* controller);
    friend void read_feedback_msg_thread(C6xxController* controller);
    void terminate_read_feedback_msg_thread();
    bool is_terminate();

    void save_feedback(uint32_t id, CanBus::FrameData* data);

    uint32_t get_time_stamp();

    void write_current1(int16_t c1, int16_t c2, int16_t c3, int16_t c4);
    void write_current2(int16_t c5, int16_t c6, int16_t c7, int16_t c8);


};