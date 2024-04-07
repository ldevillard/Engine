#pragma once

#include <glfw3.h>
#include <maths/glm/glm.hpp>

namespace Input
{
    void Initialize(GLFWwindow* _window);
    bool GetKeyDown(int key);
    bool GetKey(int key);
    bool GetKeyUp(int key);

    // mouse
    bool GetMouseButtonDown(int button);
    bool GetMouseButton(int button);
    bool GetMouseButtonUp(int button);
    glm::vec2 GetMousePosition();
}