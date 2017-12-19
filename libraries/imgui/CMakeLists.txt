cmake_minimum_required(VERSION 3.9)

project(imgui)

add_library(imgui
        src/imgui.cpp
        src/imgui_draw.cpp
        src/imgui_impl_sdl_gl3.cpp
)

target_include_directories(imgui PUBLIC include/)
target_link_libraries(imgui PUBLIC glad sdl2)
