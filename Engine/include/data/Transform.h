#pragma once

#include <glm/glm.hpp>

struct Transform
{
	Transform() :
		Position(glm::vec3(0)),
		Rotation(glm::vec3(0)),
		Scale(glm::vec3(1))
	{
	}

	Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) :
		Position(position),
		Rotation(rotation),
		Scale(scale)
	{
	}

	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;
};