#pragma once

// glm
#include <glm/glm.hpp>

// Phong model
class Material
{
public:
	// These vectors are the colors of the different lights
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	
	// This impact the radius of the specular light
	float Shininess = 0.6f;

	// Default colors methods
	void SetAsEmerald();
	void SetAsChrome();
	void SetAsTurquoise();
};