cmake_minimum_required(VERSION 3.8)
project(Constellation)

set(CMAKE_CXX_STANDARD 17)

# Common libraries
find_package(fmt REQUIRED)
find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(libraries/gsl)
add_subdirectory(libraries/glad)
add_subdirectory(libraries/imgui)
add_subdirectory(libraries/json)
add_subdirectory(libraries/tinyobjloader)

include_directories(SYSTEM src/)
include_directories(
        ${fmt_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${GLM_LIBRARIES}
)
link_libraries(
        fmt
        sdl2
        GSL
        glad
        imgui
        glm
        json
        tinyobjloader
        c++experimental
)

# Graphics library
set(GRAPHICS_SOURCE_FILES
        src/Graphics/Program.hpp
        src/Graphics/Program.cpp
        src/Graphics/Geometry.hpp
        src/Graphics/Geometry.cpp
        src/Graphics/Mesh.hpp
        src/Graphics/Mesh.cpp
        src/Graphics/Camera.hpp
        src/Graphics/Camera.cpp
        src/Graphics/OrbitControls.hpp
        src/Graphics/OrbitControls.cpp
        src/Graphics/Framebuffer.hpp
        src/Graphics/Framebuffer.cpp
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
        src/Foundation/Infrastructures/Wiring.hpp
        src/Foundation/Infrastructures/Wiring.cpp
        src/Foundation/Infrastructures/Wireless.hpp
        src/Foundation/Infrastructures/Wireless.cpp
        src/Foundation/Infrastructures/Manual.hpp
        src/Foundation/Infrastructures/Manual.cpp
        src/Foundation/Components/Component.hpp
        src/Foundation/Components/Component.cpp
        src/Foundation/Components/CPU.hpp
        src/Foundation/Components/CPU.cpp
        src/Foundation/Components/Terminal.hpp
        src/Foundation/Components/Terminal.cpp
        src/Foundation/Systems/Energy.hpp
        src/Foundation/Systems/Energy.cpp
        src/Foundation/Systems/Video.hpp
        src/Foundation/Systems/Video.cpp
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
        src/Util/Filesystem.hpp
        src/Util/Adapters.hpp
        src/Util/FunctionTraits.hpp
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

# Link resources
add_custom_target(
        link_resources
        ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/res
        ${CMAKE_CURRENT_BINARY_DIR}/res
        COMMENT "Linking resources" VERBATIM
)
add_dependencies(${PROJECT_NAME} link_resources)
