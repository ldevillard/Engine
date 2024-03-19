#pragma once

#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>

class FrameBuffer
{
public:
	FrameBuffer();
	FrameBuffer(float width, float height);
	~FrameBuffer();

	unsigned int& GetFrameTexture();

	void RescaleFrameBuffer(float width, float height);
	void Bind() const;
	void Unbind() const;

private:
	unsigned int fbo;
	unsigned int texture;
	unsigned int rbo;
};