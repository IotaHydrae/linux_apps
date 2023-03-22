cmake_minimum_required(VERSION 3.10)

# set the project name and version.
project(learnopengl VERSION 0.1)

include_directories(
    "include"
)

# make c source file list.
list(APPEND C_SRCS main.c)

link_libraries(GLEW GLU  glfw3 GL X11 pthread Xrandr Xi dl m)

add_executable(demo ${C_SRCS})
