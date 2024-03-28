#pragma once

#include "component/Component.h"
#include "data/Color.h"

class Light : public Component
{
public:
	enum LightType
	{
		Directional,
		Point,
		Spot
	};

	Light(const LightType& type = Directional, const Color& color = Color::White);

	void Compute() override;

	LightType lightType;
	Color	  color;
	float	  Intensity = 1.0f;

private:
	void computeDirectional();
	void computePoint();
	void computeSpot();
};