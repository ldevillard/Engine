#include "data/Material.h"

#pragma region Default Materials

const Material Material::Emerald(glm::vec3(0.0215f, 0.1745f, 0.0215f), // Ambient
	glm::vec3(0.07568f, 0.61424f, 0.07568f), // Diffuse
	glm::vec3(0.633f, 0.727811f, 0.633f), // Specular
	0.6f); // Shininess

const Material Material::Chrome(glm::vec3(0.25f, 0.25f, 0.25f), // Ambient
	glm::vec3(0.4f, 0.4f, 0.4f), // Diffuse
	glm::vec3(0.774597f, 0.774597f, 0.774597f), // Specular
	0.6f); // Shininess

const Material Material::Turquoise(glm::vec3(0.1f, 0.18725f, 0.1745f), // Ambient
	glm::vec3(0.396f, 0.74151f, 0.69102f), // Diffuse
	glm::vec3(0.297254f, 0.30829f, 0.306678f), // Specular
	0.1f); // Shininess

const Material Material::Gold(glm::vec3(0.24725f, 0.1995f, 0.0745f), // Ambient
	glm::vec3(0.75164f, 0.60648f, 0.22648f), // Diffuse
	glm::vec3(0.628281f, 0.555802f, 0.366065f), // Specular
	0.4f); // Shininess

const Material Material::Silver(glm::vec3(0.19225f, 0.19225f, 0.19225f), // Ambient
	glm::vec3(0.50754f, 0.50754f, 0.50754f), // Diffuse
	glm::vec3(0.508273f, 0.508273f, 0.508273f), // Specular
	0.4f); // Shininess

const Material Material::Bronze(glm::vec3(0.2125f, 0.1275f, 0.054f), // Ambient
	glm::vec3(0.714f, 0.4284f, 0.18144f), // Diffuse
	glm::vec3(0.393548f, 0.271906f, 0.166721f), // Specular
	0.2f); // Shininess

const Material Material::Ruby(glm::vec3(0.1745f, 0.01175f, 0.01175f), // Ambient
	glm::vec3(0.61424f, 0.04136f, 0.04136f), // Diffuse
	glm::vec3(0.727811f, 0.626959f, 0.626959f), // Specular
	0.6f); // Shininess

const Material Material::Sapphire(glm::vec3(0.01175f, 0.01175f, 0.1745f), // Ambient
	glm::vec3(0.04136f, 0.04136f, 0.61424f), // Diffuse
	glm::vec3(0.626959f, 0.626959f, 0.727811f), // Specular
	0.6f); // Shininess

const Material Material::Topaz(glm::vec3(0.2f, 0.2f, 0.2f), // Ambient
	glm::vec3(0.8f, 0.8f, 0.8f), // Diffuse
	glm::vec3(0.8f, 0.8f, 0.8f), // Specular
	0.4f); // Shininess

const Material Material::Prune(glm::vec3(0.439216f, 0.172549f, 0.521569f), // Ambient
	glm::vec3(0.615686f, 0.286275f, 0.729412f), // Diffuse
	glm::vec3(0.811804f, 0.811804f, 0.811804f), // Specular
	0.4f); // Shininess


#pragma endregion

#pragma region Private Methods

Material::Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess)
	: Ambient(ambient), Diffuse(diffuse), Specular(specular), Shininess(shininess)
{
}

#pragma endregion