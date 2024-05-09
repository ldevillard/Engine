#pragma once
#include <utils/glad/glad.h> // include glad to get all the required OpenGL headers
#include <render/stb_image.h>

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
	Texture(const char* texturePath, std::string name, TextureParam params = TextureParam());
	Texture(const Texture& other);

	unsigned int ID;
	std::string Name;
	std::string Path;
};