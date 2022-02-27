cmake_minimum_required(VERSION 3.10)

project(at24cxx VERSION 0.1)

option(CROSS_COMP "Is need cross compile project?" false)

if(${CROSS_COMP})
    message("Cross compile mode")
endif(${CROSS_COMP})

include(cross_compile.cmake)

file(GLOB SRCS "src/*.c" "src/*.cpp" main.c)
include_directories(GLOB "include")

add_executable(demo ${SRCS})