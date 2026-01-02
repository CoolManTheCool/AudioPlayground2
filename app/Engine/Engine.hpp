#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <cstdlib>
#include <iostream>
#include <vector>

struct FrameContext {
    int width;
    int height;
    double deltaTime;
};

class RollingBuffer_T {
public:
    RollingBuffer_T(size_t size) : size(size), index(0) {
        buffer.resize(size);
    }

    void add(double value) {
        buffer[index] = value;
        index = (index + 1) % size;
    }

    const std::vector<double>& getBuffer() const {
        return buffer;
    }
    size_t getIndex() const {
        return index;
    }

private:
    size_t size;
    size_t index;
    std::vector<double> buffer;
};

class Engine_T {
public:
    Engine_T();

    FrameContext beginFrame();

    const std::vector<double>& getFrameTimes() const;
    size_t getFrameTimeIndex() const;

    bool shouldClose() const;
    void setShouldClose(bool shouldClose);

    void endFrame();

    void destroy();

    // ImGui pointers
    ImGuiIO* io = nullptr;
    ImGuiStyle* style = nullptr;

private:
    static void glfwErrorCallback(int error, const char* description);

    void onFailure(const char* message, bool fatal = true) {
        std::cerr << "Error: " << message << std::endl;
        if (fatal) abort();
    }

    GLFWwindow* window = nullptr;
    double lastTime = 0.0;

    std::vector<double> frameTimes;
    size_t frameTimeIndex = 0; // Circular buffer index
};

extern Engine_T* Engine;

#endif // ENGINE_HPP
