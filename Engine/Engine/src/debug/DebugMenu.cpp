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

void DebugMenu::Render()
{
   if (isNullParam())
      return;

   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();

   float fps = 0;
   if (*m_Parameters.DeltaTime > 0)
      fps = 1.0f / *m_Parameters.DeltaTime;

   if (*m_Parameters.Wireframe)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   ImGui::Begin("Editor");
   ImGui::Text("FPS: %.1f", fps);
   ImGui::Checkbox("Wireframe", &*m_Parameters.Wireframe);
   ImGui::InputFloat("Camera Speed", m_Parameters.CameraSpeed);
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
   m_Parameters = params;
}

const DebugParameters& DebugMenu::GetParameters()
{
   return  m_Parameters;
}

#pragma endregion

#pragma endregion


#pragma region Private Methods

bool DebugMenu::isNullParam()
{
   if (m_Parameters.CameraSpeed == nullptr)
      return true;
   if (m_Parameters.Wireframe == nullptr)
      return true;
   if (m_Parameters.DeltaTime == nullptr)
      return true;

   return false;
}

#pragma endregion