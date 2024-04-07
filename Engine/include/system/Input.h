#pragma once

#include <glfw3.h>

namespace Input
{
    void Initialize(GLFWwindow* _window);
    bool GetKeyDown(int key);
    bool GetKey(int key);
    bool GetKeyUp(int key);
}