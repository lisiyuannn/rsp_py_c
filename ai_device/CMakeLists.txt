cmake_minimum_required(VERSION 3.10)
project(ai_device)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置调试标志
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")  # 添加-g调试符号
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g")

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_OBJECT_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/obj)

# 包含头文件目录
include_directories(include)

# 查找WiringPi库
find_library(WIRINGPI_LIBRARY wiringPi)
if(NOT WIRINGPI_LIBRARY)
    message(FATAL_ERROR "WiringPi library not found!")
endif()

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
    ${WIRINGPI_LIBRARY}  # 添加WiringPi库链接
)

# 添加GDB调试支持
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # 添加额外的调试标志
    target_compile_options(ai_device PRIVATE -Wall -Wextra -pedantic)
    # 为GDB设置更友好的调试信息
    target_compile_options(ai_device PRIVATE -ggdb3 -O0)
    # 禁用优化以确保调试准确性
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
endif()