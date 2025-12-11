#ifndef INPUT_HPP
#define INPUT_HPP
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <string>

class Input {
public:
    static void bind(const std::string& action, int key);
    static void unbind(const std::string& action);
    static int getKey(const std::string& action);

    static void update(GLFWwindow* window);  // Call this every frame

    static bool isDown(const std::string& action);
    static bool isPressed(const std::string& action);
    static bool isReleased(const std::string& action);

private:
    static std::unordered_map<std::string, int> keyBinds;
    static std::unordered_map<int, bool> currentState;
    static std::unordered_map<int, bool> previousState;
};

#endif // INPUT_HPP