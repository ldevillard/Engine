#pragma once

#include <string>
#include <vector>

#include "data/mesh/MeshData.h"

class Shader;

class CubeMap
{
public:
	CubeMap(const std::vector<std::string>& faces, Shader* sh);

	void Draw() const;

private:
	void loadTextures(const std::vector<std::string>& faces);
	void setupScreenCube();

private:
	unsigned int ID;
	ScreenCube screenCube;
	Shader* shader;
};