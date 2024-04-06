#include "system/editor/Editor.h"

#include <glfw3.h>

#include "system/Time.h"
#include "system/entity/EntityManager.h"
#include "utils/ImGui_Utils.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <maths/glm/gtx/matrix_decompose.hpp>

// singleton instance
Editor* Editor::instance = nullptr;

#pragma region Public Methods

Editor::Editor(GLFWwindow* window, EditorSettings params)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	ImGui_Utils::SetPurpleTheme();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// setup
	parameters = params;
	inspector = Inspector();
}

Editor::~Editor()
{
	// clean ImGui resources
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Editor::CreateInstance(GLFWwindow* window, EditorSettings params)
{
	if (instance == nullptr)
	{
		instance = new Editor(window, params);
	}
}

void Editor::DestroyInstance()
{
	if (instance != nullptr)
	{
		delete instance;
		instance = nullptr;
	}
}

Editor* Editor::Get()
{
	return instance;
}

const EditorSettings& Editor::GetSettings() const
{
	return parameters;
}

void Editor::Render()
{
	float w = static_cast<float>(*parameters.SCR_WIDTH);
	float h = static_cast<float>(*parameters.SCR_HEIGHT);

	// Rendering ImGui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowSize(ImVec2(w, h));

	// Setup docking space
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::Begin("DockSpace", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking);
	ImGuiID dockspace_id = ImGui::GetID("DockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	ImGui::End();

	ImGui::SetNextWindowSizeConstraints(ImVec2(300, -1), ImVec2(600, -1));

	// Clean the screen
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Render the scene and the UI
	renderSettings();
	renderHierarchy();
	renderInspector();
	renderScene(w, h);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::SelectEntity(Entity* entity)
{
	selectedEntity = entity;
}

void Editor::SetCamera(EditorCamera* camera)
{
	editorCamera = camera;
}

#pragma endregion

#pragma region Private Methods

void Editor::renderScene(float width, float height)
{
	ImGui::Begin("Scene", nullptr);
	{
		ImGui::BeginChild("GameRender");

		float width = ImGui::GetContentRegionAvail().x;
		float height = ImGui::GetContentRegionAvail().y;

		*parameters.SCR_WIDTH = static_cast<unsigned int>(width);
		*parameters.SCR_HEIGHT = static_cast<unsigned int>(height);;
		ImGui::Image(
			reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(parameters.FrameBuffer->GetFrameTexture())),
			ImGui::GetContentRegionAvail(),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
	}
	transformGizmo(width, height);
	ImGui::EndChild();
	ImGui::End();
}

void Editor::renderInspector()
{
	ImGui::Begin("Inspector");
	if (selectedEntity != nullptr)
	{
		inspector.Inspect(selectedEntity);
	}
	ImGui::End();
}

void Editor::renderHierarchy()
{
	ImGui::Begin("Hierarchy");
	{
		EntityManager* manager = EntityManager::Get();
		if (manager != nullptr)
		{
			for (Entity* entity : manager->GetEntities())
			{
				if (ImGui::Selectable(entity->Name.c_str(), selectedEntity == entity))
				{
					SelectEntity(entity);
				}
			}
		}
	}
	ImGui::End();
}

void Editor::renderSettings()
{
	ImGui::Begin("Editor");
	ImGui::Text("FPS: %.1f", Time::Get()->GetFrameRate());
	ImGui::Text("Triangles: %d", *parameters.TrianglesNumber);
	ImGui::Separator();
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Gizmos"))
	{
		ImGui_Utils::DrawBoolControl("Gizmos", parameters.Gizmo, 100.f);
		ImGui_Utils::DrawBoolControl("Bounding Box", parameters.BoundingBoxGizmo, 100.f);

		int transformControl = static_cast<int>(gizmoOperation); // ImGuizmo::OPERATION::TRANSLATE
		ImGui_Utils::DrawComboBoxControl("Transform", transformControl, gizmoOperations, 100.f);
		gizmoOperation = static_cast<ImGuizmo::OPERATION>(transformControl);

		ImGui::TreePop();
	}
	ImGui::NewLine();
	ImGui::Separator();
	ImGui_Utils::DrawBoolControl("Wireframe", *parameters.Wireframe, 100.f);
	ImGui_Utils::DrawBoolControl("BlinnPhong", *parameters.BlinnPhong, 100.f);
	ImGui_Utils::DrawFloatControl("Camera Speed", *parameters.CameraSpeed, 5.f, 100.f);
	ImGui::End();
}

void Editor::transformGizmo(float width, float height)
{
	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, width, height);

	glm::mat4 projection = editorCamera->GetProjectionMatrix(width, height);
	glm::mat4 view = editorCamera->GetViewMatrix();
	glm::mat4 model = selectedEntity->transform->GetTransformMatrix();


	ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), 
		gizmoOperation, ImGuizmo::MODE::LOCAL,
		glm::value_ptr(model));

	if (ImGuizmo::IsUsing())
	{
		// Decompose the model matrix
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		
		glm::decompose(model, scale, rotation, translation, skew, perspective);

		if (scale.x > 0 && scale.y > 0 && scale.z > 0)
		{
			glm::vec3 deltaRotation = glm::eulerAngles(rotation) - glm::radians(selectedEntity->transform->Rotation);

			selectedEntity->transform->Position = translation;
			selectedEntity->transform->Rotation += glm::degrees(deltaRotation);
			selectedEntity->transform->Scale = scale;
		}
	}
}

#pragma endregion