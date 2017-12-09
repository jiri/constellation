cmake_minimum_required(VERSION 3.8)
project(Wiring)

set(CMAKE_CXX_STANDARD 17)

# Common libraries
find_package(fmt REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(libraries/gsl)
add_subdirectory(libraries/glad)
add_subdirectory(libraries/imgui)
add_subdirectory(libraries/json)

include_directories(SYSTEM src/)
include_directories(
        ${fmt_INCLUDE_DIRS}
        ${glfw3_INCLUDE_DIRS}
        ${GLM_LIBRARIES}
)
link_libraries(
        fmt
        glfw
        GSL
        glad
        imgui
        glm
        json
        c++experimental
)

# Graphics library
set(GRAPHICS_SOURCE_FILES
        src/Graphics/Program.hpp
        src/Graphics/Program.cpp
)
add_library(${PROJECT_NAME}_Graphics ${GRAPHICS_SOURCE_FILES})

# Foundation library
set(FOUNDATION_SOURCE_FILES
        src/Foundation/Universe.hpp
        src/Foundation/Universe.cpp
        src/Foundation/Debugger.hpp
        src/Foundation/Debugger.cpp
        src/Foundation/Infrastructures/Capabilities.hpp
        src/Foundation/Infrastructures/Capabilities.cpp
        src/Foundation/Infrastructures/Infrastructure.hpp
        src/Foundation/Infrastructures/Infrastructure.cpp
        src/Foundation/Infrastructures/Wireless.hpp
        src/Foundation/Infrastructures/Wireless.cpp
        src/Foundation/Infrastructures/Manual.hpp
        src/Foundation/Infrastructures/Manual.cpp
        src/Foundation/Components/Component.hpp
        src/Foundation/Components/Component.cpp
        src/Foundation/Components/CPU.hpp
        src/Foundation/Components/CPU.cpp
        src/Foundation/Systems/Energy.hpp
        src/Foundation/Systems/Energy.cpp
        src/Foundation/Systems/Picture.hpp
        src/Foundation/Systems/Picture.cpp
        src/Foundation/Systems/Text.hpp
        src/Foundation/Systems/Text.cpp
        src/Foundation/Systems/System.hpp
        src/Foundation/Systems/System.cpp
)
add_library(${PROJECT_NAME}_Foundation ${FOUNDATION_SOURCE_FILES})

# Main
set(SOURCE_FILES
        src/main.cpp
        src/Util/Random.hpp
        src/Util/Parse.hpp
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(
        ${PROJECT_NAME} PUBLIC
        ${PROJECT_NAME}_Graphics
        ${PROJECT_NAME}_Foundation
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
