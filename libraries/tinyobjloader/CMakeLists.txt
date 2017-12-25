project(tinyobjloader)

cmake_minimum_required(VERSION 3.6)

add_library(${PROJECT_NAME} src/tiny_obj_loader.cc)
target_include_directories(${PROJECT_NAME} PUBLIC include/)
