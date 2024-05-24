#include "system/editor/Editor.h"

#include <glfw3.h>

#include "maths/Math.h"
#include "physics/Physics.h"
#include "render/RayTracer.h"
#include "system/Time.h"
#include "system/Input.h"
#include "system/entity/EntityManager.h"
#include "utils/ImFileDialog.h"
#include "utils/ImGui_Utils.h"
#include "utils/serializer/Serializer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <maths/glm/gtx/matrix_decompose.hpp>

#pragma region Singleton Methods

// singleton override
void Editor::initialize()
{
	Singleton<Editor>::initialize();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	ImGui_Utils::SetPurpleTheme();

	ImGui_ImplGlfw_InitForOpenGL(instance->window, true);
	ImGui_ImplOpenGL3_Init("#version 430");

	// init file browser
	ifd::FileDialog::Instance().Initialize();
}

#pragma endregion

#pragma region Public Methods

void Editor::Initialize(GLFWwindow* win)
{
	Get();
	instance->window = win;

	// setup
	instance->editorCamera = new EditorCamera(glm::vec3(0.0f, 5.f, 30.0f));
	instance->sceneBuffer = new FrameBuffer(SCENE_WIDTH, SCENE_HEIGHT);
	instance->raytracingBuffer = new FrameBuffer(RAYTRACED_SCENE_WIDTH, RAYTRACED_SCENE_HEIGHT);
	instance->accumulationBuffer = new FrameBuffer(RAYTRACED_SCENE_WIDTH, RAYTRACED_SCENE_HEIGHT);
	instance->outlineBuffer[0] = new FrameBuffer(SCENE_WIDTH, SCENE_HEIGHT);
	instance->outlineBuffer[1] = new FrameBuffer(SCENE_WIDTH, SCENE_HEIGHT);
	instance->inspector = Inspector();

	instance->initialize();
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
	delete raytracingBuffer;
	delete accumulationBuffer;
	delete outlineBuffer[0];
	delete outlineBuffer[1];
}

#pragma region Getters

const EditorSettings& Editor::GetSettings() const
{
	return parameters;
}

const EditorCamera* Editor::GetCamera() const
{
	return editorCamera;
}

FrameBuffer* Editor::GetSceneBuffer() const
{
	return sceneBuffer;
}

FrameBuffer* Editor::GetRaytracingBuffer() const
{
	return raytracingBuffer;
}

FrameBuffer* Editor::GetAccumulationBuffer() const
{
	return accumulationBuffer;
}

FrameBuffer* Editor::GetOutlineBuffer(int idx) const
{
	// add assert to idx
	return outlineBuffer[idx];
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
	shader->SetBool("wireframe", parameters.Wireframe);

	glm::mat4 projection = editorCamera->GetProjectionMatrix(static_cast<float>(SCENE_WIDTH), static_cast<float>(SCENE_HEIGHT));
	glm::mat4 view = editorCamera->GetViewMatrix();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);
}

void Editor::RenderEditor()
{
	float w = static_cast<float>(SCENE_WIDTH);
	float h = static_cast<float>(SCENE_HEIGHT);

	// Rendering ImGui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowSize(ImVec2(w, h));

	// Setup docking space
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 pos(viewport->Pos.x, viewport->Pos.y + TOP_BAR_HEIGHT);
	ImVec2 size(viewport->Size.x, viewport->Size.y - TOP_BAR_HEIGHT);
	ImGui::SetNextWindowPos(pos);
	ImGui::SetNextWindowSize(size);
	ImGui::Begin("DockSpace", nullptr,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking);
	ImGuiID dockspace_id = ImGui::GetID("DockSpace");
	ImGui::DockSpace(dockspace_id);
	ImGui::End();

	// Clean the screen
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Render the scene and the UI
	renderTopBar();
	renderSettings();
	renderHierarchy();
	renderInspector();
	renderScene(w, h);
	renderRayTracer();

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
	const float sensitivity = .1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	editorCamera->ProcessMouseMovement(xoffset, yoffset);
}

// make sure the viewport matches the new window dimensions; note that width and 
// height will be significantly larger than specified on retina displays.
void Editor::FramebufferSizeCallback(int width, int height)
{
	glViewport(0, 0, width, height);

	SCENE_WIDTH = width;
	SCENE_HEIGHT = height;

	RAYTRACED_SCENE_WIDTH = width;
	RAYTRACED_SCENE_HEIGHT = height;

	sceneBuffer->RescaleFrameBuffer(width, height);
	raytracingBuffer->RescaleFrameBuffer(width, height);
	accumulationBuffer->RescaleFrameBuffer(width, height);
	outlineBuffer[0]->RescaleFrameBuffer(width, height);
	outlineBuffer[1]->RescaleFrameBuffer(width, height);
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

	// close the editor by press the escape key
	/*if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);*/

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
				, editorCamera->GetProjectionMatrix(static_cast<float>(SCENE_WIDTH), static_cast<float>(SCENE_HEIGHT))
				, glm::vec4(0, 0, SCENE_WIDTH, SCENE_HEIGHT));
		
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

void Editor::renderTopBar()
{
	// TO DO correct format for the save and load filedialog
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Scene"))
			{
				ifd::FileDialog::Instance().Save("SaveSceneDialog", "Save Scene", "Scene file (*.devil){.devil},.*");
			}
			if (ImGui::MenuItem("Load Scene"))
			{
				ifd::FileDialog::Instance().Open("LoadSceneDialog", "Load Scene", "Scene file (*.devil){.devil},.*");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (ifd::FileDialog::Instance().IsDone("SaveSceneDialog")) 
	{
		if (ifd::FileDialog::Instance().HasResult()) 
		{
			std::string filepath = ifd::FileDialog::Instance().GetResult().string();
			std::string filename = ifd::FileDialog::Instance().GetResult().stem().string();

			Serializer::SaveSceneToFile(filepath, filename);
		}
		ifd::FileDialog::Instance().Close();
	}

	if (ifd::FileDialog::Instance().IsDone("LoadSceneDialog"))
	{
		if (ifd::FileDialog::Instance().HasResult())
		{
			std::string filepath = ifd::FileDialog::Instance().GetResult().string();
			std::string filename = ifd::FileDialog::Instance().GetResult().stem().string();

			resetEntitySelection();
			Serializer::LoadSceneFromFile(filepath, filename);
		}
		ifd::FileDialog::Instance().Close();
	}
}

void Editor::renderScene(float width, float height)
{
	ImGui::Begin("Scene", nullptr);
	{
		ImGui::BeginChild("GameRender");

		float width = ImGui::GetContentRegionAvail().x;
		float height = ImGui::GetContentRegionAvail().y;

		SCENE_WIDTH = static_cast<unsigned int>(width);
		SCENE_HEIGHT = static_cast<unsigned int>(height);
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

void Editor::renderRayTracer()
{
	ImGui::Begin("RayTracer", nullptr);
	{
		ImGui::BeginChild("Render");

		float width = ImGui::GetContentRegionAvail().x;
		float height = ImGui::GetContentRegionAvail().y;

		RAYTRACED_SCENE_WIDTH = static_cast<unsigned int>(width);
		RAYTRACED_SCENE_HEIGHT = static_cast<unsigned int>(height);

		FrameBuffer* buffer = parameters.Accumulate ? accumulationBuffer : raytracingBuffer;

		ImGui::Image(
			reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(buffer->GetFrameTexture())),
			ImGui::GetContentRegionAvail(),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		ProcessInputs();
	}
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
		EntityManager& manager = EntityManager::Get();
		for (Entity* entity : manager.GetEntities())
		{
			if (ImGui::Selectable(entity->Name.c_str(), selectedEntity == entity))
			{
				SelectEntity(entity);
			}
			if (ImGui::GetID(entity->Name.c_str()) == ImGui::GetHoveredID())
			{
				hoveredEntity = entity;
			}
		}

		// handle right click menu
		showHierarchyContextMenu();
		showEntityContextMenu();
	}
	ImGui::End();
}

void Editor::renderSettings()
{
	// update triangle count
	parameters.TrianglesNumber = EntityManager::Get().GetNumberOfTriangles();

	ImGui::Begin("Editor");
	ImGui::Text("FPS: %.1f", Time::FrameRate());
	ImGui::Text("Frame time : %.1f ms", Time::DeltaTime * 1000);
	ImGui::Text("Triangles: %d", parameters.TrianglesNumber);
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
	ImGui_Utils::DrawBoolControl("Wireframe", parameters.Wireframe, 100.f);
	ImGui_Utils::DrawFloatControl("Camera Speed", editorCamera->MovementSpeed, 5.f, 100.f);

	ImGui::Separator();
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode("RayTracing"))
	{
		ImGui_Utils::DrawBoolControl("Enabled", parameters.RayTracing, 100.f);
		if (parameters.RayTracing)
		{
			ImGui_Utils::DrawBoolControl("Accumulate", parameters.Accumulate, 100.f);
			if (parameters.Accumulate)
			{
				// progress bar purpose is to show the velocity of the accumulation (based on 10 frames computations)
				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.6f, 0.3f, 0.6f, 1.0f));
				ImGui::ProgressBar(float(RayTracer::GetFrameCount() % 10) / 10.f, ImVec2(-1, 0), "");
				ImGui::PopStyleColor();
				ImGui::Text("Samples: %d", RayTracer::GetFrameCount());
				ImGui::NewLine();
			}
			ImGui_Utils::DrawIntControl("Max Bounces", parameters.MaxBounces, 1, 100.f);
			ImGui_Utils::DrawIntControl("Rays Per Pixel", parameters.RaysPerPixel, 1, 100.f);
		}
		ImGui::TreePop();
	}
	ImGui::NewLine();
	ImGui::Separator();

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

/* Show context menu when right click on the hierarchy                            
/* For the entity creation we generate a name to avoid duplicate name in the scene */
void Editor::showHierarchyContextMenu()
{
	if (!ImGui::IsAnyItemHovered() || ImGui::IsPopupOpen("HierarchyContextMenu"))
	{
		if (ImGui::BeginPopupContextWindow("HierarchyContextMenu"))
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Sphere"))
				{
					const std::string name = EntityManager::Get().GenerateNewEntityName("Sphere");
					Entity* entity = EntityManager::Get().CreateEntity(name);
					entity->transform->SetPosition(editorCamera->Position + editorCamera->Front * 15.0f);
					entity->AddComponent<Model>(SpherePrimitive);
					SelectEntity(entity);
				}
				if (ImGui::MenuItem("Cube"))
				{
					const std::string name = EntityManager::Get().GenerateNewEntityName("Cube");
					Entity* entity = EntityManager::Get().CreateEntity(name);
					entity->transform->SetPosition(editorCamera->Position + editorCamera->Front * 15.0f);
					entity->AddComponent<Model>(CubePrimitive);
					SelectEntity(entity);
				}
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
}

/* Show context menu when right click on an entity
/* We want to have the ability to delete/duplicate the entity */
void Editor::showEntityContextMenu()
{
	if (hoveredEntity == nullptr)
		return;

	if (!ImGui::IsAnyItemHovered() && !ImGui::IsPopupOpen("HierarchyContextEntity"))
	{
		hoveredEntity = nullptr;
		return;
	}
		
	if (ImGui::BeginPopupContextWindow("HierarchyContextEntity"))
	{
		if (ImGui::Selectable("Delete"))
		{
			Entity* entity = hoveredEntity;
			resetEntitySelection();
			EntityManager::Get().DestroyEntity(entity);
		}
		if (ImGui::Selectable("Duplicate"))
		{
			Entity* entity = hoveredEntity;
			resetEntitySelection();
			Entity* duplicatedEntity = EntityManager::Get().DuplicateEntity(entity);
			SelectEntity(duplicatedEntity);
		}
		ImGui::EndPopup();
	}
}

void Editor::resetEntitySelection()
{
	selectedEntity = nullptr;
	hoveredEntity = nullptr;
}

#pragma endregion