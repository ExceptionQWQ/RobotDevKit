/*
 * @author BusyBox
 * @date 2024/4/29
 * @version 1.0
 */

#include "rdk/core/motor/damiao/damiao_controller.h"

DamiaoController::DamiaoController(CAN_HandleTypeDef* hcan, Mode mode)
{
    this->hcan = hcan;
    this->mode = mode;
}

DamiaoController::~DamiaoController()
{

}

void DamiaoController::OnHAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance != this->hcan->Instance) return ;
    read_feedback_msg(hcan, CAN_RX_FIFO0);
}

void DamiaoController::OnHAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance != this->hcan->Instance) return ;
    read_feedback_msg(hcan, CAN_RX_FIFO1);
}

uint32_t DamiaoController::get_transmit_id(uint32_t id)
{
    if (mode == Mode::MIT) return id;
    else if (mode == Mode::PosSpeed) return 0x100 + id;
    else if (mode == Mode::Speed) return 0x200 + id;
    else return 0;
}

void DamiaoController::read_feedback_msg(CAN_HandleTypeDef* hcan, uint32_t RxFifo)
{
    CAN_RxHeaderTypeDef rx;
    uint8_t data[8];
    if (HAL_CAN_GetRxMessage(hcan, RxFifo, &rx, data) != HAL_OK) return ;
    if (rx.RTR != CAN_RTR_DATA || rx.IDE != CAN_ID_STD || rx.DLC != 8) return ;

}

void DamiaoController::enable(uint32_t id)
{
    CAN_TxHeaderTypeDef tx;
    tx.StdId = get_transmit_id(id);
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = 8;
    uint8_t data[8];
    data[0] = 0xFF;
    data[1] = 0xFF;
    data[2] = 0xFF;
    data[3] = 0xFF;
    data[4] = 0xFF;
    data[5] = 0xFF;
    data[6] = 0xFF;
    data[7] = 0xFC;
    uint32_t mail;
    HAL_CAN_AddTxMessage(hcan, &tx, data, &mail);
}

void DamiaoController::disable(uint32_t id)
{
    CAN_TxHeaderTypeDef tx;
    tx.StdId = get_transmit_id(id);
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = 8;
    uint8_t data[8];
    data[0] = 0xFF;
    data[1] = 0xFF;
    data[2] = 0xFF;
    data[3] = 0xFF;
    data[4] = 0xFF;
    data[5] = 0xFF;
    data[6] = 0xFF;
    data[7] = 0xFD;
    uint32_t mail;
    HAL_CAN_AddTxMessage(hcan, &tx, data, &mail);
}

void DamiaoController::save_pos_zero(uint32_t id)
{
    CAN_TxHeaderTypeDef tx;
    tx.StdId = get_transmit_id(id);
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = 8;
    uint8_t data[8];
    data[0] = 0xFF;
    data[1] = 0xFF;
    data[2] = 0xFF;
    data[3] = 0xFF;
    data[4] = 0xFF;
    data[5] = 0xFF;
    data[6] = 0xFF;
    data[7] = 0xFE;
    uint32_t mail;
    HAL_CAN_AddTxMessage(hcan, &tx, data, &mail);
}

void DamiaoController::clear_error(uint32_t id)
{
    CAN_TxHeaderTypeDef tx;
    tx.StdId = get_transmit_id(id);
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = 8;
    uint8_t data[8];
    data[0] = 0xFF;
    data[1] = 0xFF;
    data[2] = 0xFF;
    data[3] = 0xFF;
    data[4] = 0xFF;
    data[5] = 0xFF;
    data[6] = 0xFF;
    data[7] = 0xFB;
    uint32_t mail;
    HAL_CAN_AddTxMessage(hcan, &tx, data, &mail);
}

void DamiaoController::write_pos_speed(uint32_t id, float pos, float speed)
{
    uint8_t* _pos = (uint8_t*)&pos;
    uint8_t* _speed = (uint8_t*)&speed;
    CAN_TxHeaderTypeDef tx;
    tx.StdId = get_transmit_id(id);
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = 8;
    uint8_t data[8];
    data[0] = _pos[0];
    data[1] = _pos[1];
    data[2] = _pos[2];
    data[3] = _pos[3];
    data[4] = _speed[0];
    data[5] = _speed[1];
    data[6] = _speed[2];
    data[7] = _speed[3];
    uint32_t mail;
    HAL_CAN_AddTxMessage(hcan, &tx, data, &mail);
}

void DamiaoController::write_speed(uint32_t id, float speed)
{
    uint8_t* _speed = (uint8_t*)&speed;
    CAN_TxHeaderTypeDef tx;
    tx.StdId = get_transmit_id(id);
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = 4;
    uint8_t data[8];
    data[0] = _speed[0];
    data[1] = _speed[1];
    data[2] = _speed[2];
    data[3] = _speed[3];
    uint32_t mail;
    HAL_CAN_AddTxMessage(hcan, &tx, data, &mail);
}