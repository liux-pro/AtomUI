cmake_minimum_required(VERSION 3.24)
project(atomUI)


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})





file(GLOB SRCS_EMULATOR emulator/u8g2/csrc/*.c emulator/u8g2/sys/sdl/common/*.c emulator/u8g2/cppsrc/*.cpp emulator/Arduino.c)
include_directories(emulator/u8g2/csrc/)
include_directories(emulator/u8g2/cppsrc/)
include_directories(emulator)


include_directories(.)
add_executable(atomUI main.cpp  ${SRCS_EMULATOR})


target_link_libraries(atomUI ${SDL2_LIBRARIES})

