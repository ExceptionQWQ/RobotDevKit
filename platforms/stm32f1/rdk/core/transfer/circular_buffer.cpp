/*
 * @author BusyBox
 * @date 2024/4/24
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/transfer/circular_buffer.h"


CircularBuffer::CircularBuffer()
{

}

CircularBuffer::~CircularBuffer()
{

}

/*
 * @brief 将元素t放入循环缓冲末尾，缓冲满了则会跳过这个元素
 */
void CircularBuffer::push_back(uint8_t t)
{
    if (full()) return ;
    buff[tail++] = t;
    ++sz;
    if (tail >= buff_size) tail = 0;
}

/*
 * @brief 弹出循环缓冲前面的元素，如果缓冲为空则会抛出BufferEmptyException异常
 */
uint8_t CircularBuffer::pop_front()
{
    uint8_t ret = buff[head++];
    --sz;
    if (head >= buff_size) head = 0;
    return ret;
}

/*
 * @brief 检查循环缓冲是否塞满
 */
bool CircularBuffer::full()
{
    if (sz == buff_size) return true;
    else return false;
}

/*
 * @brief 检查是否可以弹出一个元素
 */
bool CircularBuffer::available()
{
    if (sz == 0) return false;
    else return true;
}