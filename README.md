# RobotDevKit
机器人开发工具包

# 目录说明
cmake 配置文件

docs 文档

modules 模块

platforms 在其他平台上的实现

samples 例程

# 安装方法
1，在x86平台交叉编译arm32程序使用下面的指令安装
```bash
cd build
sudo apt-get install libboost-dev:arm
sudo apt-get install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
cmake -D CMAKE_INSTALL_PREFIX=/usr/arm-linux-gnueabihf -D CMAKE_BUILD_TYPE=RELEASE ..
make install
```
2，在x86平台交叉编译arm64程序使用下面的指令安装
```bash
cd build
sudo apt-get install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
sudo apt-get install libboost-dev:arm64
cmake -D CMAKE_INSTALL_PREFIX=/usr/aarch64-linux-gnu -D CMAKE_BUILD_TYPE=RELEASE ..
make install
```
3，如果直接在arm平台上编译则使用下面的指令安装
```bash
cd build
sudo apt-get install libboost-dev
cmake -D CMAKE_INSTALL_PREFIX=/usr/local -D CMAKE_BUILD_TYPE=RELEASE ..
make install
```

# 使用方法
1，在x86平台交叉编译arm32程序
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

2，在x86平台交叉编译arm64程序
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

3，直接在arm平台编译程序
```cmake
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

project(Demo VERSION 1.0)

find_package(RobotDevKit REQUIRED)
include_directories(${RobotDevKit_INCLUDE_DIRS})

add_executable(main src/main.cpp)
target_link_libraries(main ${RobotDevKit_LIBRARIES})
```

