#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "data/Transform.h"

class BoundingBox
{
public:
	glm::vec3 Min;
	glm::vec3 Max;
	glm::vec3 Center;

	BoundingBox();
	BoundingBox(const glm::vec3& min, const glm::vec3& max);

	void ApplyTransform(const Transform& transform);
};