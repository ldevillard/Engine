#include <windows.h>

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
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	Input::Initialize(window);

	// icon
	int iconWidth, iconHeight, iconChannels;
	stbi_uc* iconPixels = stbi_load("resources/DevilEngine.png", &iconWidth, &iconHeight, &iconChannels, 4);

	GLFWimage images[1] = {};
	images[0].width = iconWidth;
	images[0].height = iconHeight;
	images[0].pixels = iconPixels;

	if (iconPixels)
	{
		glfwSetWindowIcon(window, 1, images);
		stbi_image_free(iconPixels);
	}
	else
	{
		std::cerr << "Failed to load icon" << std::endl;
	}

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
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

	// initialize systems
	Outliner::Initialize(&outlineShader, &outlineDilateShader, &outlineBlitShader);
	RayTracer::Initialize(&raytracingShader, &accumulateShader);
	Gizmo::InitGizmos(&gizmoShader);
	EntityManager::Initialize(&shader);
	Model::LoadPrimitives();
	Editor::Initialize(window);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		Time::Update();

		if (Editor::Get().GetSettings().Wireframe)
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
		Editor::Get().GetSceneBuffer()->Bind(); // bind to framebuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		Editor::Get().RenderCamera(&shader);
		EntityManager::Get().ComputeEntities();

		// unbind framebuffer
		Editor::Get().GetSceneBuffer()->Unbind();
		// set the multisampled texture to the rendered texture only if there's no selected entity because outliner modifying de raw texutre directly
		// TODO encapsulate all these Editor calls in the Editor class 
		if (Editor::Get().GetSelectedEntity() == nullptr)
		{
			Editor::Get().GetSceneBuffer()->Blit(); 
		}

		// raytracing
		if (Editor::Get().GetSettings().RayTracing)
		{
			RayTracer::Draw();
		}

		// editor
		Editor::Get().RenderEditor();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	Editor::Get().MouseCallback(xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Editor::Get().ScrollCallback(xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	if (width == 0 || height == 0)
		return;

	SCR_WIDTH = width;
	SCR_HEIGHT = height;

	Editor& editor = Editor::Get();
	editor.FramebufferSizeCallback(width, height);
	editor.RenderEditor();
}

