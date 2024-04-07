#pragma once

#include <maths/glm/glm.hpp>
#include "component/physics/EditorCollider.h"

class EditorCollider;

struct RaycastHit
{
	bool hit = false;

	glm::vec3 point = glm::vec3(0);
	float distance = std::numeric_limits<float>::max();

	EditorCollider* editorCollider = nullptr;

	// add collider pointer when physics is implemented
};