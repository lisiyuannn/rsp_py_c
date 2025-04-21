cmake_minimum_required(VERSION 3.10)
project(ai_device)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_OBJECT_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/obj)

# 包含头文件目录
include_directories(include)

# 创建静态库目标
add_library(digital_disp STATIC src/digital_disp.cpp)
add_library(led STATIC src/led.cpp)
add_library(motor STATIC src/motor.cpp)
add_library(threadpool STATIC src/threadpool.cpp)

# 创建可执行文件
add_executable(ai_device src/main.cpp)

# 链接静态库到可执行文件
target_link_libraries(ai_device 
    digital_disp
    led
    motor
    threadpool
)