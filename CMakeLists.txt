cmake_minimum_required(VERSION 3.24)
project(u8g2_sdl)



find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(.)


# 添加源文件
file(GLOB SOURCES u8g2/csrc/*.c u8g2/sys/sdl/common/*.c u8g2/cppsrc/*.cpp)
include_directories(u8g2/csrc/)
include_directories(u8g2/cppsrc/)


add_executable(u8g2_sdl main.cpp Arduino.c ${SOURCES})
target_link_libraries(u8g2_sdl  ${SDL2_LIBRARIES})

