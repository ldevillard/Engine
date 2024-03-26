#include "utils/Gizmo.h"

#include "system/GlobalSettings.h"

#include "data/mesh/WireCube.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// initialize static variables
std::map<GizmoType, std::unique_ptr<Mesh>> Gizmo::gizmos;
Shader* Gizmo::shader = nullptr;

#pragma region Public Methods

void Gizmo::InitGizmos(Shader* s)
{
	shader = s;
	gizmos[WireCubeGizmo] = std::make_unique<WireCube>();
	gizmos[WireSphereGizmo] = std::make_unique<WireSphere>();
}

// Need to make a function for shader binding
void Gizmo::DrawWireCube(const Color& color, const Transform& transform)
{
	if (!shader)
	{
		std::cerr << "Gizmo shader is not initialized" << std::endl;
		return;
	}

	bindShader(color, transform);

	gizmos[WireCubeGizmo]->Draw(shader);
}

void Gizmo::DrawWireSphere(const Color& color, const Transform& transform)
{
	if (!shader)
	{
		std::cerr << "Gizmo shader is not initialized" << std::endl;
		return;
	}

	bindShader(color, transform);

	gizmos[WireSphereGizmo]->Draw(shader);
}

#pragma endregion

#pragma region Private Methods

void Gizmo::bindShader(const Color& color, const Transform& transform)
{
	shader->Use();

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shader->SetMat4("projection", projection);
	shader->SetMat4("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, transform.Position);

	float angleX = glm::radians(transform.Rotation.x);
	float angleY = glm::radians(transform.Rotation.y);
	float angleZ = glm::radians(transform.Rotation.z);

	model = glm::rotate(model, angleX, glm::vec3(1.0f, 0.f, 0.f));
	model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.f, 0.f));
	model = glm::rotate(model, angleZ, glm::vec3(0.0f, 0.f, 1.f));

	model = glm::scale(model, transform.Scale);

	shader->SetMat4("model", model);

	shader->SetVec3("color", color.Value);
}

#pragma endregion