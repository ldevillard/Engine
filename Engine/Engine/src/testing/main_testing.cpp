//// std
//#include <iostream>
//
//// libs
//#include <glad/glad.h>
//#include <glfw3.h>
//#include <stb_image.h>
//
//// glm
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//// user define
//#include "Shader.h"
//#include "data/Texture.h"
//#include "data/Material.h"
//#include "Camera.h"
//#include "Model.h"
//#include "debug/DebugMenu.h"
//#include "render/FrameBuffer.h"
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(GLFWwindow* window);
//
//// settings
//unsigned int SCR_WIDTH = 1280;
//unsigned int SCR_HEIGHT = 720;
//
//// camera
//Camera camera(glm::vec3(0.0f, 1.f, 10.0f));
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//bool firstMouse = true;
//
//// timing
//float deltaTime = 0.0f;	// time between current frame and last frame
//float lastFrame = 0.0f;
//
//// lighting
//glm::vec3 lightPos(0.f, 2.f, 10.f);
//
//// global defined indices for OpenGL
//GLuint VAO; // vertex array object
//GLuint VBO; // vertex buffer object
//GLuint FBO; // frame buffer object
//GLuint RBO; // rendering buffer object
//GLuint texture_id; // the texture id we'll need later to create a texture 
//
//FrameBuffer* ptr = nullptr;
//
//void create_triangle()
//{
//    GLfloat vertices[] = {
//        -1.0f, -1.0f, 0.0f, // 1. vertex x, y, z
//        1.0f, -1.0f, 0.0f, // 2. vertex ...
//        0.0f, 1.0f, 0.0f // etc... 
//    };
//
//    glGenVertexArrays(1, &VAO);
//    glBindVertexArray(VAO);
//
//    glGenBuffers(1, &VBO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//    glEnableVertexAttribArray(0);
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//}
//
//int main()
//{
//    // glfw: initialize and configure
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    // glfw window creation
//    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Engine", NULL, NULL);
//    if (window == NULL)
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);
//
//    // glad: load all OpenGL function pointers
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//    // configure global opengl state
//    glEnable(GL_DEPTH_TEST);
//
//    // build and compile shader programs
//    Shader shader("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl");
//
//    bool wireframeMode = false;
//
//    IMGUI_CHECKVERSION();
//    ImGui::CreateContext();
//    ImGuiIO& io = ImGui::GetIO(); (void)io;
//    ImGui::StyleColorsDark();
//    ImGui_ImplGlfw_InitForOpenGL(window, true);
//    ImGui_ImplOpenGL3_Init("#version 330");
//
//    //create_triangle();
//    Model object("resources/models/cube/cube.obj");
//    Material material = Material::Ruby;
//
//    FrameBuffer sceneBuffer = FrameBuffer(SCR_WIDTH / 2, SCR_HEIGHT / 2);
//    ptr = &sceneBuffer;
//
//    sceneBuffer.RescaleFrameBuffer(SCR_WIDTH, SCR_HEIGHT);
//
//    // render loop
//    while (!glfwWindowShouldClose(window))
//    {
//
//        // per-frame time logicw
//        float currentFrame = static_cast<float>(glfwGetTime());
//        deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//
//        // input
//        processInput(window);
//
//        // render
//        sceneBuffer.Bind(); // Lier le framebuffer
//        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        // Utiliser le shader
//        shader.Use();
//
//        shader.SetVec3("objectColor", 1.0f, 1.0f, 1.0f);
//        shader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
//        shader.SetVec3("lightPos", lightPos);
//        shader.SetVec3("viewPos", camera.Position);
//        shader.SetBool("wireframe", wireframeMode);
//
//        //material uniforms
//        shader.SetVec3("material.ambient", material.Ambient);
//        shader.SetVec3("material.diffuse", material.Diffuse);
//        shader.SetVec3("material.specular", material.Specular);
//        shader.SetFloat("material.shininess", material.Shininess);
//
//        // view/projection transformations
//        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
//        glm::mat4 view = camera.GetViewMatrix();
//        shader.SetMat4("projection", projection);
//        shader.SetMat4("view", view);
//
//        glm::mat4 model = glm::mat4(1.0f);
//        model = glm::translate(model, glm::vec3(0.0f, -0.05f, 4.675f)); // translate it down so it's at the center of the scene
//        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f) * 0.5f);	// it's a bit too big for our scene, so scale it down
//        //model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, .0f));
//        //model = glm::rotate(model, glm::radians(15.0f) * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
//        shader.SetMat4("model", model);
//        //shader.SetBool("textured", true);
//		object.Draw(shader);
//
//        // Unbind le framebuffer
//        sceneBuffer.Unbind();
//
//        // Rendering ImGui
//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplGlfw_NewFrame();
//        ImGui::NewFrame();
//
//        // Nettoyer le framebuffer ImGui
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        ImGui::Begin("Scene");
//        {
//            ImGui::BeginChild("GameRender");
//
//            float width = ImGui::GetContentRegionAvail().x;
//            float height = ImGui::GetContentRegionAvail().y;
//
//            SCR_WIDTH = width;
//            SCR_HEIGHT = height;
//            ImGui::Image(
//                (ImTextureID)sceneBuffer.GetFrameTexture(),
//                ImGui::GetContentRegionAvail(),
//                ImVec2(0, 1),
//                ImVec2(1, 0)
//            );
//        }
//        ImGui::EndChild();
//        ImGui::End();
//
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
//
//    // glfw: terminate, clearing all previously allocated GLFW resources.
//    glfwTerminate();
//    return 0;
//}
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        camera.ProcessKeyboard(FORWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        camera.ProcessKeyboard(BACKWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        camera.ProcessKeyboard(LEFT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        camera.ProcessKeyboard(RIGHT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//        camera.ProcessKeyboard(DOWN, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
//        camera.ProcessKeyboard(UP, deltaTime);
//
//    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
//        camera.SetSpeedFactor(2);
//    else
//        camera.SetSpeedFactor(1);
//}
//
//// glfw: whenever the mouse moves, this callback is called
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
//    {
//        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//        firstMouse = true;
//        return;
//    }
//
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//    float xpos = static_cast<float>(xposIn);
//    float ypos = static_cast<float>(yposIn);
//
//    if (firstMouse)
//    {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//    lastX = xpos;
//    lastY = ypos;
//
//    // smooth
//    const float sensitivity = 0.5f;
//    xoffset *= sensitivity;
//    yoffset *= sensitivity;
//
//    camera.ProcessMouseMovement(xoffset, yoffset);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    camera.ProcessMouseScroll(static_cast<float>(yoffset));
//}
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    // make sure the viewport matches the new window dimensions; note that width and 
//    // height will be significantly larger than specified on retina displays.
//    glViewport(0, 0, width, height);
//    SCR_HEIGHT = height;
//    SCR_WIDTH = width;
//
//    if (ptr != nullptr)
//		ptr->RescaleFrameBuffer(width, height);
//}
//
