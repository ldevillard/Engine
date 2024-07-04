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
#include "system/entity/Entity.h"
#include "system/editor/EditorCamera.h"
#include "system/editor/ScreenSettings.h"
#include "render/FrameBuffer.h"
#include "data/template/Singleton.h"

class AxisGrid;
class CubeMap;

struct EditorSettings
{
	// scene
	bool Wireframe = false;
	int TrianglesNumber = 0;
	
	// gizmos
	bool Gizmo = true;
	bool Grid = true;
	bool BoundingBoxGizmo = false;
	bool BVHGizmo = false;

	// ray tracing
	bool Raytracing = false;
	bool BVH = true;
	int RaysPerPixel = 1;
	float DivergeStrength = 0.25f;
	int MaxBounces = 1;
	bool Accumulate = false;
};

// TODO: Think about split this class to not centralize all the logic here 
class Editor : public Singleton<Editor>
{
public:
	// singleton
	static void Initialize(GLFWwindow* win);

	~Editor();

	const EditorSettings& GetSettings() const;
	const EditorCamera* GetCamera() const;
	FrameBuffer* GetSceneBuffer() const;
	FrameBuffer* GetRaytracingBuffer() const;
	FrameBuffer* GetAccumulationBuffer() const;
	FrameBuffer* GetOutlineBuffer(int idx) const;
	const Entity* GetSelectedEntity() const;

	void PreRender();
	void RenderFrame(Shader* shader, CubeMap* cubemap, AxisGrid* grid);
	void RenderCamera(Shader* shader);
	void RenderEditor();
	void ScrollCallback(double xoffset, double yoffset);
	void MouseCallback(double xposIn, double yposIn);
	void FramebufferSizeCallback(int width, int height);

	void SelectEntity(Entity* entity);

protected:
	void initialize() override;

private:
	void processInputs();

	// render functions
	void renderTopBar();
	void renderScene(unsigned int width, unsigned int height);
	void renderRayTracer();
	void renderInspector();
	void renderHierarchy();
	void renderSettings();
	void transformGizmo(unsigned int width, unsigned int height);

	// context menu
	void showHierarchyContextMenu();
	void showEntityContextMenu();

	void resetEntitySelection();

	static constexpr float TOP_BAR_HEIGHT = 12.0f;

	// member references
	Entity* selectedEntity = nullptr;
	Entity* hoveredEntity = nullptr; // register UI hovered entity
	EditorCamera* editorCamera = nullptr;
	GLFWwindow* window = nullptr;

	// frame buffers
	FrameBuffer* sceneBuffer = nullptr;
	FrameBuffer* outlineBuffer[2] = { nullptr };
	FrameBuffer* raytracingBuffer = nullptr;
	FrameBuffer* accumulationBuffer = nullptr;

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