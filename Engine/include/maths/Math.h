#pragma once

#include <maths/glm/glm.hpp>
#include <maths/glm/gtc/quaternion.hpp>
#include <maths/glm/gtc/matrix_transform.hpp>

namespace Math
{
	// z of screenPos is the depth value
	glm::vec3 ScreenToWorldPoint(const glm::vec2& screenPos, const glm::mat4& view, const glm::mat4& projection, const glm::vec4& viewport);
}