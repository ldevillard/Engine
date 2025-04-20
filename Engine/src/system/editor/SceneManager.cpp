#include "system/editor/SceneManager.h"

#include "imgui.h"
#include "utils/serializer/Serializer.h"

#pragma region Singleton Methods

// singleton override
void SceneManager::initialize()
{
    Singleton<SceneManager>::initialize();
}

#pragma endregion

#pragma region Public Methods

void SceneManager::Initialize()
{
    Get();
    
    instance->initialize();
}

void SceneManager::LoadScene(const std::string& scenePath, const std::string& sceneName)
{
    // Load the scene from the specified path
    Serializer::LoadSceneFromFile(scenePath, sceneName);
}

void SceneManager::ShowLoadingScreen(float loadingProgress)
{
    // block ui interactions
    ImGui::BeginDisabled(true);

    // loading window in the center of the screen
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(300, 80));
    ImGui::Begin("Loading", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
    {
        ImGui::Text("Loading...");
        ImGui::ProgressBar(loadingProgress, ImVec2(-1.0f, 0.0f));
    }
    ImGui::End();

    // unblock ui interactions
    ImGui::EndDisabled();
}

#pragma endregion
