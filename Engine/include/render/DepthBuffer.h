#pragma once

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

    static constexpr unsigned int WIDTH = 4096;
    static constexpr unsigned int HEIGHT = 4096;
};
