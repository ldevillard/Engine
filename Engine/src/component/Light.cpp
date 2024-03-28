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
	shader->Use();
	// binding light data
	shader->SetVec3("light.direction", transform->GetForwardVector());
	shader->SetVec3("light.color", color.Value);
	shader->SetFloat("light.intensity", Intensity);
	
	// draw gizmo
	Transform tr(*transform);
	tr.Scale *= 0.5f;
	Gizmo::DrawArrow(Color::Yellow, tr);
}

void Light::computePoint()
{

}

void Light::computeSpot()
{

}

#pragma endregion