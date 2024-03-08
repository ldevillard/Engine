#pragma once
#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <stb_image.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

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
	unsigned int ID;
	std::string Name;

	Texture(const char* texturePath, std::string name, TextureParam params = TextureParam());
};