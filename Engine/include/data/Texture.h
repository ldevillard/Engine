#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <utils/glad/glad.h>

enum TextureFormat
{
	RGB,
	RGBA
};

struct TextureParam
{
	bool flip;
	TextureFormat format;
};

class Texture
{
public:
	Texture(const char* texturePath, std::string name, TextureParam params = TextureParam());
	Texture(const Texture& other);

	unsigned int ID;
	std::string Name;
	std::string Path;
	GLuint64 TextureHandle;
};