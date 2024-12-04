#pragma once

#include <iostream>
#include <utils/glad/glad.h>
#include <maths/glm/glm.hpp>

class DepthBuffer
{
public:
    DepthBuffer();
    ~DepthBuffer();

    unsigned int GetDepthTexture();

    void Bind() const;
    void Unbind() const;

private:
    unsigned int fbo;
    unsigned int depthMap;

    static constexpr unsigned int WIDTH = 1024;
    static constexpr unsigned int HEIGHT = 1024;
};
