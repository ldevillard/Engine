#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// openGL
#include <glad/glad.h>

// glm
#include <glm/glm.hpp>

struct DebugParameters
{
	bool* Wireframe = nullptr;
	float* DeltaTime = nullptr;
	float* CameraSpeed = nullptr;
	glm::vec3* LightPosition = nullptr;
};

class DebugMenu
{
public:
	DebugMenu(GLFWwindow* window);

	// free resources;
	void Terminate();

	void Render();

	// utility
	void SetParameters(DebugParameters params);
	const DebugParameters& GetParameters();

private:
	DebugParameters parameters;

	bool isNullParam();
};