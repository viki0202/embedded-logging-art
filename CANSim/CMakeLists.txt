cmake_minimum_required(VERSION 3.10)
project(CANSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

add_executable(cansim
    src/utils.cpp
    src/logger.cpp
    src/can.cpp
    src/diag.cpp
    src/can_dictionary.cpp
    src/scheduler.cpp
    src/tasks.cpp
    src/main.cpp
)

target_include_directories(cansim
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(cansim PRIVATE
    $<$<CONFIG:Debug>:-g -O0 -Wall -Wextra>
)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")