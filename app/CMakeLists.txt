set(PROJECT_NAME "AudioPlayground2")

cmake_minimum_required(VERSION 4.0.1)
project(${PROJECT_NAME} VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(RTAUDIO REQUIRED rtaudio)

# Find OpenAL | Disabled, as not used.
#find_package(OpenAL REQUIRED)

set(SOURCE_FILES
    src/main.cpp
    engine/engine.cpp
    engine/rolling_buffer.cpp
    engine/input.cpp
    audio/audio.cpp
    audio/synth_engine.cpp
)

set(IMGUI_SRC
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/libs/imgui-knobs/imgui-knobs.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/libs/imgui
    ${CMAKE_SOURCE_DIR}/libs/imgui/backends
    ${CMAKE_SOURCE_DIR}/libs/imgui-knobs
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    engine/
    audio/
    src/simulation/
    src/
    libs/
)
target_include_directories(${PROJECT_NAME} PRIVATE ${RTAUDIO_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENAL_LIBRARY} glfw imgui OpenGL::GL ${RTAUDIO_LIBRARIES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
target_compile_options(${PROJECT_NAME} PRIVATE ${RTAUDIO_CFLAGS_OTHER})