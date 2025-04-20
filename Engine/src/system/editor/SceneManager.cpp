#include "system/editor/SceneManager.h"

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

#pragma endregion