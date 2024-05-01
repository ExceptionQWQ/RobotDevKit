/*
 * @author BusyBox
 * @date 2024/4/27
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/motor/dji/c6xx_controller.h"

C6xxController::C6xxController(CAN_HandleTypeDef* hcan)
{
    this->hcan = hcan;
}

C6xxController::~C6xxController()
{

}

void C6xxController::OnHAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance != this->hcan->Instance) return ;
    read_feedback_msg(hcan, CAN_RX_FIFO0);
}

void C6xxController::OnHAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance != this->hcan->Instance) return ;
    read_feedback_msg(hcan, CAN_RX_FIFO1);
}

/*
 * @brief 保存电调反馈结果
 */
void C6xxController::save_feedback(uint8_t* data, int motor_id)
{
    if (motor_id < 1 || motor_id > 8) return ;
    feedback[motor_id].angle = (uint16_t)(((uint32_t)data[0] << 8) | ((uint32_t)data[1]));
    feedback[motor_id].rpm = (int16_t)(((uint32_t)data[2] << 8) | ((uint32_t)data[3]));
    feedback[motor_id].current = (int16_t)(((uint32_t)data[4] << 8) | ((uint32_t)data[5]));
    feedback[motor_id].temp = (uint8_t)data[6];
}

/*
 * @brief 读取电调反馈消息
 */
void C6xxController::read_feedback_msg(CAN_HandleTypeDef* hcan, uint32_t RxFifo)
{
    CAN_RxHeaderTypeDef rx;
    uint8_t data[8];
    if (HAL_CAN_GetRxMessage(hcan, RxFifo, &rx, data) != HAL_OK) return ;
    if (rx.RTR != CAN_RTR_DATA || rx.IDE != CAN_ID_STD || rx.DLC != 8) return ;
    int id = (int)rx.StdId - 0x200;
    if (id >= 1 && id <= 8) save_feedback(data, id);
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
    CAN_TxHeaderTypeDef tx;
    tx.StdId = 0x200;
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.TransmitGlobalTime = DISABLE;
    tx.DLC = 9;
    uint8_t data[8];
    data[0] = (uint8_t)(c1 >> 8);
    data[1] = (uint8_t)(c1);
    data[2] = (uint8_t)(c2 >> 8);
    data[3] = (uint8_t)(c2);
    data[4] = (uint8_t)(c3 >> 8);
    data[5] = (uint8_t)(c3);
    data[6] = (uint8_t)(c4 >> 8);
    data[7] = (uint8_t)(c4);
    uint32_t mail;
    HAL_CAN_AddTxMessage(hcan, &tx, data, &mail);
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
    CAN_TxHeaderTypeDef tx;
    tx.StdId = 0x1FF;
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.TransmitGlobalTime = DISABLE;
    tx.DLC = 9;
    uint8_t data[8];
    data[0] = (uint8_t)(c5 >> 8);
    data[1] = (uint8_t)(c5);
    data[2] = (uint8_t)(c6 >> 8);
    data[3] = (uint8_t)(c6);
    data[4] = (uint8_t)(c7 >> 8);
    data[5] = (uint8_t)(c7);
    data[6] = (uint8_t)(c8 >> 8);
    data[7] = (uint8_t)(c8);
    uint32_t mail;
    HAL_CAN_AddTxMessage(hcan, &tx, data, &mail);
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
    if (id >= 1 && id <= 8) return feedback[id].temp;
    return 0;
}