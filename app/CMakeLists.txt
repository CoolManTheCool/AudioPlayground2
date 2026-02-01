cmake_minimum_required(VERSION 3.27)
project(AudioPlayground2)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)

# Disable all tests/examples globally
set(BUILD_TESTING OFF CACHE BOOL "Disable tests" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Disable examples" FORCE)
set(BUILD_JACK OFF CACHE BOOL "Disable JACK backend" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

find_package(X11 REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(libs/rtaudio)

add_subdirectory(libs/rtmidi)

set(IMGUI_KNOBS_SRC
    libs/imgui-knobs/imgui-knobs.cpp
)

add_library(imgui-knobs STATIC ${IMGUI_KNOBS_SRC})
target_include_directories(imgui-knobs PUBLIC
    libs/imgui-knobs
)

set(SOURCE_FILES
    src/main.cpp
    DSP/Oscillator.cpp
    DSP/Voice.cpp
    DSP/Envelope.cpp
    Instruments/Instrument.cpp
    Instruments/NOscillator.cpp
    Playground/Audio.cpp
    Playground/Playground.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    Instruments/
    Playground/
    DSP/
    Utility/
    src/

    libs/
    libs/rtaudio/include
    libs/rtmidi
)

find_package(nova-engine REQUIRED)
target_link_libraries(imgui-knobs PRIVATE nova-engine::nova-engine)
target_link_libraries(${PROJECT_NAME} PRIVATE nova-engine::nova-engine)

target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui-knobs
    rtaudio
    rtmidi
    glfw
    OpenGL::GL
    ${X11_LIBRARIES}
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)