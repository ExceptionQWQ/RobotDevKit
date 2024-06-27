/*
 * @author BusyBox
 * @date 2024/4/28
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

/*
 * 需要使用高速串口TTL转CAN总线模块 https://github.com/ExceptionQWQ/SerialPort_TO_CAN
 * TTL串口波特率：921600 数据位：8位 停止位：1位 校验位：无
 * 模块上4个拨码开关说明：
 * SW1 帧格式选择：ON 标准帧；OFF 扩展帧；
 * SW2 & SW3 CAN波特率选择： SW2  SW3  波特率
 *                         ON   ON     1M
 *                         OFF  ON    500K
 *                         ON   OFF   250K
 *                         OFF  OFF   100K
 * SW4 是否开启终端电阻 ON 开启 OFF 关闭
 */

#include <cstdint>
#include <memory>
#include "rdk/core/transfer/can_bus.h"
#include "rdk/core/transfer/serial_port.h"
#include "rdk/core/transfer/simple_binary_transfer.h"

class SerialToCan : public CanBus
{
public:
    SerialToCan(std::shared_ptr<SerialPort> serial_port, IDType id_type);
    ~SerialToCan();

    /*
     * @brief 异步写入帧数据
     * @param id 帧id
     * @param data 数据帧
     * @param result_handler 写入完成回调函数
     */
    std::size_t async_write_frame(uint32_t id, FrameData data, ResultHandler result_handler) override;
    /*
     * @brief 异步读取帧数据
     * @param id 帧id
     * @param data 数据帧
     * @param result_handler 读取完成回调函数
     */
    std::size_t async_read_frame(uint32_t* id, FrameData* data, ResultHandler result_handler) override;

    /*
     * @brief 写入帧数据
     * @param id 帧id
     * @param data 数据帧
     */
    std::size_t write_frame(uint32_t id, FrameData data) override;
    /*
     * @brief 读取帧数据
     * @param id 帧id
     * @param data 数据帧
     */
    std::size_t read_frame(uint32_t* id, FrameData* data) override;

    /*
     * @brief 写入帧数据
     * @param id 帧id
     * @param data 数据帧
     * @param timeout 超时时间
     */
    std::size_t write_frame(uint32_t id, FrameData data, int timeout) override;

    /*
     * @brief 读取帧数据
     * @param id 帧id
     * @param data 数据帧
     * @param timeout 超时时间
     */
    std::size_t read_frame(uint32_t* id, FrameData* data, int timeout) override;

private:

#pragma pack(push, 1)
    struct TransferData
    {
        CanBus::IDType id_type;
        uint32_t frame_id;
        CanBus::FrameData frame_data;
    };
#pragma pack(pop)

    std::shared_ptr<SerialPort> serial_port;
    std::shared_ptr<SimpleBinaryTransfer> transfer;


};