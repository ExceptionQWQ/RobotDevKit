/*
 * @author BusyBox
 * @date 2024/5/11
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <cstdint>

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
        uint8_t d8; //扩展帧使用
        uint8_t d9; //扩展帧使用
    };

    using ResultHandler = void(uint32_t id, CanBus::FrameData data);

    CanBus(IDType id_type);
    ~CanBus();


protected:
    IDType id_type;

private:

};