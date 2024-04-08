#include "system/editor/Editor.h"

#include <glfw3.h>

#include "system/Time.h"
#include "system/Input.h"
#include "system/entity/EntityManager.h"
#include "utils/ImGui_Utils.h"
#include "maths/Math.h"
#include "physics/Physics.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <maths/glm/gtx/matrix_decompose.hpp>

// singleton instance
Editor* Editor::instance = nullptr;

#pragma region Public Methods

Editor::Editor(GLFWwindow* win, EditorSettings params) :
	window(win),
	parameters(params)
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
	editorCamera = new EditorCamera(glm::vec3(0.0f, 5.f, 30.0f));
	sceneBuffer = new FrameBuffer(SCR_WIDTH, SCR_HEIGHT);
	inspector = Inspector();
}

Editor::~Editor()
{
	// clean ImGui resources
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// clean
	delete editorCamera;
	delete sceneBuffer;
}

#pragma region Singleton Methods

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

#pragma endregion

#pragma region Getters

const EditorSettings& Editor::GetSettings() const
{
	return parameters;
}

const EditorCamera* Editor::GetCamera() const
{
	return editorCamera;
}

const FrameBuffer* Editor::GetSceneBuffer() const
{
	return sceneBuffer;
}

const Entity* Editor::GetSelectedEntity() const
{
	return selectedEntity;
}

#pragma endregion

#pragma region Rendering

void Editor::RenderCamera(Shader* shader)
{
	shader->Use();
	shader->SetVec3("viewPos", editorCamera->Position);
	shader->SetBool("wireframe", *parameters.Wireframe);
	shader->SetBool("blinn", *parameters.BlinnPhong);

	glm::mat4 projection = editorCamera->GetProjectionMatrix(static_cast<float>(SCR_WIDTH), static_cast<float>(SCR_HEIGHT));
	glm::mat4 view = editorCamera->GetViewMatrix();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
}

void Editor::RenderEditor()
{
	float w = static_cast<float>(SCR_WIDTH);
	float h = static_cast<float>(SCR_HEIGHT);

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

#pragma endregion

#pragma region Callbacks

void Editor::MouseCallback(double xposIn, double yposIn)
{
	if (!Input::GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstMouse = true;
		return;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	// smooth
	const float sensitivity = 0.5f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	editorCamera->ProcessMouseMovement(xoffset, yoffset);
}

// make sure the viewport matches the new window dimensions; note that width and 
// height will be significantly larger than specified on retina displays.
void Editor::FramebufferSizeCallback(int width, int height)
{
	glViewport(0, 0, width, height);

	SCR_WIDTH = width;
	SCR_HEIGHT = height;

	sceneBuffer->RescaleFrameBuffer(width, height);
}

void Editor::ScrollCallback(double xoffset, double yoffset)
{
	editorCamera->ProcessMouseScroll(static_cast<float>(yoffset));
}

#pragma endregion

// maybe make an input manager with KeyPressed, KeyReleased
void Editor::ProcessInputs()
{
	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	float deltaTime = Time::DeltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (Input::GetKey(GLFW_KEY_W))
		editorCamera->ProcessKeyboard(FORWARD, deltaTime);
	if (Input::GetKey(GLFW_KEY_S))
		editorCamera->ProcessKeyboard(BACKWARD, deltaTime);
	if (Input::GetKey(GLFW_KEY_A))
		editorCamera->ProcessKeyboard(LEFT, deltaTime);
	if (Input::GetKey(GLFW_KEY_D))
		editorCamera->ProcessKeyboard(RIGHT, deltaTime);
	if (Input::GetKey(GLFW_KEY_Q))
		editorCamera->ProcessKeyboard(DOWN, deltaTime);
	if (Input::GetKey(GLFW_KEY_E))
		editorCamera->ProcessKeyboard(UP, deltaTime);

	if (Input::GetKey(GLFW_KEY_LEFT_SHIFT)) // shift key
		editorCamera->SetSpeedFactor(2);
	else
		editorCamera->SetSpeedFactor(1);

	// gizmos shortcuts
	if (Input::GetKeyDown(GLFW_KEY_1))
		gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	if (Input::GetKeyDown(GLFW_KEY_2))
		gizmoOperation = ImGuizmo::OPERATION::ROTATE;
	if (Input::GetKeyDown(GLFW_KEY_3))
		gizmoOperation = ImGuizmo::OPERATION::SCALE;

	// mouse picker
	if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && !ImGuizmo::IsOver())
	{
		// need to check if the mouse is inside the scene window
		ImVec2 sceneWindowPos = ImGui::GetWindowPos();
		ImVec2 sceneWindowSize = ImGui::GetWindowSize();
		ImVec2 sceneWindowMin = ImVec2(sceneWindowPos.x, sceneWindowPos.y);
		ImVec2 sceneWindowMax = ImVec2(sceneWindowPos.x + sceneWindowSize.x, sceneWindowPos.y + sceneWindowSize.y);

		// compute mouse position relative to the imgui window
		ImVec2 mPos = ImGui::GetMousePos();
		ImVec2 mousePosScene = ImVec2(mPos.x - sceneWindowPos.x, mPos.y - sceneWindowPos.y);

		// check if the mouse is inside the scene window
		if (!(mPos.x >= sceneWindowMin.x && mPos.x <= sceneWindowMax.x &&
			mPos.y >= sceneWindowMin.y && mPos.y <= sceneWindowMax.y))
			return;

		glm::vec3 worldPos = Math::ScreenToWorldPoint(glm::vec2(mousePosScene.x, mousePosScene.y)
				, editorCamera->GetViewMatrix()
				, editorCamera->GetProjectionMatrix(static_cast<float>(SCR_WIDTH), static_cast<float>(SCR_HEIGHT))
				, glm::vec4(0, 0, SCR_WIDTH, SCR_HEIGHT));
		
		// Raycast
		glm::vec3 direction = glm::normalize(worldPos - editorCamera->Position);
		Ray ray = Ray(editorCamera->Position, direction);
		RaycastHit hit;
		Physics::EditorRaycast(ray, hit);
	
		if (hit.editorCollider != nullptr)
		{
			SelectEntity(hit.editorCollider->entity);
		}
		else
		{
			SelectEntity(nullptr);
		}
	}
}

void Editor::SelectEntity(Entity* entity)
{
	selectedEntity = entity;
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

		SCR_WIDTH = static_cast<unsigned int>(width);
		SCR_HEIGHT = static_cast<unsigned int>(height);;
		ImGui::Image(
			reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(sceneBuffer->GetFrameTexture())),
			ImGui::GetContentRegionAvail(),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		ProcessInputs();
	}
	if (selectedEntity != nullptr)
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
	ImGui::Text("FPS: %.1f", Time::FrameRate());
	ImGui::Text("Triangles: %d", *parameters.TrianglesNumber);
	ImGui::Separator();
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("Gizmos"))
	{
		ImGui_Utils::DrawBoolControl("Gizmos", parameters.Gizmo, 100.f);
		ImGui_Utils::DrawBoolControl("Bounding Box", parameters.BoundingBoxGizmo, 100.f);

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Transform"))
		{
			int transformControl = static_cast<int>(gizmoOperation); // ImGuizmo::OPERATION::TRANSLATE
			ImGui_Utils::DrawComboBoxControl("Mode", transformControl, gizmoOperations, 100.f);
			gizmoOperation = static_cast<ImGuizmo::OPERATION>(transformControl);
			
			int spaceControl = static_cast<int>(gizmoSpace); // ImGuizmo::MODE::LOCAL
			ImGui_Utils::DrawComboBoxControl("Space", spaceControl, gizmoSpaces, 100.f);
			gizmoSpace = static_cast<ImGuizmo::MODE>(spaceControl);

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
	ImGui::NewLine();
	ImGui::Separator();
	ImGui_Utils::DrawBoolControl("Wireframe", *parameters.Wireframe, 100.f);
	ImGui_Utils::DrawBoolControl("BlinnPhong", *parameters.BlinnPhong, 100.f);
	ImGui_Utils::DrawFloatControl("Camera Speed", editorCamera->MovementSpeed, 5.f, 100.f);
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

	// snapping
	bool snap = Input::GetKey(GLFW_KEY_LEFT_CONTROL);
	float snapValue = 0.5f; // Snap to 0.5m for translation and scale

	if (gizmoOperation == ImGuizmo::OPERATION::ROTATE)
		snapValue = 22.5f; // Snap to 22.5 degrees for rotation

	float snapValues[3] = { snapValue, snapValue, snapValue };

	ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), 
		gizmoOperation, gizmoSpace,
		glm::value_ptr(model),
		nullptr, snap ? snapValues : nullptr);

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