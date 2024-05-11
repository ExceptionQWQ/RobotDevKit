/*
 * @author BusyBox
 * @date 2024/4/28
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/transfer/serial_to_can.h"

SerialToCan::SerialToCan(SimpleBinaryTransfer* transfer, IDType id_type):CanBus(id_type)
{
    this->transfer = transfer;
}

SerialToCan::~SerialToCan()
{

}


/*
 * @brief 设置帧类型
 * @param id_type 帧类型
 */
void SerialToCan::set_id_type(IDType id_type)
{
    this->id_type = id_type;
}

/*
 * @brief 异步写入帧数据
 * @param id 帧id
 * @param data 数据帧
 * @param result_handler 写入完成回调函数
 */
std::size_t SerialToCan::async_write_frame(uint32_t id, FrameData data, ResultHandler result_handler)
{
    // TODO
    return 0;
}

/*
 * @brief 异步读取帧数据
 * @param id 帧id
 * @param data 数据帧
 * @param result_handler 读取完成回调函数
 */
std::size_t SerialToCan::async_read_frame(uint32_t* id, FrameData* data, ResultHandler result_handler)
{
    // TODO
    return 0;
}

/*
 * @brief 写入帧数据
 * @param id 帧id
 * @param data 数据帧
 */
std::size_t SerialToCan::write_frame(uint32_t id, FrameData data)
{
    TransferData td;
    td.id_type = id_type;
    td.frame_id = id;
    td.frame_data = data;

    transfer->send_binary(reinterpret_cast<uint8_t*>(&td), sizeof(TransferData));
    return sizeof(TransferData);
}

/*
 * @brief 读取帧数据
 * @param id 帧id
 * @param data 数据帧
 */
std::size_t SerialToCan::read_frame(uint32_t* id, FrameData* data)
{
    uint8_t recv_buff[256];

    std::size_t recv_bytes = transfer->recv_binary(recv_buff, 256);
    if (recv_bytes) {
        TransferData* td = reinterpret_cast<TransferData*>(recv_buff);
        *id = td->frame_id;
        *data = td->frame_data;
        return sizeof(TransferData);
    }
    return 0;
}

/*
 * @brief 写入帧数据
 * @param id 帧id
 * @param data 数据帧
 * @param timeout 超时时间
 */
std::size_t SerialToCan::write_frame(uint32_t id, FrameData data, int timeout)
{
    return write_frame(id, data);
}

/*
 * @brief 读取帧数据
 * @param id 帧id
 * @param data 数据帧
 * @param timeout 超时时间
 */
std::size_t SerialToCan::read_frame(uint32_t* id, FrameData* data, int timeout)
{
    uint8_t recv_buff[256];

    std::size_t recv_bytes = transfer->recv_binary(recv_buff, 256, timeout);
    if (recv_bytes) {
        TransferData* td = reinterpret_cast<TransferData*>(recv_buff);
        *id = td->frame_id;
        *data = td->frame_data;
        return sizeof(TransferData);
    }
    return 0;
}