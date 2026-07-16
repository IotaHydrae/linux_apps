cmake_minimum_required(VERSION 3.10)

# set the project name and version.
project(nuklear_x11 VERSION 0.1)

include_directories(
    "include"
)

# make c source file list.
list(APPEND C_SRCS main.c)

link_libraries(X11 Xext m)

add_executable(demo ${C_SRCS})
