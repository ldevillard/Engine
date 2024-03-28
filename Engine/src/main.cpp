// std
#include <iostream>

// libs
#include <glad/glad.h>
#include <glfw3.h>
#include <stb_image.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// user define
#include "system/GlobalSettings.h"

#include "Shader.h"
#include "data/Texture.h"
#include "data/Material.h"
#include "component/Model.h"
#include "debug/DebugMenu.h"
#include "system/editor/Editor.h"
#include "system/EntityManager.h"
#include "system/Time.h"
#include "utils/Gizmo.h"
#include "data/Color.h"
#include "component/Light.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

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

	FrameBuffer sceneBuffer = FrameBuffer(SCR_WIDTH / 2, SCR_HEIGHT / 2);
	ptr = &sceneBuffer;

	// build and compile shader programs
	Shader shader("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl");
	Shader gizmoShader("shaders/gizmo/GizmoVertexShader.glsl", "shaders/gizmo/GizmoFragmentShader.glsl");

	Gizmo::InitGizmos(&gizmoShader);

	EntityManager::CreateInstance();
	Model::LoadPrimitives();

	/*Entity entity1 = Entity("cube", &shader);
	Model model1 = Model(PrimitiveType::CubePrimitive);
	entity1.AddComponent(&model1);*/

	Entity lightEntity = Entity("light", &shader);
	lightEntity.transform->Position = lightPos;
	Light light = Light();
	lightEntity.AddComponent(&light);

	Entity templeEntity = Entity("temple", &shader);
	Model templeModel = Model("resources/models/temple/Japanese_Temple.obj");
	templeEntity.AddComponent(&templeModel);

	bool wireframeMode = false;
	bool blinnPhong = true;
	int trianglesNumber = EntityManager::Get()->GetNumberOfTriangles();

	// setup editor settings
	EditorSettings settings;
	settings.FrameBuffer = &sceneBuffer;
	settings.SCR_WIDTH = &SCR_WIDTH;
	settings.SCR_HEIGHT = &SCR_HEIGHT;
	settings.Wireframe = &wireframeMode;
	settings.BlinnPhong = &blinnPhong;
	settings.CameraSpeed = &camera.MovementSpeed;
	settings.LightPosition = &lightPos;
	settings.TrianglesNumber = &trianglesNumber;

	Editor::CreateInstance(window, settings);
	Editor::Get()->SelectEntity(&lightEntity);

	sceneBuffer.RescaleFrameBuffer(SCR_WIDTH, SCR_HEIGHT);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		Time::Get()->Update();

		if (wireframeMode)
		{
			glPolygonMode(GL_BACK, GL_LINE);
			glPolygonMode(GL_FRONT, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT, GL_FILL);
			glPolygonMode(GL_BACK, GL_FILL);
		}

		// input
		processInput(window);

		// render
		sceneBuffer.Bind(); // Lier le framebuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		// Utiliser le shader
		shader.Use();

		shader.SetVec3("objectColor", 1.0f, 1.0f, 1.0f);
		shader.SetVec3("viewPos", camera.Position);
		shader.SetBool("wireframe", wireframeMode);
		shader.SetBool("blinn", blinnPhong);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.SetMat4("projection", projection);
		shader.SetMat4("view", view);
		
		EntityManager::Get()->ComputeEntities();

		// Unbind le framebuffer
		sceneBuffer.Unbind();

		// render
		Editor::Get()->Render();

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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	float deltaTime = Time::Get()->DeltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.SetSpeedFactor(2);
	else
		camera.SetSpeedFactor(1);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_HEIGHT = height;
	SCR_WIDTH = width;

	if (ptr != nullptr)
	{
		ptr->RescaleFrameBuffer(width, height);
	}

	if (Editor::Get() != nullptr)
	{
		Editor::Get()->Render();
	}
}

