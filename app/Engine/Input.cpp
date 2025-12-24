#include "Input.hpp"

std::unordered_map<std::string, int> Input::keyBinds;
std::unordered_map<int, bool> Input::currentState;
std::unordered_map<int, bool> Input::previousState;

void Input::bind(const std::string& action, int key) {
    keyBinds[action] = key;
}

void Input::unbind(const std::string& action) {
    keyBinds.erase(action);
}

int Input::getKey(const std::string& action) {
    return keyBinds.count(action) ? keyBinds[action] : -1;
}

void Input::update(GLFWwindow* window) {
    // Save current state to previous
    previousState = currentState;

    for (const auto& [action, key] : keyBinds) {
        currentState[key] = glfwGetKey(window, key) == GLFW_PRESS;
    }
}

bool Input::isDown(const std::string& action) {
    int key = getKey(action);
    return key != -1 && currentState[key];
}

bool Input::isPressed(const std::string& action) {
    int key = getKey(action);
    return key != -1 && currentState[key] && !previousState[key];
}

bool Input::isReleased(const std::string& action) {
    int key = getKey(action);
    return key != -1 && !currentState[key] && previousState[key];
}
