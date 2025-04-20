#pragma once

#include <string>

#include "data/template/Singleton.h"

class SceneManager : public Singleton<SceneManager>
{
public:
    // singleton
    static void Initialize();

    void LoadScene(const std::string& scenePath, const std::string& sceneName);

protected:
    void initialize() override;
};
