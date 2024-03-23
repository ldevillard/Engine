#include "system/editor/Editor.h"

#include <glfw3.h>

#include "system/Time.h"
#include "system/EntityManager.h"
#include "utils/ImGui_Utils.h"

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

void LabelWithColor(const char* label, ImGuiDataType data_type, float value)
{
	ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(data_type / ImGuiDataType_COUNT, 0.7f, 0.7f));
	ImGui::Text(" %c", label[0]);
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::DragScalar(label, data_type, &value);
}

void Editor::Render()
{
	// Rendering ImGui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowSize(ImVec2(*parameters.SCR_WIDTH, *parameters.SCR_HEIGHT));

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

	// Nettoyer le framebuffer ImGui
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui::Begin("Editor");
	ImGui::Text("FPS: %.1f", Time::Get()->GetFrameRate());
	ImGui::Text("Triangles: %d", *parameters.TrianglesNumber);
	ImGui::Separator();
	ImGui_Utils::DrawBoolControl("Wireframe", *parameters.Wireframe, 100.f);
	ImGui_Utils::DrawFloatControl("Camera Speed", *parameters.CameraSpeed, 5.f, 100.f);
	ImGui::End();

	// show selected entity properties
	ImGui::Begin("Inspector");
	if (selectedEntity != nullptr)
	{
		ImGui::Text(std::string("Name : " + selectedEntity->Name).c_str());
		ImGui::Separator();
		ImGui_Utils::DrawVec3Control("Position", selectedEntity->transform->Position);
		ImGui_Utils::DrawVec3Control("Rotation", selectedEntity->transform->Rotation);
		ImGui_Utils::DrawVec3Control("Scale", selectedEntity->transform->Scale, 1);
	}
	ImGui::End();
	//

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

	ImGui::Begin("Scene", nullptr);
	{
		ImGui::BeginChild("GameRender");

		float width = ImGui::GetContentRegionAvail().x;
		float height = ImGui::GetContentRegionAvail().y;

		*parameters.SCR_WIDTH = width;
		*parameters.SCR_HEIGHT = height;
		ImGui::Image(
			(ImTextureID)parameters.FrameBuffer->GetFrameTexture(),
			ImGui::GetContentRegionAvail(),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
	}
	ImGui::EndChild();
	ImGui::End();


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::SelectEntity(Entity* entity)
{
	selectedEntity = entity;
}

#pragma endregion
