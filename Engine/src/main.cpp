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
#include "Shader.h"
#include "data/Texture.h"
#include "data/Material.h"
#include "Camera.h"
#include "component/Model.h"
#include "debug/DebugMenu.h"
#include "render/FrameBuffer.h"
#include "system/Editor.h"
#include "system/EntityManager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 1920;
unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 1.f, 15.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.f, 2.f, 10.f);

// frame buffer pointer
FrameBuffer* ptr = nullptr;

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

	FrameBuffer sceneBuffer = FrameBuffer(SCR_WIDTH / 2, SCR_HEIGHT / 2);
	ptr = &sceneBuffer;

	// build and compile shader programs
	Shader shader("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl");

	EntityManager::CreateInstance();

	Entity entity1 = Entity("cube");
	Model model1 = Model("resources/models/backpack/cube.obj", &shader);
	entity1.AddComponent(&model1);

	Entity entity2 = Entity("cube2");
	Model model2 = Model("resources/models/cube/cube.obj", &shader);
	entity2.AddComponent(&model2);
	entity2.transform->Position = glm::vec3(0.f, -2.f, 5.f);
	entity2.transform->Scale *= 0.25f;

	// Have to handle materials in models
	Material material = Material::None;

	bool wireframeMode = false;
	int trianglesNumber = EntityManager::Get()->GetNumberOfTriangles();

	// setup editor settings
	EditorSettings settings;
	settings.FrameBuffer = &sceneBuffer;
	settings.SCR_WIDTH = &SCR_WIDTH;
	settings.SCR_HEIGHT = &SCR_HEIGHT;
	settings.Wireframe = &wireframeMode;
	settings.DeltaTime = &deltaTime;
	settings.CameraSpeed = &camera.MovementSpeed;
	settings.LightPosition = &lightPos;
	settings.TrianglesNumber = &trianglesNumber;

	Editor::CreateInstance(window, settings);
	Editor::Get()->SelectEntity(&entity1);

	sceneBuffer.RescaleFrameBuffer(SCR_WIDTH, SCR_HEIGHT);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logicw
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (wireframeMode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// input
		processInput(window);

		// render
		sceneBuffer.Bind(); // Lier le framebuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Utiliser le shader
		shader.Use();

		shader.SetVec3("objectColor", 1.0f, 1.0f, 1.0f);
		shader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
		shader.SetVec3("lightPos", lightPos);
		shader.SetVec3("viewPos", camera.Position);
		shader.SetBool("wireframe", wireframeMode);

		//material uniforms
		shader.SetVec3("material.ambient", material.Ambient);
		shader.SetVec3("material.diffuse", material.Diffuse);
		shader.SetVec3("material.specular", material.Specular);
		shader.SetFloat("material.shininess", material.Shininess);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.SetMat4("projection", projection);
		shader.SetMat4("view", view);

		//glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.0f, -0.05f, 4.675f)); // translate it down so it's at the center of the scene
		//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f) * 0.5f);	// it's a bit too big for our scene, so scale it down
		////model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, .0f));
		////model = glm::rotate(model, glm::radians(15.0f) * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		//shader.SetMat4("model", model);
		//object.Draw(shader);
		
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

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
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

