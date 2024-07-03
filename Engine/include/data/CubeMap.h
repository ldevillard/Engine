#pragma once

#include <string>
#include <vector>

#include "data/mesh/MeshData.h"

class Shader;

class CubeMap
{
public:
	CubeMap(const std::vector<std::string>& faces, Shader* sh);
	~CubeMap();

	void Draw(const glm::mat4& view, const glm::mat4& projection) const;

	glm::vec3 GetSkyboxLightColor() const;

	unsigned int ID;

private:
	void loadTextures(const std::vector<std::string>& faces);
	void setupScreenCube();

private:
	ScreenCube screenCube;
	Shader* shader;

	static constexpr glm::vec3 dayColor = glm::vec3(1.0, 1.0, 1.0);
	static constexpr glm::vec3 nightColor = glm::vec3(0.1, 0.1, 0.15);
};