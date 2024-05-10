/*
 * @author BusyBox
 * @date 2024/4/28
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <cstdint>
#include <functional>
#include <boost/system/error_code.hpp>

class CanBus
{
public:

    enum class IDType:uint8_t
    {
        STD = 0x01, //标准帧
        EXT = 0x02, //扩展帧
    };

    enum class FrameType:uint8_t
    {
        Data = 0x01, //数据帧
        Remote = 0x02, //远程帧
    };

    struct FrameData
    {
        uint8_t d0;
        uint8_t d1;
        uint8_t d2;
        uint8_t d3;
        uint8_t d4;
        uint8_t d5;
        uint8_t d6;
        uint8_t d7;
    };

    using ResultHandler = std::function<void(const boost::system::error_code& error, uint32_t id, CanBus::FrameData data)>;

    CanBus(IDType id_type);
    ~CanBus();

    virtual void async_write_frame(uint32_t id, FrameData data, ResultHandler result_handler) = 0;
    virtual void async_read_frame(uint32_t* id, FrameData* data, ResultHandler result_handler) = 0;

    virtual void write_frame(uint32_t id, FrameData data) = 0;
    virtual void read_frame(uint32_t* id, FrameData* data) = 0;

    virtual void write_frame(uint32_t id, FrameData data, int timeout) = 0;
    virtual void read_frame(uint32_t* id, FrameData* data, int timeout) = 0;

protected:
    IDType id_type;

private:

};