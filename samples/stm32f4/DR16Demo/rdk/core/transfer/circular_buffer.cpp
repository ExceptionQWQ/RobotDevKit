/*
 * @author BusyBox
 * @date 2024/4/24
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/transfer/circular_buffer.h"

template<class T, std::size_t buff_size>
CircularBuffer<T, buff_size>::CircularBuffer()
{

}

template<class T, std::size_t buff_size>
CircularBuffer<T, buff_size>::~CircularBuffer()
{

}

/*
 * @brief 将元素t放入循环缓冲末尾，缓冲满了则会跳过这个元素
 */
template<class T, std::size_t buff_size>
void CircularBuffer<T, buff_size>::push_back(const T& t)
{
    if (full()) return ;
    buff[tail++] = t;
    ++sz;
    if (tail >= buff_size) tail = 0;
}

/*
 * @brief 弹出循环缓冲前面的元素，如果缓冲为空则会抛出BufferEmptyException异常
 */
template<class T, std::size_t buff_size>
T CircularBuffer<T, buff_size>::pop_front()
{
    if (!available()) {
        throw BufferEmptyException{"[CircularBuffer<T, buff_size>::pop_front] BufferEmptyException"};
    }
    T ret = buff[head++];
    --sz;
    if (head >= buff_size) head = 0;
    return ret;
}

/*
 * @brief 检查循环缓冲是否塞满
 */
template<class T, std::size_t buff_size>
bool CircularBuffer<T, buff_size>::full()
{
    if (sz == buff_size) return true;
    else return false;
}

/*
 * @brief 检查是否可以弹出一个元素
 */
template<class T, std::size_t buff_size>
bool CircularBuffer<T, buff_size>::available()
{
    if (sz == 0) return false;
    else return true;
}