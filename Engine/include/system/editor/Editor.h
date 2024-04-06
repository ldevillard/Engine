#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"

// openGL
#include <utils/glad/glad.h>

// glm
#include <maths/glm/glm.hpp>

#include "system/editor/Inspector.h"
#include "render/FrameBuffer.h"
#include "system/entity/Entity.h"
#include "system/editor/EditorCamera.h"

struct EditorSettings
{
	// frame
	FrameBuffer* FrameBuffer = nullptr;
	
	// screen resolution
	unsigned int* SCR_WIDTH = nullptr;
	unsigned int* SCR_HEIGHT = nullptr;

	// scene
	bool* Wireframe = nullptr;
	bool* BlinnPhong = nullptr;
	int* TrianglesNumber = nullptr;
	
	// camera
	float* CameraSpeed = nullptr;
	
	bool Gizmo = true;
	bool BoundingBoxGizmo = true;
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

	const EditorSettings& GetSettings() const;

	void Render();

	void SelectEntity(Entity* entity);
	void SetCamera(EditorCamera* camera);

private:
	// singleton
	static Editor* instance;

	// UI
	void renderScene(float width, float height);
	void renderInspector();
	void renderHierarchy();
	void renderSettings();

	void transformGizmo(float width, float height);

	EditorSettings parameters;
	Inspector inspector;

	// counter for frame rate
	float frameCounter = 1.0f;
	float frameRate = 60.0f;

	// entity
	Entity* selectedEntity = nullptr;

	// selected gizmo
	ImGuizmo::OPERATION	gizmoOperation = ImGuizmo::TRANSLATE;
	const std::vector<const char*> gizmoOperations = { "Translate", "Rotate", "Scale" };

	// camera
	EditorCamera* editorCamera = nullptr;
};