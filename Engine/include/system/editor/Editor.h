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

inline unsigned int SCENE_WIDTH = 1280;
inline unsigned int SCENE_HEIGHT = 720;

inline unsigned int RAYTRACED_SCENE_WIDTH = 1280;
inline unsigned int RAYTRACED_SCENE_HEIGHT = 720;

inline unsigned int SCR_WIDTH = 1280;
inline unsigned int SCR_HEIGHT = 720;

struct EditorSettings
{
	// scene
	bool* Wireframe = nullptr;
	int* TrianglesNumber = nullptr;
	
	bool Gizmo = true;
	bool BoundingBoxGizmo = false;

	// ray tracing
	bool RayTracing = true;
	int RaysPerPixel = 1;
	int MaxBounces = 2;
};

class Editor
{
public:
	Editor(GLFWwindow* window, EditorSettings params);
	~Editor();

	// singleton
	static void CreateInstance(GLFWwindow* win, EditorSettings params);
	static void DestroyInstance();
	static Editor* Get();

	const EditorSettings& GetSettings() const;
	const EditorCamera* GetCamera() const;
	FrameBuffer* GetSceneBuffer() const;
	FrameBuffer* GetRaytracingBuffer() const;
	FrameBuffer* GetOutlineBuffer(int idx) const;
	const Entity* GetSelectedEntity() const;

	void RenderCamera(Shader* shader);
	void RenderEditor();
	void ProcessInputs();
	void ScrollCallback(double xoffset, double yoffset);
	void MouseCallback(double xposIn, double yposIn);
	void FramebufferSizeCallback(int width, int height);

	void SelectEntity(Entity* entity);

private:
	// singleton
	static Editor* instance;

	// render functions
	void renderScene(float width, float height);
	void renderRayTracer();
	void renderInspector();
	void renderHierarchy();
	void renderSettings();
	void transformGizmo(float width, float height);

	// member references
	Entity* selectedEntity = nullptr;
	EditorCamera* editorCamera = nullptr;
	FrameBuffer* sceneBuffer = nullptr;
	FrameBuffer* outlineBuffer[2] = { nullptr };
	FrameBuffer* raytracingBuffer = nullptr;
	GLFWwindow* window = nullptr;

	EditorSettings parameters;
	Inspector inspector;

	// counter for frame rate
	float frameCounter = 1.0f;
	float frameRate = 60.0f;

	// selected gizmo
	ImGuizmo::OPERATION	gizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE		gizmoSpace = ImGuizmo::LOCAL;
	const std::vector<const char*> gizmoOperations = { "Translate", "Rotate", "Scale" };
	const std::vector<const char*> gizmoSpaces = { "Local", "World" };

	// mouse and screen settings
	float lastX = SCENE_WIDTH / 2.0f;
	float lastY = SCENE_HEIGHT / 2.0f;
	bool firstMouse = true;
};