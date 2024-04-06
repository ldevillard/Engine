#pragma once

#include <iostream>
#include <utils/glad/glad.h>
#include <maths/glm/glm.hpp>

class FrameBuffer
{
public:
	FrameBuffer();
	FrameBuffer(GLsizei width, GLsizei height);
	~FrameBuffer();

	unsigned int& GetFrameTexture();

	void RescaleFrameBuffer(unsigned int width, unsigned int height);
	void Bind() const;
	void Unbind() const;

private:
	unsigned int fbo;
	unsigned int texture;
	unsigned int rbo;
};