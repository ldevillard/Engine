#include "maths/Math.h"

#include <maths/glm/ext/matrix_projection.hpp>

namespace Math
{
	glm::vec3 ScreenToWorldPoint(const glm::vec2& screenPos, const glm::mat4& view, const glm::mat4& projection, const glm::vec4& viewport)
	{
		glm::vec3 win(screenPos.x, viewport.w - screenPos.y, 0.0f);
		glm::vec3 point = glm::unProject(win, view, projection, viewport);
		return point;
	}
}