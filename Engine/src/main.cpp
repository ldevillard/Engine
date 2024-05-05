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
#include "system/Input.h"
#include "system/Time.h"
#include "system/editor/Gizmo.h"
#include "system/editor/Outliner.h"
#include "system/editor/Editor.h"
#include "system/entity/EntityManager.h"
#include "data/Texture.h"
#include "data/Material.h"
#include "component/Model.h"
#include "data/Color.h"
#include "component/Light.h"
#include "render/Shader.h"
#include "render/ComputeShader.h"
#include "render/RayTracer.h"
#include "debug/DebugMenu.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCENE_WIDTH, SCENE_HEIGHT, "Engine", NULL, NULL);
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
	Input::Initialize(window);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSwapInterval(1); // enable vsync

	// configure global opengl state
	glEnable(GL_DEPTH_TEST); // enable depth testing
	glEnable(GL_STENCIL_TEST); // enable stencil testing
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // configure the stencil buffer to replace the value of the stencil buffer if the depth test fails

	// build and compile shader programs
	Shader shader("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl");
	Shader gizmoShader("shaders/gizmo/GizmoVertexShader.glsl", "shaders/gizmo/GizmoFragmentShader.glsl");
	Shader outlineShader("shaders/outline/OutlineVertexShader.glsl", "shaders/outline/OutlineFragmentShader.glsl");
	Shader outlineDilateShader("shaders/outline/OutlineQuadVertexShader.glsl", "shaders/outline/OutlineDilatingFragmentShader.glsl");
	ComputeShader outlineBlitShader("shaders/compute/BlitTexturesComputeShader.glsl", glm::uvec2(SCENE_WIDTH, SCENE_HEIGHT));
	
	Shader raytracingShader("shaders/raytracing/RayTracerVertexShader.glsl", "shaders/raytracing/RayTracerFragmentShader.glsl");
	ComputeShader accumulateShader("shaders/compute/AccumulateComputeShader.glsl", glm::uvec2(RAYTRACED_SCENE_WIDTH, RAYTRACED_SCENE_HEIGHT));

	Outliner::Initialize(&outlineShader, &outlineDilateShader, &outlineBlitShader);
	RayTracer::Initialize(&raytracingShader, &accumulateShader);
	Gizmo::InitGizmos(&gizmoShader);

	EntityManager::CreateInstance(&shader);
	Model::LoadPrimitives();

	Entity* entity1 = EntityManager::Get()->CreateEntity("Sphere Light");
	Model* model1 = entity1->AddComponent<Model>(PrimitiveType::SpherePrimitive, Material::Default);
	entity1->transform->SetPosition({ 0.f, 0.f, -25.f });
	entity1->transform->SetScale({ 6.5f, 6.5f, 6.5f });

	Entity* entity2 = EntityManager::Get()->CreateEntity("Sphere Ground");
	Model* model2 = entity2->AddComponent<Model>(PrimitiveType::SpherePrimitive, Material::Prune);
	entity2->transform->SetPosition({ 1.05f, -3.88f, -9.44f });
	entity2->transform->SetScale({ 1.f, 1.f, 1.f });

	Entity* entity3 = EntityManager::Get()->CreateEntity("Sphere1");
	Model* model3 = entity3->AddComponent<Model>(PrimitiveType::SpherePrimitive, Material::Turquoise);
	entity3->transform->SetPosition({ 0.f, -461.30f, -10.f });
	entity3->transform->SetScale({ 456.57f, 456.57f, 456.57f });

	Entity* entity4 = EntityManager::Get()->CreateEntity("Sphere2");
	Model* model4 = entity4->AddComponent<Model>(PrimitiveType::SpherePrimitive);
	entity4->transform->SetPosition({ 0.94f, -4.31f, -10.79f });
	entity4->transform->SetScale({ .47f, .47f, .47f });

	Entity* entity5 = EntityManager::Get()->CreateEntity("Cube");
	Model* model5 = entity5->AddComponent<Model>(PrimitiveType::CubePrimitive);
	entity5->transform->SetPosition({ 2.24f, -3.88f, -10.98f });
	entity5->transform->SetScale({ 1.f, 1.f, 1.f });

	Entity* lightEntity = EntityManager::Get()->CreateEntity("DirectionalLight");
	Light* light = lightEntity->AddComponent<Light>(Light::Directional, Color::White);
	light->Intensity = 1.f;
	lightEntity->transform->SetPosition({ 0.f, 7.5f, 15.f });
	lightEntity->transform->SetRotation({ -45.f, 0.f, 0.f });

	bool wireframeMode = false;
	int trianglesNumber = EntityManager::Get()->GetNumberOfTriangles();

	// setup editor settings
	EditorSettings settings;
	settings.Wireframe = &wireframeMode;
	settings.TrianglesNumber = &trianglesNumber;

	Editor::CreateInstance(window, settings);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		Time::Update();

		if (wireframeMode)
		{
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// render
		Editor::Get()->GetSceneBuffer()->Bind(); // bind to framebuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		Editor::Get()->RenderCamera(&shader);
		EntityManager::Get()->ComputeEntities();

		// unbind framebuffer
		Editor::Get()->GetSceneBuffer()->Unbind();

		// raytracing
		if (Editor::Get()->GetSettings().RayTracing)
		{
			RayTracer::Draw();
		}

		// editor
		Editor::Get()->RenderEditor();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// cleanup
	Editor::DestroyInstance();
	EntityManager::DestroyInstance();

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
	SCR_WIDTH = width;
	SCR_HEIGHT = height;

	Editor* editor = Editor::Get();
	if (editor)
	{
		editor->FramebufferSizeCallback(width, height);
		editor->RenderEditor();
	}
}

