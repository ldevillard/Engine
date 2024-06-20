#pragma once

#include <limits>
#include <maths/glm/glm.hpp>

struct HitInfo
{
	bool hit = false;
	glm::vec3 hitPoint = glm::vec3(0);
	float distance = std::numeric_limits<float>::max();
};