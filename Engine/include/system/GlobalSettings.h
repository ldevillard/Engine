#pragma once

#include <glm/glm.hpp>
#include "Camera.h"
#include "render/FrameBuffer.h"

// this file is temporary, until we have a proper way to handle this
// need to implement classes for each of these, and have a way to create them

// settings
inline unsigned int SCR_WIDTH = 1280;
inline unsigned int SCR_HEIGHT = 720;

// camera
inline Camera camera(glm::vec3(0.0f, 5.f, 30.0f));
inline float lastX = SCR_WIDTH / 2.0f;
inline float lastY = SCR_HEIGHT / 2.0f;
inline bool firstMouse = true;

// frame buffer pointer
inline FrameBuffer* ptr = nullptr;
