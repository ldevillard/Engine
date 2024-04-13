#include "render/ComputeShader.h"

#pragma region Public Methods

ComputeShader::ComputeShader(const char* path, glm::uvec2 workSize)
    : workSize(workSize)
    , Path(path)
{
    // 1. retrieve the shader source code from filePath
    std::string shaderCode;
    std::ifstream shaderFile;
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        shaderFile.open(path);
        std::stringstream shaderStream;
        // read file buffer content into stream
        shaderStream << shaderFile.rdbuf();
        // close file handler
        shaderFile.close();
        // convert stream into string
        shaderCode = shaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* code = shaderCode.c_str();
    // 2. compile shader
    unsigned int shader;
    // compute shader
    shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    checkCompileErrors(shader, "COMPUTE");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, shader);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(shader);
}

ComputeShader::~ComputeShader()
{
    // delete the shader program
    glDeleteProgram(ID);
}

void ComputeShader::Use()
{
    // use the shader program
    glUseProgram(ID);
}

void ComputeShader::Dispatch(glm::uvec2 workCount)
{
    // dispatch the compute shader
    glDispatchCompute((GLuint)ceil(workSize.x / workCount.x), (GLuint)ceil(workSize.y / workCount.y), 1);
}

void ComputeShader::Wait()
{
    // wait for the compute shader to finish
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ComputeShader::SetTexture(unsigned int id)
{
    tempTexture = id;
    glActiveTexture(GL_TEXTURE0);
    glBindImageTexture(0, id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
}

void ComputeShader::SetTextures(unsigned int id1, unsigned int id2)
{
    glActiveTexture(GL_TEXTURE0);
    glBindImageTexture(0, id1, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);

    glActiveTexture(GL_TEXTURE1);
    glBindImageTexture(1, id2, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
}


void ComputeShader::SetWorkSize(glm::uvec2 workSize)
{
    this->workSize = workSize;
}

#pragma region Utility

void ComputeShader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void ComputeShader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void ComputeShader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void ComputeShader::SetFloat4(const std::string& name, float v1, float v2, float v3, float v4) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), v1, v2, v3, v4);
}

void ComputeShader::SetVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void ComputeShader::SetVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void ComputeShader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void ComputeShader::SetVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void ComputeShader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void ComputeShader::SetVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void ComputeShader::SetMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ComputeShader::SetMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ComputeShader::SetMat4(const std::string& name, glm::mat4 mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

#pragma endregion

#pragma endregion

#pragma region Private Methods

void ComputeShader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

#pragma endregion