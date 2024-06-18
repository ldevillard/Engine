#include "debug/DebugMenu.h"

#pragma region Public Methods

DebugMenu::DebugMenu(GLFWwindow* window)
{
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO(); (void)io;
   ImGui::StyleColorsDark();
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init("#version 330");
}

void DebugMenu::Render(FrameBuffer& frameBuffer)
{
   if (isNullParam())
      return;

   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();

   float fps = 0;
   if (*parameters.DeltaTime > 0)
      fps = 1.0f / *parameters.DeltaTime;

   ImGui::Begin("Editor");
   ImGui::Text("FPS: %.1f", fps);
   ImGui::Text("Triangles: %d", *parameters.TrianglesNumber);
   ImGui::Checkbox("Wireframe", parameters.Wireframe);
   ImGui::InputFloat("Camera Speed", parameters.CameraSpeed);
   ImGui::DragFloat3("Light Position", &parameters.LightPosition->x, 0.025f);
   ImGui::End();

   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugMenu::Terminate()
{
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();
}

#pragma region Utility

void DebugMenu::SetParameters(DebugParameters params)
{
   parameters = params;
}

const DebugParameters& DebugMenu::GetParameters()
{
   return  parameters;
}

#pragma endregion

#pragma endregion


#pragma region Private Methods

bool DebugMenu::isNullParam()
{
   if (parameters.CameraSpeed == nullptr)
      return true;
   if (parameters.Wireframe == nullptr)
      return true;
   if (parameters.DeltaTime == nullptr)
      return true;

   return false;
}

#pragma endregion