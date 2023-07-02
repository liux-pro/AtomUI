cmake_minimum_required(VERSION 3.24)
project(u8g2_sdl C)

set(CMAKE_C_STANDARD 11)


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})


# 添加源文件
file(GLOB SOURCES u8g2/csrc/*.c u8g2/sys/sdl/common/*.c)
include_directories(u8g2/csrc/)


add_executable(u8g2_sdl main.c ${SOURCES})
target_link_libraries(u8g2_sdl  ${SDL2_LIBRARIES})

