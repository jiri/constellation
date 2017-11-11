cmake_minimum_required(VERSION 3.8)
project(Wiring)

set(CMAKE_CXX_STANDARD 17)

# Common libraries
find_package(Boost REQUIRED COMPONENTS system graph filesystem)
find_package(fmt REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(libraries/gsl)
add_subdirectory(libraries/glad)
add_subdirectory(libraries/imgui)

include_directories(SYSTEM src/)
include_directories(
        ${Boost_INCLUDE_DIRS}
        ${fmt_INCLUDE_DIRS}
        ${glfw3_INCLUDE_DIRS}
        ${GLM_LIBRARIES}
)
link_libraries(
        ${Boost_LIBRARIES}
        fmt
        glfw
        GSL
        glad
        imgui
        glm
)

# Graphics library
set(GRAPHICS_SOURCE_FILES
        src/Graphics/Program.cpp
)
add_library(${PROJECT_NAME}_Graphics ${GRAPHICS_SOURCE_FILES})

# Main
set(SOURCE_FILES
        src/main.cpp
        src/Util/Random.hpp
)
add_executable(Wiring ${SOURCE_FILES})

target_link_libraries(
        ${PROJECT_NAME} PUBLIC
        ${PROJECT_NAME}_Graphics
)

# Link shaders
add_custom_target(
        link_shaders
        ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/shd
        ${CMAKE_CURRENT_BINARY_DIR}/shd
        COMMENT "Linking shaders" VERBATIM
)
add_dependencies(${PROJECT_NAME} link_shaders)
