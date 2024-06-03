# RobotDevKit
一个集成了机器人开发过程中常用的通信协议、控制算法的机器人开发工具包，主要面向Linux平台同时也支持stm32f1与stm32f4平台。目前该工具包包含PID增量式与位置式算法的实现、维特智能私有协议（惯性导航）、达妙电机与大疆电机的控制协议、常见编码电机的控制算法、USB串口类、USB转CAN总线功能、简单消息传输协议（传输字符串）、简单二进制传输协议（传输自定义结构体）、可靠二进制传输协议（带丢包超时重传功能）、底盘运动解算（两轮差速、麦克纳姆轮、全向轮）、VOFA可视化服务器。

# 目录说明
cmake 配置文件

docs 文档

modules 模块

platforms 在其他平台上的实现

samples 例程

# 安装方法
1，目标平台为arm32
```bash
cd build
sudo apt-get install libboost-dev:arm
sudo apt-get install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
cmake -D BUILD_ARM32=ON -D CMAKE_INSTALL_PREFIX=/usr/arm-linux-gnueabihf -D CMAKE_BUILD_TYPE=RELEASE ..
make install
```
2，目标平台为arm64
```bash
cd build
sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
sudo apt-get install libboost-dev:arm64
cmake -D BUILD_ARM64=ON -D CMAKE_INSTALL_PREFIX=/usr/aarch64-linux-gnu -D CMAKE_BUILD_TYPE=RELEASE ..
make install
```
3，目标平台为x86
```bash
cd build
sudo apt-get install libboost-dev
cmake -D CMAKE_BUILD_TYPE=RELEASE ..
make install
```

# 使用方法
1，交叉编译arm32程序
```cmake
cmake_minimum_required(VERSION 3.10)

#必须在project()前面
set(CMAKE_TOOLCHAIN_FILE "cmake/arm-gnueabi.toolchain.cmake")
set(CMAKE_CXX_STANDARD 20)

project(Demo VERSION 1.0)

find_package(RobotDevKit REQUIRED)
include_directories(${RobotDevKit_INCLUDE_DIRS})

add_executable(main src/main.cpp)
target_link_libraries(main ${RobotDevKit_LIBRARIES})
```

2，交叉编译arm64程序
```cmake
cmake_minimum_required(VERSION 3.10)

#必须在project()前面
set(CMAKE_TOOLCHAIN_FILE "cmake/aarch64-gnu.toolchain.cmake")
set(CMAKE_CXX_STANDARD 20)

project(Demo VERSION 1.0)

find_package(RobotDevKit REQUIRED)
include_directories(${RobotDevKit_INCLUDE_DIRS})

add_executable(main src/main.cpp)
target_link_libraries(main ${RobotDevKit_LIBRARIES})
```

3，直接在本地平台编译程序
```cmake
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

project(Demo VERSION 1.0)

find_package(RobotDevKit REQUIRED)
include_directories(${RobotDevKit_INCLUDE_DIRS})

add_executable(main src/main.cpp)
target_link_libraries(main ${RobotDevKit_LIBRARIES})
```

