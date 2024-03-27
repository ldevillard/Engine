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

private:
	LightType lightType;
	Color	  color;

	void computeDirectional();
	void computePoint();
	void computeSpot();
};