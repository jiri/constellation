cmake_minimum_required(VERSION 3.8)
project(Wiring)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system graph)
find_package(fmt REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${fmt_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES} fmt)

set(SOURCE_FILES
        src/main.cpp
)
add_executable(Wiring ${SOURCE_FILES})
