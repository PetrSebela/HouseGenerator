cmake_minimum_required(VERSION 4.2.3)
project(HouseGenerator)
set(CMAKE_CXX_STANDARD 17)

# OpenGL
find_package(OpenGL)

#ImGUI
file(GLOB IMGUI_SOURCES
        external/imgui/*.cpp
        external/imgui/backends/imgui_impl_sdl3.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
)
add_library(ImGui STATIC ${IMGUI_SOURCES})
target_include_directories(ImGui PUBLIC external/imgui)

#SDL3
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

#GLAD
add_library(glad STATIC external/glad/src/gl.c)
target_include_directories(glad PUBLIC external/glad/include)

#GLM
add_subdirectory(external/glm)

include_directories("src")
file(GLOB HOUSE_GENERATOR_SRC src/*.cpp src/*.hpp)
add_executable(HouseGenerator ${HOUSE_GENERATOR_SRC})
target_link_libraries(ImGui PUBLIC SDL3::SDL3 OpenGL::GL)
target_link_libraries(HouseGenerator PRIVATE ImGui SDL3::SDL3 OpenGL::GL glad glm)