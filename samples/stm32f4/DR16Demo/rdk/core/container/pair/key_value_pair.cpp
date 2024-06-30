/*
 * @author BusyBox
 * @date 2024/5/29
 * @version 1.0
 * @git https://github.com/ExceptionQWQ/RobotDevKit
 */

#include "rdk/core/container/pair/key_value_pair.h"


/*
 * @brief 默认构造一个pair
 */
KeyValuePair::KeyValuePair()
{

}

/*
 * @brief 从字符串配对列表中构造pair，如xx=aa|yy=bb|zz=cc的形式
 */
KeyValuePair::KeyValuePair(const std::string& pair_list)
{
    std::istringstream iss(pair_list);
    std::string pair;
    while (std::getline(iss, pair, '|')) {
        std::istringstream pair_iss(pair);
        std::string key;
        std::string value;
        if (std::getline(pair_iss, key, '=') && std::getline(pair_iss, value, '=')) {
            pair_container[key] = value;
        }
    }
}

KeyValuePair::~KeyValuePair()
{

}

/*
 * @brief 获取字符串配对列表
 * @details 用字符串表示pair，方便通信传输
 */
std::string KeyValuePair::get_pair_list()
{
    std::string list;
    bool is_first_pair = true;
    for(auto iter_b = pair_container.begin(); iter_b != pair_container.end(); ++iter_b) {
        std::string pair = iter_b->first + "=" + iter_b->second;
        if (is_first_pair) list += pair, is_first_pair = false;
        else list += "|" + pair;
    }
    return list;
}

/*
 * @brief 放置一个键值配对
 * @param key 键
 * @param value 值
 */
void KeyValuePair::put(const std::string& key, const std::string& value)
{
    pair_container[key] = value;
}

/*
 * @brief 根据键获取值
 * @param key 键
 */
std::string KeyValuePair::get(const std::string& key)
{
    if (pair_container.find(key) == pair_container.end()) return "";
    return pair_container[key];
}