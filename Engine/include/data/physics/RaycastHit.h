#pragma once

#include <maths/glm/glm.hpp>
#include "component/physics/EditorCollider.h"
#include "data/physics/HitInfo.h"

struct RaycastHit
{
	HitInfo hitInfo;
	EditorCollider* editorCollider = nullptr;

	// add collider pointer when physics is implemented
};