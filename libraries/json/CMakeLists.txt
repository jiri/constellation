cmake_minimum_required(VERSION 3.9)

project(json)

add_library(json INTERFACE)
#target_sources(json INTERFACE json.hpp)

target_include_directories(json INTERFACE ${PROJECT_SOURCE_DIR})
