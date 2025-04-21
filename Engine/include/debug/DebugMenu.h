#pragma once

// glm
#include <maths/glm/glm.hpp>

// imgui
#include "imgui_impl_glfw.h"

#include "render/FrameBuffer.h"

struct DebugParameters
{
	bool* Wireframe = nullptr;
	float* DeltaTime = nullptr;
	float* CameraSpeed = nullptr;
	glm::vec3* LightPosition = nullptr;
	int* TrianglesNumber = nullptr;
};

class DebugMenu
{
public:
	DebugMenu(GLFWwindow* window);

	// free resources;
	void Terminate();

	void Render(FrameBuffer& frameBuffer);

	// utility
	void SetParameters(DebugParameters params);
	const DebugParameters& GetParameters();

private:
	DebugParameters parameters;

	bool isNullParam();
};