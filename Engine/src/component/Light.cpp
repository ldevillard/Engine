#include "component/Light.h"
#include "utils/Gizmo.h"

#pragma region Public Methods

Light::Light(const LightType& type, const Color& color) : Component(),
	lightType(type), color(color)
{

}

void Light::Compute()
{
	shader->SetInt("light.type", static_cast<int>(lightType));

	switch (lightType)
	{
		case Light::Directional:
			computeDirectional();
			break;
		case Light::Point:
			computePoint();
			break;
		case Light::Spot:
			computeSpot();
			break;
	}
}

#pragma endregion

#pragma region Private Methods

void Light::computeDirectional()
{
	// binding light data
	shader->SetVec3("light.direction", transform->GetForwardVector());
	shader->SetVec3("light.color", color.Value);

	// draw gizmo
	glm::vec3 scale = glm::vec3(0.1f, 0.1f, 1);
	Transform tr(transform->Position, transform->Rotation, scale);
	Gizmo::DrawWireCube(Color::Yellow, tr);
}

void Light::computePoint()
{

}

void Light::computeSpot()
{

}

#pragma endregion