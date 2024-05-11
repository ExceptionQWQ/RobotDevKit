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

class C6xxController
{
public:
    C6xxController(std::shared_ptr<CanBus> can_bus);
    ~C6xxController();

    /*
     * @brief 写入电流到缓冲区
     * @param id 电调id
     * @param current 电流
     */
    void write_current(int id, int16_t current);
    /*
     * @brief 将缓冲区的电流写入到电调
     */
    void flush();
    /*
     * @brief 每隔1ms tick一次，会自动调用flush函数
     */
    void tick();

    /*
     * @brief 读取角度
     * @param id 电调id
     * @return 角度
     */
    uint16_t read_angle(int id);
    /*
     * @brief 读取转速
     * @param id 电调id
     * @return 转速
     */
    int16_t read_rpm(int id);
    /*
     * @brief 读取实际转矩电流
     * @param id 电调id
     * @return 电流
     */
    int16_t read_current(int id);
    /*
     * @brief 读取温度
     * @param id 电调id
     * @return 温度
     */
    uint8_t read_temp(int id);

    /*
     * @brief 控制器使能
     */
    void enable();
    /*
     * @brief 控制器失能
     */
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

    /*
     * @brief 保存电调反馈结果
     */
    void save_feedback(uint32_t id, CanBus::FrameData* data);

    uint32_t get_time_stamp();

    /*
     * @brief 向电调写入电流
     * @param c1 电调1的电流
     * @param c2 电调2的电流
     * @param c3 电调3的电流
     * @param c4 电调4的电流
     */
    void write_current1(int16_t c1, int16_t c2, int16_t c3, int16_t c4);
    /*
     * @brief 向电调写入电流
     * @param c5 电调5的电流
     * @param c6 电调6的电流
     * @param c7 电调7的电流
     * @param c8 电调8的电流
     */
    void write_current2(int16_t c5, int16_t c6, int16_t c7, int16_t c8);


};