#pragma once

#include <vector>

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

	void SetLightTypeFromString(const std::string& type);
	static const std::vector<const char*> Names; // = { "Directional", "Point", "Spot" };

	void SetIndex(unsigned int i);

	LightType lightType;
	Color	  color;
	float	  Intensity = 1.0f;

	// point light
	float	  Radius = 1.0f;

private:
	unsigned int index = 0;

	void computeDirectional();
	void computePoint();
	void computeSpot();
};