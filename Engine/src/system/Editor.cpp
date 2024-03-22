#include "system/Editor.h"
#include <glfw3.h>

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

	/*ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	colors[ImGuiCol_TitleBg] = ImVec4(0.314f, 0.231f, 0.38f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.408f, 0.298f, 0.502f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(1.f, 0.f, 0.f, 1.00f);*/

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

	frameCounter += *parameters.DeltaTime;

	if (frameCounter >= .1f)
	{
		frameCounter = 0;
		if (*parameters.DeltaTime > 0)
			frameRate = 1.0f / *parameters.DeltaTime;
	}

	ImGui::Text("FPS: %.1f", frameRate);
	ImGui::Text("Triangles: %d", *parameters.TrianglesNumber);
	ImGui::Checkbox("Wireframe", parameters.Wireframe);
	ImGui::InputFloat("Camera Speed", parameters.CameraSpeed);
	ImGui::End();

	// show selected entity properties
	ImGui::Begin("Inspector");
	if (selectedEntity != nullptr)
	{
		ImGui::Text(std::string("Selected Entity : " + selectedEntity->Name).c_str());
		ImGui::DragFloat3("Position", &selectedEntity->transform->Position.x, 0.025f);
		ImGui::DragFloat3("Rotation", &selectedEntity->transform->Rotation.x, 0.025f);
		ImGui::DragFloat3("Scale", &selectedEntity->transform->Scale.x, 0.025f);
	}
	ImGui::End();
	//

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

#pragma region Private Methods

#pragma endregion