// std
#include <iostream>

// libs
#include <utils/glad/glad.h>
#include <glfw3.h>
#include <render/stb_image.h>

// glm
#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/matrix_transform.hpp>
#include <maths/glm/gtc/type_ptr.hpp>

// engine
#include "render/Shader.h"
#include "data/Texture.h"
#include "data/Material.h"
#include "component/Model.h"
#include "debug/DebugMenu.h"
#include "system/editor/Editor.h"
#include "system/entity/EntityManager.h"
#include "system/Time.h"
#include "system/editor/Gizmo.h"
#include "data/Color.h"
#include "component/Light.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Engine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	Time::CreateInstance();

	// build and compile shader programs
	Shader shader("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl");
	Shader gizmoShader("shaders/gizmo/GizmoVertexShader.glsl", "shaders/gizmo/GizmoFragmentShader.glsl");

	Gizmo::InitGizmos(&gizmoShader);

	EntityManager::CreateInstance(&shader);
	Model::LoadPrimitives();

	Entity entity1 = Entity("Plane", &shader);
	Model model1 = Model("resources/models/primitive/plane.obj", Material::Silver);
	entity1.transform->SetScale({ 25.f, 1.f, 25.f });
	entity1.AddComponent(&model1);

	Entity entity2 = Entity("Car", &shader);
	Model model2 = Model("resources/models/car/car.obj", Material::Turquoise);
	entity2.transform->SetPosition({ 0.f, 0.f, 0.f });
	entity2.transform->SetScale({ 0.05f, 0.05f, 0.05f });
	entity2.transform->SetRotation({ 0.f, 45.f, 0.f });
	entity2.AddComponent(&model2);

	/*Entity cubeEntity = Entity("Cube", &shader);
	Model cubeModel = Model(PrimitiveType::CubePrimitive, Material::Gold);
	cubeEntity.AddComponent(&cubeModel);
	cubeEntity.transform->SetPosition({ 0.f, 4.f, 0.f });
	cubeEntity.transform->SetScale({ 3.f, 1.f, 1.f });*/


	Entity lightEntity = Entity("DirectionalLight", &shader);
	Light light = Light(Light::Directional, Color::White);
	lightEntity.transform->SetPosition({ 0.f, 7.5f, 15.f });
	lightEntity.transform->SetRotation({ -45.f, 0.f, 0.f });
	lightEntity.AddComponent(&light);

	//Entity lightEntity2 = Entity("PointLight", &shader);
	//Light light2 = Light(Light::Point);
	//light2.Intensity = 5.1f;
	//light2.Radius = .3f;
	//lightEntity2.transform->SetPosition({ 4.90f, 2.6f, 9.1f });
	//lightEntity2.AddComponent(&light2);

	//Entity lightEntity3 = Entity("PointLight2", &shader);
	//Light light3 = Light(Light::Point);
	//light3.Intensity = 5.1f;
	//light3.Radius = .3f;
	//lightEntity3.transform->SetPosition({ 9.f, 2.6f, 5.f });
	//lightEntity3.AddComponent(&light3);

	//Entity lightEntity4 = Entity("SpotLight", &shader);
	//Light light4 = Light(Light::Spot);
	//lightEntity4.transform->SetPosition({ 3.f, 9.8f, 3.f });
	//lightEntity4.transform->SetRotation({ -160.f, 45.f, 0.f });
	//light4.CutOff = 19.f;
	//light4.OutCutOff = 30.f;
	//light4.Intensity = 3.5f;
	//lightEntity4.AddComponent(&light4);

	bool wireframeMode = false;
	bool blinnPhong = true;
	int trianglesNumber = EntityManager::Get()->GetNumberOfTriangles();

	// setup editor settings
	EditorSettings settings;
	settings.Wireframe = &wireframeMode;
	settings.BlinnPhong = &blinnPhong;
	settings.TrianglesNumber = &trianglesNumber;

	Editor::CreateInstance(window, settings);
	Editor::Get()->SelectEntity(&entity2);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		Time::Get()->Update();

		if (wireframeMode)
		{
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_BACK, GL_LINE);
			glPolygonMode(GL_FRONT, GL_LINE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT, GL_FILL);
			glPolygonMode(GL_BACK, GL_FILL);
		}

		// render
		Editor::Get()->GetSceneBuffer()->Bind(); // bind to framebuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		Editor::Get()->RenderCamera(&shader);
		EntityManager::Get()->ComputeEntities();

		// unbind framebuffer
		Editor::Get()->GetSceneBuffer()->Unbind();

		// editor
		Editor::Get()->RenderEditor();
		Editor::Get()->ProcessInputs();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// cleanup
	Editor::DestroyInstance();
	EntityManager::DestroyInstance();
	Time::DestroyInstance();

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	Editor* editor = Editor::Get();
	if (editor)
	{
		editor->MouseCallback(xposIn, yposIn);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Editor* editor = Editor::Get();
	if (editor)
	{
		editor->ScrollCallback(xoffset, yoffset);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Editor* editor = Editor::Get();
	if (editor)
	{
		editor->FramebufferSizeCallback(width, height);
		editor->RenderEditor();
	}
}

