cmake_minimum_required(VERSION 3.10)
project(ljstack)

set(CMAKE_CXX_STANDARD 14)

# 是否为64位的luajit
add_definitions(-DLJ_GC64)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
if (${LJ_GC64})
    message(STATUS "Support Luajit: LJ_GC64")
else()
    message(STATUS "Support Luajit: LJ_GC32")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3")

include_directories(include)
include_directories(include/luajit21)
# 添加所有源文件
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRCLIST)

# 将源文件打包成动态库
add_library(libljstack SHARED ${SRCLIST})

add_executable(ljstack  main.cpp ${SRCLIST})

target_link_libraries(ljstack
    elf++
)

