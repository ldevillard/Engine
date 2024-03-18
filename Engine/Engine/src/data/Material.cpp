#include "data/Material.h"

#pragma region Public Methods

void Material::SetAsChrome()
{
	 Ambient = glm::vec3(.25f, .25f, .25f);
	 Diffuse = glm::vec3(.4f, .4f, .4f);
	 Specular = glm::vec3(0.774597f, 0.774597f, 0.774597f);
	 Shininess = 0.6f;
}

void Material::SetAsEmerald()
{
	Ambient = glm::vec3(0.0215f, 0.1745f, 0.0215f);
	Diffuse = glm::vec3(0.07568f, 0.61424f, 0.07568f);
	Specular = glm::vec3(0.633f, 0.727811f, 0.633f);
	Shininess = 0.6f;
}

void Material::SetAsTurquoise()
{
	Ambient = glm::vec3(0.1f, 0.18725f, 0.1745f);
	Diffuse = glm::vec3(0.396f, 0.74151f, 0.69102f);
	Specular = glm::vec3(0.297254f, 0.30829f, 0.306678f);
	Shininess = 0.1f;
}

#pragma endregion