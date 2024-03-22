#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// openGL
#include <glad/glad.h>

// glm
#include <glm/glm.hpp>

#include "render/FrameBuffer.h"
#include "Entity.h"

struct EditorSettings
{
	// frame
	FrameBuffer* FrameBuffer = nullptr;
	float* DeltaTime = nullptr;
	
	// screen resolution
	unsigned int* SCR_WIDTH = nullptr;
	unsigned int* SCR_HEIGHT = nullptr;

	// scene
	bool* Wireframe = nullptr;
	glm::vec3* LightPosition = nullptr;
	int* TrianglesNumber = nullptr;
	
	// camera
	float* CameraSpeed = nullptr;
};

class Editor
{
public:
	Editor(GLFWwindow* window, EditorSettings params);
	~Editor();

	// singleton
	static void CreateInstance(GLFWwindow* window, EditorSettings params);
	static void DestroyInstance();
	static Editor* Get();

	void Render();

	void SelectEntity(Entity* entity);

	// utility
	//void SetParameters(EditorSettings params);
	//const EditorSettings& GetParameters();

private:
	// singleton
	static Editor* instance;

	EditorSettings parameters;

	// counter for frame rate
	float frameCounter = 1.0f;
	float frameRate = 60.0f;

	// entity
	Entity* selectedEntity = nullptr;

	void setDarkTheme();
};