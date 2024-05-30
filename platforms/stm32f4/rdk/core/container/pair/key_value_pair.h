/*
 * @author BusyBox
 * @date 2024/5/29
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#pragma once

#include <map>
#include <string>
#include <sstream>


class KeyValuePair
{
public:
    /*
     * @brief 默认构造一个pair
     */
    KeyValuePair();
    /*
     * @brief 从字符串配对列表中构造pair，如xx=aa|yy=bb|zz=cc的形式
     */
    KeyValuePair(const std::string& pair_list);
    ~KeyValuePair();

    /*
     * @brief 获取字符串配对列表
     * @details 用字符串表示pair，方便通信传输
     */
    std::string get_pair_list();

    /*
     * @brief 放置一个键值配对
     * @param key 键
     * @param value 值
     */
    void put(const std::string& key, const std::string& value);

    /*
     * @brief 根据键获取值
     * @param key 键
     */
    std::string get(const std::string& key);

private:
    std::map<std::string, std::string> pair_container;

};