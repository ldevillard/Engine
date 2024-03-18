#pragma once

// glm
#include <glm/glm.hpp>

// Phong model
class Material
{
public:
	// Default materials
	const static Material Emerald;
	const static Material Chrome;
	const static Material Turquoise;
	const static Material Gold;
	const static Material Silver;
	const static Material Bronze;
	const static Material Ruby;
	const static Material Sapphire;
	const static Material Topaz;
	const static Material Prune;

	// These vectors are the colors of the different lights
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	
	// This impact the radius of the specular light
	float Shininess = 0.6f;

	// Operator to assign default materials

private:
	// Private constructor to set up default materials
	Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess);
};