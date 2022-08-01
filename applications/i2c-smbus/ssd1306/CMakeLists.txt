cmake_minimum_required(VERSION 3.0.0)
project(SSD1306 VERSION 0.1.0)

add_executable(demo main.c src/SSD1306.c)

target_include_directories(demo PUBLIC include)
target_link_libraries(demo m)