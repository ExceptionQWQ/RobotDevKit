/*
 * @author BusyBox
 * @date 2024/4/27
 * @version 1.0
 */

#include "RobotMain.h"

std::shared_ptr<SerialPort> serial_port;
std::shared_ptr<ReliableBinaryTransfer> transfer;
std::shared_ptr<C6xxController> c6xx_controller1;
std::shared_ptr<C6xxController> c6xx_controller2;
std::shared_ptr<M2006Motor> m2006_motor1;
std::shared_ptr<M3508Motor> m3508_motor2;
std::shared_ptr<M3508Motor> m3508_motor3;
std::shared_ptr<M3508Motor> m3508_motor4;
std::shared_ptr<M3508Motor> m3508_motor5;
std::shared_ptr<M3508Motor> m3508_motor6;
std::shared_ptr<M2006Motor> m2006_motor7;
std::shared_ptr<M3508Motor> m3508_motor8;
std::shared_ptr<M3508Motor> m3508_motor9;
std::shared_ptr<M3508Motor> m3508_motor10;
std::shared_ptr<M3508Motor> m3508_motor11;
std::shared_ptr<OmnidirectionalMotion> motion;

uint32_t motion_timeout = 0;

void RobotInit()
{
    serial_port = std::make_shared<SerialPort>(&huart8);
    serial_port->start();
    transfer = std::make_shared<ReliableBinaryTransfer>(serial_port);
    c6xx_controller1 = std::make_shared<C6xxController>(&hcan1);
    c6xx_controller2 = std::make_shared<C6xxController>(&hcan2);

    m2006_motor1 = std::make_shared<M2006Motor>(c6xx_controller1, 1, DjiMotor::Mode::SPEED_POS);
    m3508_motor2 = std::make_shared<M3508Motor>(c6xx_controller1, 2, DjiMotor::Mode::SPEED);
    m3508_motor3 = std::make_shared<M3508Motor>(c6xx_controller1, 3, DjiMotor::Mode::SPEED);
    m3508_motor4 = std::make_shared<M3508Motor>(c6xx_controller1, 4, DjiMotor::Mode::SPEED_POS);
    m3508_motor5 = std::make_shared<M3508Motor>(c6xx_controller1, 5, DjiMotor::Mode::SPEED);
    m3508_motor6 = std::make_shared<M3508Motor>(c6xx_controller1, 6, DjiMotor::Mode::SPEED);

    m2006_motor7 = std::make_shared<M2006Motor>(c6xx_controller2, 1, DjiMotor::Mode::SPEED_POS);
    m3508_motor8 = std::make_shared<M3508Motor>(c6xx_controller2, 2, DjiMotor::Mode::SPEED);
    m3508_motor9 = std::make_shared<M3508Motor>(c6xx_controller2, 3, DjiMotor::Mode::SPEED);
    m3508_motor10 = std::make_shared<M3508Motor>(c6xx_controller2, 4, DjiMotor::Mode::SPEED);
    m3508_motor11 = std::make_shared<M3508Motor>(c6xx_controller2, 5, DjiMotor::Mode::SPEED);

    m3508_motor2->set_reverse(true);
    m3508_motor8->set_reverse(true);
    m3508_motor9->set_reverse(true);
    m3508_motor3->set_reverse(true);

    motion = std::make_shared<OmnidirectionalMotion>(m3508_motor2, m3508_motor8, m3508_motor9, m3508_motor3);
    motion_timeout = HAL_GetTick();

}

void RobotTest()
{

}

void HandlePingRequest(MasterCmd* cmd)
{
    transfer->send_binary(reinterpret_cast<uint8_t*>(cmd), sizeof(MasterCmd));
}

void HandleMotionRequest(MasterCmd* cmd)
{
    MasterCmdMotion* motion_cmd = reinterpret_cast<MasterCmdMotion*>(cmd);
    motion->clear();
    motion->add_x_speed(motion_cmd->x_speed);
    motion->add_y_speed(motion_cmd->y_speed);
    motion->add_z_speed(motion_cmd->z_speed);
    motion->commit();
    motion_timeout = HAL_GetTick();
}

void RobotRecvMasterCmdThread()
{
    while (true) {
        uint8_t recv_buff[64];
        std::size_t recv_len = transfer->recv_binary(recv_buff, 64, 200);
        if (recv_len == 0) continue;
        HAL_GPIO_TogglePin(LEDG_GPIO_Port, LEDG_Pin);

        MasterCmd* ret_cmd = reinterpret_cast<MasterCmd*>(recv_buff);
        switch (ret_cmd->cmd_type) {
            case MasterCmdType::Ping:
                HandlePingRequest(ret_cmd);
                break;
            case MasterCmdType::Motion:
                HandleMotionRequest(ret_cmd);
                break;
        }
    }
}

void RobotTick()
{
    c6xx_controller1->tick();
    c6xx_controller2->tick();
    motion->tick();

    //如果超过1秒没有接收到上位机控制指令，则让底盘立刻停止运动
    if (abs(HAL_GetTick() - motion_timeout) > 1000) {
        motion->clear();
        motion->commit();
    }
}


void OnHAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size)
{
    serial_port->OnHAL_UARTEx_RxEventCallback(huart, size);
}

void OnHAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    serial_port->OnHAL_UART_TxCpltCallback(huart);
}

void OnHAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    serial_port->OnHAL_UART_RxCpltCallback(huart);
}

void OnHAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
    serial_port->OnHAL_UART_ErrorCallback(huart);
}

void OnHAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    c6xx_controller1->OnHAL_CAN_RxFifo0MsgPendingCallback(hcan);
    c6xx_controller2->OnHAL_CAN_RxFifo0MsgPendingCallback(hcan);
}

void OnHAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    c6xx_controller1->OnHAL_CAN_RxFifo1MsgPendingCallback(hcan);
    c6xx_controller2->OnHAL_CAN_RxFifo1MsgPendingCallback(hcan);
}