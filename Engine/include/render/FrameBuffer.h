#pragma once

#include <utils/glad/glad.h>

class FrameBuffer
{
public:
    FrameBuffer();
    FrameBuffer(GLsizei width, GLsizei height, GLsizei samples);
    ~FrameBuffer();

    unsigned int GetFrameTexture();

    void RescaleFrameBuffer(unsigned int width, unsigned int height, unsigned int samples);
    void Bind() const;
    void Unbind() const;
    void Blit() const;
    void ReverseBlit() const;

private:
    unsigned int fbo;
    unsigned int texture;
    unsigned int rbo;

    unsigned int msaaFBO;
    unsigned int msaaTexture;
    unsigned int msaaRBO;

    GLsizei width;
    GLsizei height;
    GLsizei samples;
};
