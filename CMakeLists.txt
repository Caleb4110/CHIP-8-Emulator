cmake_minimum_required(VERSION 3.24)

list(APPEND MY_SOURCES main.cpp)
set(PROJECT_NAME chip8emulator)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 98)


find_package(SDL2 REQUIRED COMPONENTS SDL2)
add_executable(${PROJECT_NAME} ${MY_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
