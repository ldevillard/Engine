#pragma once

#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/matrix_transform.hpp>

#include "component/Transform.h"
#include "data/Color.h"

class BoundingBox
{
public:
	glm::vec3 Min;
	glm::vec3 Max;
	glm::vec3 Center;

	BoundingBox();
	BoundingBox(const glm::vec3& min, const glm::vec3& max);

	void DrawDebug(const Transform& transform, const Color& color = Color::Green);
};