#pragma once
#include <utils/glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/matrix_transform.hpp>
#include <maths/glm/gtc/type_ptr.hpp>

class Shader
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();
    // use/activate the shader
    void Use();
    // utility uniform functions
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetUInt(const std::string& name, unsigned int value) const;
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
    void SetTextureHandle(const std::string& name, GLuint64 handle) const;

    std::string FragmentPath = {};
    std::string VertexPath = {};

private:
	void checkCompileErrors(unsigned int shader, std::string type);
};

