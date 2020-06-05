cmake_minimum_required(VERSION 3.10)
project(ljstack)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(include/luajit21)
# 添加所有源文件
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCLIST)

# 将源文件打包成动态库
add_library(libljstack SHARED ${SRCLIST})

add_executable(ljstack  main.cpp)

target_link_libraries(ljstack
        libljstack
        )

