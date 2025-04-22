#pragma once

#include <string>

#include "data/template/Singleton.h"

class SceneManager : public Singleton<SceneManager>
{
public:
    // singleton
    static void Initialize();

    void LoadScene(const std::string& scenePath, const std::string& sceneName);

    void ShowLoadSceneDialog();
    void ShowSaveSceneDialog();
    void ShowLoadingScreen(float loadingProgress);

protected:
    void initialize() override;
};
