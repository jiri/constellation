cmake_minimum_required(VERSION 3.8)
project(Wiring)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system graph)
find_package(fmt REQUIRED)
find_package(glfw3 REQUIRED)

add_subdirectory(libraries/gsl)
add_subdirectory(libraries/glad)
add_subdirectory(libraries/imgui)

include_directories(
        ${Boost_INCLUDE_DIRS}
        ${fmt_INCLUDE_DIRS}
        ${glfw3_INCLUDE_DIRS}
)
link_libraries(
        ${Boost_LIBRARIES}
        fmt
        glfw
        GSL
        glad
        imgui
)

set(SOURCE_FILES
        src/main.cpp
)
add_executable(Wiring ${SOURCE_FILES})
