cmake_minimum_required(VERSION 3.10)
project(ljstack)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(include/luajit21)
aux_source_directory(src DIR_SRCS)

add_executable(ljstack main.cpp)

target_link_libraries(ljstack DIR_SRCS)

