#pragma once
#include <utils/glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/matrix_transform.hpp>
#include <maths/glm/gtc/type_ptr.hpp>

class ComputeShader
{
public:
    // constructor reads and builds the shader
    ComputeShader(const char* path, glm::uvec2 workSize);
    ~ComputeShader();
    // use/activate the shader
    void Use();
    void Dispatch(glm::uvec2 workCount);
    void Wait();

    void SetWorkSize(glm::uvec2 workSize);
    void SetTexture(unsigned int id);
    void SetTextures(unsigned int id1, unsigned int id2);

    // utility uniform functions
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetFloat4(const std::string& name, float v1, float v2, float v3, float v4) const;
    void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec2(const std::string& name, float x, float y) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec3(const std::string& name, float x, float y, float z) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetVec4(const std::string& name, float x, float y, float z, float w) const;
    void SetMat2(const std::string& name, const glm::mat2& mat) const;
    void SetMat3(const std::string& name, const glm::mat3& mat) const;
    void SetMat4(const std::string& name, glm::mat4 mat) const;

    // the program ID
    unsigned int ID = 0;
    std::string Path = {};

private:
    void checkCompileErrors(unsigned int shader, std::string type);

    glm::uvec2 workSize = glm::uvec2(0);
    unsigned int tempTexture = 0;
};