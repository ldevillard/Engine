#include "data/CubeMap.h"

#include <iostream>
#include <utils/glad/glad.h> // include glad to get all the required OpenGL headers
#include <render/stb_image.h>

#include "render/Shader.h"
#include "system/editor/Editor.h"
#include "system/entity/EntityManager.h"

#pragma region Public Methods

CubeMap::CubeMap(const std::vector<std::string>& faces, Shader* sh)
	: ID(0), shader(sh)
{
    setupScreenCube();
    loadTextures(faces);
    shader->Use();
    shader->SetInt("skybox", 0);
}

void CubeMap::Draw() const
{
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    
    shader->Use();

    const EditorCamera* camera = Editor::Get().GetCamera();
    const Light* light = EntityManager::Get().GetMainLight();
    
    glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
    shader->SetMat4("view", view);
    shader->SetMat4("projection", camera->GetProjectionMatrix(static_cast<float>(SCR_WIDTH), static_cast<float>(SCR_WIDTH)));

    shader->SetVec3("lightColor", GetSkyboxLightColor()); 
    
    // skybox cube
    glBindVertexArray(screenCube.VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
}

glm::vec3 CubeMap::GetSkyboxLightColor() const
{
    const Light* light = EntityManager::Get().GetMainLight();

    if (light == nullptr)
        return nightColor;

    glm::vec3 lightDirection = light->GetDirection();
    float lightAngle = std::clamp((lightDirection.y + 1.0f) * 0.5f, 0.0f, 1.0f);
    return glm::mix(dayColor, nightColor, lightAngle) * (1 + log(light->Intensity));
}

#pragma endregion

#pragma region Private Methods

void CubeMap::loadTextures(const std::vector<std::string>& faces)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CubeMap::setupScreenCube()
{
    screenCube = ScreenCube();

    glGenVertexArrays(1, &screenCube.VAO);
    glGenBuffers(1, &screenCube.VBO);
    glBindVertexArray(screenCube.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenCube.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenCube.vertices), &screenCube.vertices, GL_STATIC_DRAW);
    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
}

#pragma endregion