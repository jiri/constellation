cmake_minimum_required(VERSION 3.9)

project(glad)

#find_package(OpenGL REQUIRED)

add_library(glad src/glad.c)

target_include_directories(glad PUBLIC include/ ${OPENGL_INCLUDE_DIR})
target_link_libraries(glad PUBLIC ${OPENGL_LIBRARIES})
