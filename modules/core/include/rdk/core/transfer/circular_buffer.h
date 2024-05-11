/*
 * @author BusyBox
 * @date 2024/4/24
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <cstdint>
#include <string>

template<class T, std::size_t buff_size>
class CircularBuffer
{
public:
    CircularBuffer();
    ~CircularBuffer();

    /*
     * @brief 将元素t放入循环缓冲末尾，缓冲满了则会跳过这个元素
    */
    void push_back(const T& t);
    /*
     * @brief 弹出循环缓冲前面的元素，如果缓冲为空则会抛出BufferEmptyException异常
     * @attention 如果缓冲为空，则会抛出BufferEmptyException异常
     */
    T pop_front();
    /*
     * @brief 检查循环缓冲是否塞满
     */
    bool full();
    /*
     * @brief 检查是否可以弹出一个元素
     */
    bool available();

    struct BufferEmptyException
    {
        std::string error;
    };

private:
    T buff[buff_size];
    std::size_t sz = 0;
    std::size_t head = 0;
    std::size_t tail = 0;

};