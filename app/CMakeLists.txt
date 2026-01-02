cmake_minimum_required(VERSION 3.27)
project(AudioPlayground2)

# Output directory for binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)

# Disable all tests/examples globally
set(BUILD_TESTING OFF CACHE BOOL "Disable tests" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Disable examples" FORCE)
set(BUILD_JACK OFF CACHE BOOL "Disable JACK backend" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

# Core libraries
find_package(X11 REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# -------------------------
# RtAudio static library
# -------------------------
add_subdirectory(libs/rtaudio)  # Usually creates target "rtaudio"

# -------------------------
# RtMidi static library
# -------------------------
add_subdirectory(libs/rtmidi)   # Usually creates target "rtmidi"
# Force RtMidi to only build ALSA / Pulse, no JACK
# -------------------------
# ImGui static library
# -------------------------
set(IMGUI_SRC
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_demo.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/backends/imgui_impl_glfw.cpp
    libs/imgui/backends/imgui_impl_opengl3.cpp
    libs/imgui-knobs/imgui-knobs.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
    libs/imgui
    libs/imgui/backends
    libs/imgui-knobs
)

# -------------------------
# Source files
# -------------------------
set(SOURCE_FILES
    src/Main.cpp
    Engine/Engine.cpp
    Engine/Input.cpp
    Audio/DSP/Oscillator.cpp
    Audio/DSP/Voice.cpp
    Audio/Instruments/Instrument.cpp
    Audio/Instruments/NOscillator.cpp
    Audio/Audio.cpp
    Audio/Synth.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    Engine/
    Audio/
    Audio/DSP/
    Audio/Instruments/
    src/
    libs/
    libs/rtaudio/include
    libs/rtmidi
)

# -------------------------
# Link libraries
# -------------------------
# Use the actual CMake targets created by add_subdirectory
target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui
    rtaudio       # <- use the CMake target, not "-lrtaudio_static"
    rtmidi        # <- use the CMake target, not "-lrtmidi_static"
    glfw
    OpenGL::GL
    ${X11_LIBRARIES}
)

# Compile options
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
