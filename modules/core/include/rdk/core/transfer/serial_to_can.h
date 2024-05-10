/*
 * @author BusyBox
 * @date 2024/4/28
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

/*
 * 需要使用TTL串口转CAN总线模块
 * TTL串口波特率：115200 数据位：8位 停止位：1位 校验位：无
 * 模块上4个拨码开关说明：
 * SW1 帧格式选择：ON 扩展帧；OFF 标准帧；
 * SW2 & SW3 CAN波特率选择： SW2  SW3  波特率
 *                         OFF  OFF  500K
 *                         ON   OFF  1M
 *                         OFF  ON   250K
 *                         ON   ON   100K
 * SW4 数据传送模式： ON：串口发送与接收到数据带CAN口的标识符（ID） OFF：串口发送与接收没有CAN口的标识符（ID），即所有字节都是数据。
 * 标准帧：串口发送与接收，前2个字节为CAN口的标识符（ID）
 * 扩展帧：串口发送与接收，前4个字节为CAN口的标识符（ID）
 * 
 * 模块使用时，必须将SW4拨为ON，即串口发送与接收到数据带CAN口的标识符（ID）
 * 
 * （这个模块并不好用，串口波特率太低，数据容易错位，不带帧头，目前只针对ID为0x200~0x210进行帧头定位）
 */

#include <cstdint>
#include <memory>
#include "rdk/core/transfer/can_bus.h"
#include "rdk/core/transfer/serial_port.h"

class SerialToCan : public CanBus
{
public:
    SerialToCan(std::shared_ptr<SerialPort> serial_port, IDType id_type);
    ~SerialToCan();

    void async_write_frame(uint32_t id, FrameData data, ResultHandler result_handler) override;
    void async_read_frame(uint32_t* id, FrameData* data, ResultHandler result_handler) override;

    void write_frame(uint32_t id, FrameData data) override;
    void read_frame(uint32_t* id, FrameData* data) override;

    void write_frame(uint32_t id, FrameData data, int timeout) override;
    void read_frame(uint32_t* id, FrameData* data, int timeout) override;

private:
    std::shared_ptr<SerialPort> serial_port;
    std::size_t frame_size;
    uint8_t send_buff[16];
    uint8_t recv_buff[16];

    boost::asio::io_context ioc;
    std::shared_ptr<boost::asio::high_resolution_timer> timer;

    void encode(uint8_t* buff, uint32_t id, FrameData data);
    void decode(uint8_t* buff, uint32_t* id, FrameData* data);

};