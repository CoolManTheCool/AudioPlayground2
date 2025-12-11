#include "engine.hpp"
#include "input.hpp"

#include <iostream>

static ImGuiIO& InitImGui() {
    // Helper function to create context before GetIO()
    ImGui::CreateContext();
    return ImGui::GetIO();
}

Engine_T::Engine_T() : io(InitImGui()), style(ImGui::GetStyle()) {
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) abort();
    
    // Setup GLFW - NOT FULLSCREEN & borderless
    // Submissive and breedable
    //glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    window = glfwCreateWindow(mode->width * 3 / 4, mode->height * 3 / 4, "Audio Playground 2", nullptr, nullptr);
    if (!window) abort();

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    lastTime = glfwGetTime();
    frameTimes.resize(5 * mode->refreshRate); // Seconds * (frames/second) aka frames

    IMGUI_CHECKVERSION();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Style for ImGui stuff
    ImGui::StyleColorsDark();
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowPadding = ImVec2(0.0f, 0.0f);

    // Backend init
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

FrameContext Engine_T::beginFrame() {
    FrameContext ctx;
    glfwGetWindowSize(window, &ctx.width, &ctx.height);
    ctx.deltaTime = glfwGetTime() - lastTime;
    lastTime = glfwGetTime();

    // Update frame times
    frameTimes[frameTimeIndex] = ctx.deltaTime;
    frameTimeIndex = (frameTimeIndex + 1) % frameTimes.size();
    
    glfwPollEvents();
    Input::update(window);
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags window_flags =
    ImGuiWindowFlags_NoDocking  | ImGuiWindowFlags_NoTitleBar            |
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize              |
    ImGuiWindowFlags_NoMove     | ImGuiWindowFlags_NoBringToFrontOnFocus |
    ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("DockSpace_Window", nullptr, window_flags);
    ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();

    
    return ctx;
}

const std::vector<double>& Engine_T::getFrameTimes() const {
    return frameTimes;
}

size_t Engine_T::getFrameTimeIndex() const {
    return frameTimeIndex;
}

bool Engine_T::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Engine_T::setShouldClose(bool shouldClose) {
    glfwSetWindowShouldClose(window, shouldClose);
}

void Engine_T::endFrame() {
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

void Engine_T::destroy() {
    // Cleanup code here
    if (window) {
        glfwDestroyWindow(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Engine_T::glfwErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    Engine.onFailure(error + description);
}

Engine_T Engine;