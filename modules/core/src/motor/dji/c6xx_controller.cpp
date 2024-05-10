/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/motor/dji/c6xx_controller.h"


C6xxController::C6xxController(std::shared_ptr<CanBus> can_bus)
{
    this->can_bus = can_bus;
    time_stamp = get_time_stamp();
    read_feedback_msg_th = std::thread(read_feedback_msg_thread, this);
}

C6xxController::~C6xxController()
{

}

uint32_t C6xxController::get_time_stamp()
{
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

void C6xxController::read_feedback_msg_thread(C6xxController* controller)
{
    while (true) {
        if (controller->is_terminate()) break;
        uint32_t id;
        CanBus::FrameData data;
        controller->can_bus->read_frame(&id, &data);
        id -= 0x200;
        if (id >= 1 && id <= 8) controller->save_feedback(id, &data);
    }
}

void C6xxController::terminate_read_feedback_msg_thread()
{
    b_is_terminate = true;
}

bool C6xxController::is_terminate()
{
    return b_is_terminate;
}


/*
 * @brief 保存电调反馈结果
 */
void C6xxController::save_feedback(uint32_t id, CanBus::FrameData* data)
{
    if (id < 1 || id > 8) return ;
    feedback[id].angle = (uint16_t)(((uint32_t)data->d0 << 8) | ((uint32_t)data->d1));
    feedback[id].rpm = (int16_t)(((uint32_t)data->d2 << 8) | ((uint32_t)data->d3));
    feedback[id].current = (int16_t)(((uint32_t)data->d4 << 8) | ((uint32_t)data->d5));
    feedback[id].temp = (uint8_t)data->d6;
    time_stamp = get_time_stamp();
}


/*
 * @brief 向电调写入电流
 * @param c1 电调1的电流
 * @param c2 电调2的电流
 * @param c3 电调3的电流
 * @param c4 电调4的电流
 */
void C6xxController::write_current1(int16_t c1, int16_t c2, int16_t c3, int16_t c4)
{
    uint32_t id = 0x200;
    CanBus::FrameData data;
    data.d0 = (uint8_t)(c1 >> 8);
    data.d1 = (uint8_t)(c1);
    data.d2 = (uint8_t)(c2 >> 8);
    data.d3 = (uint8_t)(c2);
    data.d4 = (uint8_t)(c3 >> 8);
    data.d5 = (uint8_t)(c3);
    data.d6 = (uint8_t)(c4 >> 8);
    data.d7 = (uint8_t)(c4);
    can_bus->write_frame(id, data);
}

/*
 * @brief 向电调写入电流
 * @param c5 电调5的电流
 * @param c6 电调6的电流
 * @param c7 电调7的电流
 * @param c8 电调8的电流
 */
void C6xxController::write_current2(int16_t c5, int16_t c6, int16_t c7, int16_t c8)
{
    uint32_t id = 0x1FF;
    CanBus::FrameData data;
    data.d0 = (uint8_t)(c5 >> 8);
    data.d1 = (uint8_t)(c5);
    data.d2 = (uint8_t)(c6 >> 8);
    data.d3 = (uint8_t)(c6);
    data.d4 = (uint8_t)(c7 >> 8);
    data.d5 = (uint8_t)(c7);
    data.d6 = (uint8_t)(c8 >> 8);
    data.d7 = (uint8_t)(c8);
    can_bus->write_frame(id, data);
}

/*
 * @brief 写入电流到缓冲区
 * @param id 电调id
 * @param current 电流
 */
void C6xxController::write_current(int id, int16_t current)
{
    if (id >= 1 && id <= 8) current_buff[id] = current;
}

/*
 * @brief 将缓冲区的电流写入到电调
 */
void C6xxController::flush()
{
    if (enabled) {
        write_current1(current_buff[1], current_buff[2], current_buff[3], current_buff[4]);
        write_current2(current_buff[5], current_buff[6], current_buff[7], current_buff[8]);
    } else {
        write_current1(0, 0, 0, 0);
        write_current2(0, 0, 0, 0);
    }
}

/*
 * @brief 每隔1ms tick一次
 */
void C6xxController::tick()
{
    flush();
}

/*
 * @brief 电调使能
 */
void C6xxController::enable()
{
    enabled = true;
}

/*
 * @brief 电调失能
 */
void C6xxController::disable()
{
    enabled = false;
    flush();
}

/*
 * @brief 读取角度
 * @param id 电调id
 * @return 角度
 */
uint16_t C6xxController::read_angle(int id)
{
    uint32_t time_stamp = get_time_stamp();
    if (time_stamp - this->time_stamp > this->timeout) {
        throw C6xxController::TimeOutException{"[C6xxController::read_angle] TimeOut"};
    }
    if (id >= 1 && id <= 8) return feedback[id].angle;
    return 0;
}

/*
 * @brief 读取转速
 * @param id 电调id
 * @return 转速
 */
int16_t C6xxController::read_rpm(int id)
{
    uint32_t time_stamp = get_time_stamp();
    if (time_stamp - this->time_stamp > this->timeout) {
        throw C6xxController::TimeOutException{"[C6xxController::read_rpm] TimeOut"};
    }
    if (id >= 1 && id <= 8) return feedback[id].rpm;
    return 0;
}

/*
 * @brief 读取实际转矩电流
 * @param id 电调id
 * @return 电流
 */
int16_t C6xxController::read_current(int id)
{
    uint32_t time_stamp = get_time_stamp();
    if (time_stamp - this->time_stamp > this->timeout) {
        throw C6xxController::TimeOutException{"[C6xxController::read_current] TimeOut"};
    }
    if (id >= 1 && id <= 8) return feedback[id].current;
    return 0;
}

/*
 * @brief 读取温度
 * @param id 电调id
 * @return 温度
 */
uint8_t C6xxController::read_temp(int id)
{
    uint32_t time_stamp = get_time_stamp();
    if (time_stamp - this->time_stamp > this->timeout) {
        throw C6xxController::TimeOutException{"[C6xxController::read_temp] TimeOut"};
    }
    if (id >= 1 && id <= 8) return feedback[id].temp;
    return 0;
}